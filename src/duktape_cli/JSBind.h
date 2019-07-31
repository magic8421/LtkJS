#pragma once
#include "duktape.h"
#include <string>

template<typename T>
struct Disposable {
    void close() {
        T *thiz = static_cast<T*>(this);
        dukglue_invalidate_object(g_ctx, thiz);
        delete thiz;
    }
};

struct Rect {
	Rect() : x(0.0f), y(0.0f), w(0.0f), h(0.0f) {}
	Rect(float _x, float _y, float _w, float _h) :
		x(_x), y(_y), w(_w), h(_h) {}
	float x;
	float y;
	float w;
	float h;
	void close();
};

class TestClass
{
public:
    TestClass(int i);
	void Print();

	void SetName(const char *val);
	const char * GetName() const;

	void SetRect(const Rect& r);

	void close();

private:
    int m_i = 0;
	std::string m_name;
	Rect m_rect;
};

void JSBindInit(duk_context *ctx);
