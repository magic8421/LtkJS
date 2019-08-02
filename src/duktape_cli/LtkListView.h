#pragma once
#include "LtkWindow.h"

class LtkListView : public LtkWindow
{
public:
    LtkListView();
    virtual ~LtkListView();

    virtual LPCWSTR GetClassString() const override;

    virtual void OnCommand(WPARAM wparam, LPARAM lparam) override;
    virtual void OnNotify(UINT id, NMHDR *hdr) override;

    void InsertColumn(int iSubItem, LPCSTR text, int cx, int format);
    void InsertItem(int iItem, LPCSTR text);
    void SetItem(int iItem, int iSubItem, LPCSTR text);
    void SetExtendedStyle(uint32_t style);
};

