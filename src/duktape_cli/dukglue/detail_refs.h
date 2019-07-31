#pragma once

#include <duktape.h>

#include <unordered_map>
#include <algorithm>

namespace dukglue
{
	namespace detail
	{
		// This class handles keeping a map of void* -> script object.
		// It also prevents script objects from being GC'd until someone
		// explicitly frees the underlying native object.

		// Implemented by keeping an array of script objects in the heap stash.
		// An std::unordered_map maps pointer -> array index.
		// Thanks to std::unordered_map, lookup time is O(1) on average.

		// Using std::unordered_map has some memory overhead (~32 bytes per object),
		// which could be removed by using a different data structure:

		// 1. Use no data structure. Blindly scan through the reference registry,
		//    checking \xFFobj_ptr on every object until you find yours.
		//    Performance when returning native objects from functions when a lot
		//    of native objects are registered will suffer.

		// 2. Implement a self-balancing binary tree on top of a Duktape array
		//    for the registry. Still fast - O(log(N)) - and no memory overhead.
		
		// 3. A sorted list would work too, though insertion speed might be worse
		//    than a binary tree.

		struct RefManager
		{
		public:

			// Find the script object corresponding to obj_ptr and push it.
			// Returns true if successful, false if obj_ptr has not been registered.
			// Stack: ... -> ...              (if object has been registered before)
			//        ... -> ... [object]     (if object has not been registered)
			static bool find_and_push_native_object(duk_context* ctx, void* obj_ptr)
			{
				RefMap* ref_map = get_ref_map(ctx);

				const auto it = ref_map->find(obj_ptr);

				if (it == ref_map->end()) {
					return false;
				} else {
					push_ref_array(ctx);
					duk_get_prop_index(ctx, -1, it->second);
					duk_remove(ctx, -2);
					return true;
				}
			}

			// Takes a script object and adds it to the registry, associating
			// it with obj_ptr. unregistered_object is not modified.
			// If obj_ptr has already been registered with another object,
			// the old registry entry will be overidden.
			// Does nothing if obj_ptr is NULL.
			// Stack: ... [object]  ->  ... [object]
			static void register_native_object(duk_context* ctx, void* obj_ptr)
			{
				if (obj_ptr == NULL)
					return;

				RefMap* ref_map = get_ref_map(ctx);

				push_ref_array(ctx);

				// find next free index
				// free indices are kept in a linked list, starting at ref_array[0]
				duk_get_prop_index(ctx, -1, 0);
				duk_uarridx_t next_free_idx = duk_get_uint(ctx, -1);
				duk_pop(ctx);

				if (next_free_idx == 0) {
					// no free spots in the array, make a new one at arr.length
					next_free_idx = duk_get_length(ctx, -1);
				} else {
					// free spot found, need to remove it from the free list
					// ref_array[0] = ref_array[next_free_idx]
					duk_get_prop_index(ctx, -1, next_free_idx);
					duk_put_prop_index(ctx, -2, 0);
				}

				// std::cout << "putting reference at ref_array[" << next_free_idx << "]" << std::endl;
				(*ref_map)[obj_ptr] = next_free_idx;

				duk_dup(ctx, -2);  // put object on top

				// ... [object] [ref_array] [object]
				duk_put_prop_index(ctx, -2, next_free_idx);

				duk_pop(ctx);  // pop ref_array
			}
			
			// Stack: ... [trace_back] -> ...
			static void inc_native_alloc_count(duk_context* ctx)
			{
				int count = 1;
				push_native_leaks_array(ctx); // [trace_back] [array]
				duk_dup(ctx, -2); // [trace_back] [array] [trace_back]
				if (duk_get_prop(ctx, -2)) {
					// [trace_back] [array] [count]
					count = duk_require_int(ctx, -1);
					count++;
					duk_pop(ctx); // [trace_back] [array]
				}
				else {
					duk_pop(ctx); // [trace_back] [array]
				}
				duk_dup(ctx, -2); // [trace_back] [array] [trace_back]
				duk_push_int(ctx, count); // [trace_back] [array] [trace_back] [count]
				duk_put_prop(ctx, -3);  // [trace_back] [array]
				duk_pop_2(ctx);
			}

			// Stack: ... [trace_back] -> ...
			static void dec_native_alloc_count(duk_context* ctx)
			{
				push_native_leaks_array(ctx); // [trace_back] [array]
				duk_dup(ctx, -2); // [trace_back] [array] [trace_back]
				if (duk_get_prop(ctx, -2)) {
					// [trace_back] [array] [count]
					int count = duk_require_int(ctx, -1);
					count--;
					duk_pop(ctx); // [trace_back] [array]

					duk_dup(ctx, -2); // [trace_back] [array] [trace_back]
					duk_push_int(ctx, count); // [trace_back] [array] [trace_back] [count]
					duk_put_prop(ctx, -3);  // [trace_back] [array]
				}
				else {
					duk_pop(ctx); // [trace_back] [array]
				}
				duk_pop_2(ctx);
			}

