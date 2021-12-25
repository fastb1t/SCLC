#ifndef _ALGORITHMS_H_
#define _ALGORITHMS_H_

#include <tchar.h>
#include <string>
#include <sstream>
#include <Windows.h>

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>> String;
typedef std::basic_stringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>> StringStream;

template<typename T>
String ToString(T t)
{
    StringStream ss;
    ss << t;
    return ss.str();
}

HWND CreateToolTip(HINSTANCE hInstance, HWND hCtrlWnd, const TCHAR* pszText);

String GetThisPath(HINSTANCE hInstance);
String BuildPath(const String& sPath1, const String& sPath2);
bool FileExists(const String& sFileName);
bool DirExists(const String& sDirName);
String GetFileExtension(const String& sFileName);

BOOL DrawLine(HDC hDC, int x0, int y0, int x1, int y1);

#endif
