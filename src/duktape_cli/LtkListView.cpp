#include "pch.h"
#include "LtkListView.h"


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


