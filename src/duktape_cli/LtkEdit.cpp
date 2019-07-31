#include "pch.h"
#include "LtkEdit.h"

extern duk_context *g_ctx;

LtkEdit::LtkEdit()
{
    this->SetStyle(WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL);
}


LtkEdit::~LtkEdit()
{}

LPCWSTR LtkEdit::GetClassString() const
{
    return L"EDIT";
}

void LtkEdit::OnNotify(UINT id, NMHDR *hdr)
{
}

void LtkEdit::OnCommand(WPARAM wparam, LPARAM lparam)
{
    switch (HIWORD(wparam)) {
    case EN_CHANGE:
        dukglue_pcall_method2<void>(g_ctx, this, "OnChange");
        break;
    }
}
