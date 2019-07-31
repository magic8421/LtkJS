#pragma once
#include "Utils.h"

class LtkFont
{
public:
    LtkFont() {}
    ~LtkFont();
    void close() { delete this; }
    HFONT Get() { return m_font; }
    void CreateSimple(const char* familyName, int size);

private:
    HFONT m_font = 0;
    DISALLOW_COPY_AND_ASSIGN(LtkFont);
};