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

std::string Utf16ToUtf8(LPCWSTR strW, int len)
{
    if (len < 0) {
        len = (int)wcslen(strW);
    }
    std::string strA;
    int lenA = UTF8Length(strW, len);
    strA.resize(lenA);
    UTF8FromUTF16(strW, len, &strA[0], lenA);
    return std::move(strA);
}