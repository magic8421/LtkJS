#pragma once
#include "JSBind.h"

class LtkFont;

class LtkWindow
{
public:
    LtkWindow();
    virtual ~LtkWindow();
    void close();

    virtual LPCWSTR GetClassString() const;

    LRESULT WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
    static LRESULT CALLBACK WndProcStatic(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
    static void RegisterWndClass();

    virtual void OnCommand(WPARAM wparam, LPARAM lparam);
    virtual void OnNotify(UINT id, NMHDR *hdr);

    void Create(LtkWindow *parent, int x, int y, int w, int h);
    void SetVisible(bool v);
    std::string GetText();
    void SetText(const char *text);
    void SetStyle(DWORD style);
    void SetExStyle(DWORD exStyle);
    void SetFont(LtkFont *font, bool bRedraw);

private:
    HWND m_hwnd = 0;
    DWORD m_style;
    DWORD m_exStyle;
};