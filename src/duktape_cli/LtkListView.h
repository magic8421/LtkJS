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

};

