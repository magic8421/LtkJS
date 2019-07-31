#pragma once
#include "LtkWindow.h"

class LtkEdit : public LtkWindow
{
public:
    LtkEdit();
    virtual ~LtkEdit();

    virtual LPCWSTR GetClassString() const override;

    virtual void OnNotify(UINT id, NMHDR *hdr) override;
    virtual void OnCommand(WPARAM wparam, LPARAM lparam) override;

};