			// Remove the object associated with obj_ptr from the registry
			// and invalidate the object's internal native pointer (by setting it to undefined).
			// Does nothing if obj_ptr if object was never registered or obj_ptr is NULL.
			// Does not affect the stack.
			static void find_and_invalidate_native_object(duk_context* ctx, void* obj_ptr)
			{
				if (obj_ptr == NULL)
					return;

				RefMap* ref_map = get_ref_map(ctx);
				auto it = ref_map->find(obj_ptr);
				if (it == ref_map->end())  // was never registered
					return;

				push_ref_array(ctx);
				duk_get_prop_index(ctx, -1, it->second);

				// [object]
				duk_get_prop_string(ctx, -1, "\xFF" "trace_back"); // [object] [trace_back]
				//const char *trace = duk_require_string(ctx, -1);
				//printf("native trace: %s\n", trace);
				dec_native_alloc_count(ctx); // [object]

				// invalidate internal pointer
				duk_push_undefined(ctx);
				duk_put_prop_string(ctx, -2, "\xFF" "obj_ptr");
				duk_pop(ctx);  // pop object

				// remove from references array and add the space it was in to free list
				// (refs[0] -> tail) -> (refs[0] -> old_obj_idx -> tail)

				// refs[old_obj_idx] = refs[0]
				duk_get_prop_index(ctx, -1, 0);
				duk_put_prop_index(ctx, -2, it->second);

				// refs[0] = old_obj_idx
				duk_push_uint(ctx, it->second);
				duk_put_prop_index(ctx, -2, 0);

				duk_pop(ctx);  // pop ref_array

				// also remove from map
				// std::cout << "Freeing ref_array[" << it->second << "]" << std::endl;
				ref_map->erase(it);
			}

		private:
			typedef std::unordered_map<void*, duk_uarridx_t> RefMap;

			static RefMap* get_ref_map(duk_context* ctx)
			{
				static const char* DUKGLUE_REF_MAP = "dukglue_ref_map";
				static const char* PTR = "ptr";

				duk_push_heap_stash(ctx);

				if (!duk_has_prop_string(ctx, -1, DUKGLUE_REF_MAP)) {
					// doesn't exist yet, need to create it
					duk_push_object(ctx);

					duk_push_pointer(ctx, new RefMap());
					duk_put_prop_string(ctx, -2, PTR);

					duk_push_c_function(ctx, ref_map_finalizer, 1);
					duk_set_finalizer(ctx, -2);

					duk_put_prop_string(ctx, -2, DUKGLUE_REF_MAP);
				}

				duk_get_prop_string(ctx, -1, DUKGLUE_REF_MAP);
				duk_get_prop_string(ctx, -1, PTR);
				RefMap* map = static_cast<RefMap*>(duk_require_pointer(ctx, -1));
				duk_pop_3(ctx);

				return map;
			}

			static duk_ret_t ref_map_finalizer(duk_context* ctx)
			{
				duk_get_prop_string(ctx, 0, "ptr");
				RefMap* map = static_cast<RefMap*>(duk_require_pointer(ctx, -1));
				delete map;

				return 0;
			}

			static void push_ref_array(duk_context* ctx)
			{
				static const char* DUKGLUE_REF_ARRAY = "dukglue_ref_array";
				duk_push_heap_stash(ctx);

				if (!duk_has_prop_string(ctx, -1, DUKGLUE_REF_ARRAY)) {
					duk_push_array(ctx);

					// ref_array[0] = 0 (initialize free list as empty)
					duk_push_int(ctx, 0);
					duk_put_prop_index(ctx, -2, 0);

					duk_put_prop_string(ctx, -2, DUKGLUE_REF_ARRAY);
				}

				duk_get_prop_string(ctx, -1, DUKGLUE_REF_ARRAY);
				duk_remove(ctx, -2); // pop heap stash
			}

			// Stack: ... -> ... [array]
			static void push_native_leaks_array(duk_context* ctx)
			{
				static const char* DUKGLUE_NATIVE_LEAKS_ARRAY = "dukglue_native_leaks";
				duk_push_heap_stash(ctx); // [stash]

				if (!duk_has_prop_string(ctx, -1, DUKGLUE_NATIVE_LEAKS_ARRAY)) {
					duk_push_array(ctx); // [stash] [array]
					duk_push_c_function(ctx, native_leak_finalizer, 1); // [stash] [array] [finalizer]
					duk_set_finalizer(ctx, -2); // [stash] [array]
					duk_put_prop_string(ctx, -2, DUKGLUE_NATIVE_LEAKS_ARRAY); // [stash]
				}

				duk_get_prop_string(ctx, -1, DUKGLUE_NATIVE_LEAKS_ARRAY); // [stash] [array]
				duk_remove(ctx, -2); // [array]
			}

			static duk_ret_t native_leak_finalizer(duk_context* ctx)
			{
				printf("native_leak_finalizer\n");
				std::tuple<std::string, int> tup;
				std::vector<decltype(tup)> vec;
				duk_enum(ctx, -1, 0);
				while (duk_next(ctx, -1, 1)) {
					std::get<0>(tup) = duk_require_string(ctx, -2);
					std::get<1>(tup) = duk_require_int(ctx, -1);
					duk_pop_2(ctx);
					if (std::get<1>(tup) != 0) {
						vec.push_back(tup);
					}
				}
				std::sort(vec.begin(), vec.end(), [](const decltype(tup) &lhs, const decltype(tup) &rhs) -> bool {
					return std::get<1>(lhs) < std::get<1>(rhs);
				});
				for (const auto &item : vec) {
					printf("%d leaks, stack: %s\n", std::get<1>(item), std::get<0>(item).c_str());
				}
				_CrtCheckMemory();
				return 0;
			}
		};
	}
}
