#include "pch.h"
#include "LtkApi.h"

void RunMessageLoop()
{
    MSG msg;
    BOOL bRet;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
        if (bRet == -1) {
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

void JsPostQuitMessage()
{
    PostQuitMessage(0);
}
