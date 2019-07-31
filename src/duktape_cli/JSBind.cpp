#include "pch.h"

#include "JSBind.h"
#include "LtkWindow.h"
#include "LtkApi.h"
#include "LtkEdit.h"
#include "LtkFont.h"
#include "LtkButton.h"

duk_context *g_ctx = nullptr;

inline void Rect::close() 
{ 
	dukglue_invalidate_object(g_ctx, this);
	delete this;
}

TestClass::TestClass(int i) : m_i(i)
{

}

void TestClass::Print()
{
	printf("TestClass: %d\n", m_i);
}

inline void TestClass::SetName(const char * val) { m_name = val; }

inline const char * TestClass::GetName() const { return m_name.c_str(); }

inline void TestClass::SetRect(const Rect & r) { m_rect = r; }

void TestClass::close()
{
	dukglue_invalidate_object(g_ctx, this);
	delete this;
}

//std::function<void(int)>&& lambda_test()
//{
//	std::unique_ptr<int> ptr(new int);
//	*ptr = 17;
//	auto fn2 = [ptr{ move(ptr) }](int v) {
//		printf("lambda_test %d\n", *ptr * v);
//	};
//	std::function<void(int)> func = std::move(fn2);
//	return std::move(func);
//}

void JSBindInit(duk_context *ctx)
{
	//auto fn = lambda_test();
	//fn(3);

	g_ctx = ctx;
	dukglue_register_constructor<TestClass, int>(ctx, "TestClass");
	dukglue_register_method(ctx, &TestClass::Print, "Print");
	dukglue_register_method(ctx, &TestClass::SetRect, "SetRect");
	dukglue_register_method(ctx, &TestClass::close, "close");
	dukglue_register_property(ctx, &TestClass::GetName, &TestClass::SetName, "name");

	dukglue_register_constructor<Rect, float, float, float, float>(ctx, "Rect");
	dukglue_register_method(ctx, &Rect::close, "close");

    dukglue_register_function(ctx, &LtkApi::RunMessageLoop, "RunMessageLoop");
    dukglue_register_function(ctx, &LtkApi::PostQuitMessage, "PostQuitMessage");

    dukglue_register_function(ctx, &LtkWindow::RegisterWndClass, "RegisterWndClass");
    dukglue_register_constructor<LtkWindow>(ctx, "LtkWindow");
    dukglue_register_method(ctx, &LtkWindow::close, "close");
    dukglue_register_method(ctx, &LtkWindow::Create, "Create");
    dukglue_register_method(ctx, &LtkWindow::SetVisible, "SetVisible");
    dukglue_register_method(ctx, &LtkWindow::GetText, "GetText");
    dukglue_register_method(ctx, &LtkWindow::SetText, "SetText");
    dukglue_register_method(ctx, &LtkWindow::SetFont, "SetFont");

    dukglue_register_constructor<LtkEdit>(ctx, "LtkEdit");
    dukglue_set_base_class<LtkWindow, LtkEdit>(ctx);

    dukglue_register_constructor<LtkButton>(ctx, "LtkButton");
    dukglue_set_base_class<LtkWindow, LtkButton>(ctx);

    dukglue_register_constructor<LtkFont>(ctx, "LtkFont");
    dukglue_register_method(ctx, &LtkFont::close, "close");
    dukglue_register_method(ctx, &LtkFont::CreateSimple, "CreateSimple");

}

