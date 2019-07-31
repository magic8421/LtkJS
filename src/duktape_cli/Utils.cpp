#include "pch.h"
#include "Utils.h"
#include "UniConversion.h"

std::wstring Utf8ToUtf16(LPCSTR strA, int len)
{
    if (len < 0) {
        len = (int)strlen(strA);
    }
    std::wstring strW;
    int lenW = UTF16Length(strA, len);
    strW.resize(lenW);
    UTF16FromUTF8(strA, len, &strW[0], lenW);
    return std::move(strW);
}