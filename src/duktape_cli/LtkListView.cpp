#include "pch.h"
#include "LtkListView.h"
#include "Utils.h"


LtkListView::LtkListView()
{
    this->SetStyle(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT);
}

LtkListView::~LtkListView()
{}

LPCWSTR LtkListView::GetClassString() const
{
    return WC_LISTVIEW;
}

void LtkListView::OnCommand(WPARAM wparam, LPARAM lparam)
{
}

void LtkListView::OnNotify(UINT id, NMHDR *hdr)
{
}

void LtkListView::InsertColumn(int iSubItem, LPCSTR text, int cx, int format)
{
    auto textW = Utf8ToUtf16(text);
    LVCOLUMN lvc = { 0 };

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    lvc.iSubItem = iSubItem;
    lvc.pszText = &textW[0];
    lvc.cx = cx;
    lvc.fmt = format;
    ListView_InsertColumn(GetHWND(), iSubItem, &lvc);
}

void LtkListView::InsertItem(int iItem, LPCSTR text)
{
    auto textW = Utf8ToUtf16(text);

    LVITEM item = { 0 };
    item.mask = LVIF_TEXT;
    item.iItem = iItem;
    item.pszText = &textW[0];
    ListView_InsertItem(GetHWND(), &item);
}

