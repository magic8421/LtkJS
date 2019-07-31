#pragma once

std::wstring Utf8ToUtf16(LPCSTR strA, int len = -1);

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);             \
	void operator=(const TypeName&)