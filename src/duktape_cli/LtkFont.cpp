#include "pch.h"
#include "LtkFont.h"
#include "Utils.h"

extern duk_context *g_ctx;

LtkFont::~LtkFont()
{
    if (m_font) {
        ::DeleteObject(m_font);
    }
    dukglue_invalidate_object(g_ctx, this);
}

void LtkFont::CreateSimple(const char* familyName, int size)
{
    if (m_font) {
        ::DeleteObject(m_font);
        m_font = 0;
    }
    LOGFONT lf;
    ZeroMemory(&lf, sizeof(lf));
    auto familyW = Utf8ToUtf16(familyName);
    if (familyW.size() >= 31) {
        return;
    }
    wcscpy_s(lf.lfFaceName, familyW.c_str());
    lf.lfHeight = size;
    m_font = ::CreateFontIndirectW(&lf);
    m_font = m_font;
}

