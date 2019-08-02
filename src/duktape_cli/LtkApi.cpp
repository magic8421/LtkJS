#include "pch.h"
#include "LtkApi.h"

namespace LtkApi {

void RunMessageLoop()
{
    MSG msg;
    BOOL bRet;
    while ((bRet = ::GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) {
        } else {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
}

void PostQuitMessage()
{
    ::PostQuitMessage(0);
}

struct ConstantItem {
    LPCSTR name;
    DWORD value;
};

#define CONST_ENTRY(x) {#x, x},
duk_ret_t GetConstantTable(duk_context *ctx)
{
    struct ConstantItem values[] = {
        CONST_ENTRY(WS_OVERLAPPED)
        CONST_ENTRY(WS_POPUP)
        CONST_ENTRY(WS_CHILD)
        CONST_ENTRY(WS_MINIMIZE)
        CONST_ENTRY(WS_VISIBLE)
        CONST_ENTRY(WS_DISABLED)
        CONST_ENTRY(WS_DLGFRAME)
        CONST_ENTRY(WS_VSCROLL)
        CONST_ENTRY(WS_HSCROLL)
        CONST_ENTRY(WS_SYSMENU)
        CONST_ENTRY(WS_THICKFRAME)
        CONST_ENTRY(WS_GROUP)
        CONST_ENTRY(WS_TABSTOP)
        CONST_ENTRY(WS_MINIMIZEBOX)
        CONST_ENTRY(WS_MAXIMIZEBOX)
        CONST_ENTRY(WS_OVERLAPPEDWINDOW)
        CONST_ENTRY(WS_POPUPWINDOW)

        CONST_ENTRY(LVS_EX_FULLROWSELECT)
        {NULL, 0}
    };
    duk_push_object(ctx); // obj
    for (UINT i = 0; values[i].name; i++) {
        duk_push_uint(ctx, values[i].value); // obj value
        duk_put_prop_string(ctx, -2, values[i].name); // obj
    }
    return 1;
}
#undef CONST_ENTRY

} // namespace LtkApi
