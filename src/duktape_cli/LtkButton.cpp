#include "pch.h"
#include "LtkButton.h"

extern duk_context *g_ctx;

LtkButton::LtkButton()
{
    this->SetStyle(WS_CHILD | WS_VISIBLE);
}


LtkButton::~LtkButton()
{}

LPCWSTR LtkButton::GetClassString() const
{
    return L"BUTTON";
}

void LtkButton::OnCommand(WPARAM wparam, LPARAM lparam)
{
    dukglue_pcall_method2<void>(g_ctx, this, "OnClick");
}

