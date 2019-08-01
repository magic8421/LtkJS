#include "pch.h"
#include "LtkWindow.h"
#include "Utils.h"
#include "LtkFont.h"

extern duk_context *g_ctx;

LtkWindow::LtkWindow()
{
    m_style = WS_OVERLAPPEDWINDOW;
    m_exStyle = 0;
}

LtkWindow::~LtkWindow()
{
    dukglue_invalidate_object(g_ctx, this);
}

void LtkWindow::close()
{
    delete this;
}

LPCWSTR LtkWindow::GetClassString() const
{
    return L"ltk_cls";
}

void LtkWindow::Create(LtkWindow *parent, int x, int y, int w, int h)
{
    HWND hParent = NULL;
    if (parent) {
        hParent = parent->m_hwnd;
    }
    auto ret = ::CreateWindowEx(m_exStyle, GetClassString(), L"", m_style,
        x, y, w, h,
        hParent, NULL, HINST_THISCOMPONENT, this);
    m_hwnd = ret;
    // 这里设置了两次 一次是在 WM_NCCREATE 里面 有点奇怪 2019-7-31 17:08:31
    ::SetWindowLongPtr(m_hwnd, GWLP_USERDATA,
        reinterpret_cast<LPARAM>(this));
    dukglue_pcall_method2<void>(g_ctx, this, "OnCreate");
}

void LtkWindow::SetVisible(bool v)
{
    if (v) {
        ::ShowWindow(m_hwnd, SW_SHOW);
    } else {
        ::ShowWindow(m_hwnd, SW_HIDE);
    }
}

std::string LtkWindow::GetText()
{
    std::wstring strW;
    strW.resize(::GetWindowTextLengthW(m_hwnd) + 1);
    ::GetWindowText(m_hwnd, &strW[0], strW.size() + 1);
    return std::move(Utf16ToUtf8(strW.c_str(), strW.size()));
}

void LtkWindow::SetText(LPCSTR text)
{
    auto textW = Utf8ToUtf16(text);
    ::SetWindowText(m_hwnd, textW.c_str());
}

void LtkWindow::SetStyle(DWORD style)
{
    m_style = style;
    if (m_hwnd) {
        ::SetWindowLong(m_hwnd, GWL_STYLE, style);
    }
}

void LtkWindow::SetExStyle(DWORD exStyle)
{
    m_exStyle = exStyle;
    if (m_hwnd) {
        ::SetWindowLong(m_hwnd, GWL_EXSTYLE, exStyle);
    }
}

void LtkWindow::SetFont(LtkFont *font, bool bRedraw)
{
    ::SendMessage(m_hwnd, WM_SETFONT, (WPARAM)font->Get(), bRedraw);
}

void LtkWindow::OnNotify(UINT id, NMHDR *hdr)
{
    LtkWindow *wnd = reinterpret_cast<LtkWindow *>
        (GetWindowLongPtr(hdr->hwndFrom, GWLP_USERDATA));
    if (wnd && wnd != this) {
        wnd->OnNotify(id, hdr);
    }
}

void LtkWindow::OnCommand(WPARAM wparam, LPARAM lparam)
{
    if (lparam) {
        HWND hwnd = (HWND)lparam;
        LtkWindow *wnd = reinterpret_cast<LtkWindow *>
            (GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (wnd && wnd != this) {
            wnd->OnCommand(wparam, lparam);
        }
    }
}

LRESULT LtkWindow::WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    auto ctx = g_ctx;
    switch (message) {
    /*case WM_CREATE:
        dukglue_pcall_method2<void>(ctx, this, "OnCreate"); 
        break;*/
    case WM_CLOSE:
        dukglue_pcall_method2<void>(ctx, this, "OnClose");
        break;
    case WM_COMMAND:
        OnCommand(wparam, lparam);
        break;
    case WM_NOTIFY:
        // TODO test
        this->OnNotify(wparam, (NMHDR*)lparam);
        break;
    }
    return ::DefWindowProc(hwnd, message, wparam, lparam);
}

LRESULT CALLBACK LtkWindow::WndProcStatic(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    LtkWindow *thiz = nullptr;

    //if (WM_NCCREATE == message) {
    //    LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lparam);
    //    thiz = reinterpret_cast<LtkWindow*>(lpcs->lpCreateParams);
    //    thiz->m_hwnd = hwnd;
    //    SetWindowLongPtr(hwnd, GWLP_USERDATA,
    //        reinterpret_cast<LPARAM>(thiz));
    //} else {
        thiz = reinterpret_cast<LtkWindow *>
            (GetWindowLongPtr(hwnd, GWLP_USERDATA));
    //}
    if (!thiz) {
        //LTK_LOG("WndProc thiz is NULL, message: %d", message);
        return ::DefWindowProc(hwnd, message, wparam, lparam);
    }
    return thiz->WndProc(hwnd, message, wparam, lparam);
}

void LtkWindow::RegisterWndClass()
{
    WNDCLASS wc;
    wc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = WndProcStatic;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = HINST_THISCOMPONENT;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // 如果这里改成NULL 可以防止系统去擦除背景(闪烁) 双缓冲 还有clip children clip sibling
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"ltk_cls";

    ATOM a = RegisterClass(&wc);
}
