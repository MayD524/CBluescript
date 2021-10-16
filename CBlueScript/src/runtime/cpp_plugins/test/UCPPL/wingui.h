// https://stackoverflow.com/questions/13977388/error-cannot-convert-const-wchar-t-13-to-lpcstr-aka-const-char-in-assi
#pragma once

#ifndef WINGUI_H
#define WINGUI_H

#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <WinUser.h>
#include <string>

using namespace std;

LPCWSTR stringToWideString(const string& s)
{
    wstring sTemp = wstring(s.begin(), s.end());
    LPCWSTR sw = sTemp.c_str();
    return sw;
}




#endif