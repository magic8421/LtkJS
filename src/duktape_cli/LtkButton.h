#pragma once
#include "LtkWindow.h"

class LtkButton : public LtkWindow
{
public:
    LtkButton();
    virtual ~LtkButton();

    virtual LPCWSTR GetClassString() const override;
    virtual void OnCommand(WPARAM wparam, LPARAM lparam) override;



};

