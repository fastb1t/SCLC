#include "algorithms.h"

#include <commctrl.h>
#include <shlwapi.h>


HWND CreateToolTip(HINSTANCE hInstance, HWND hCtrlWnd, const TCHAR* pszText)
{
    if (!hCtrlWnd || !pszText)
    {
        return NULL;
    }

    HWND hTipWnd = CreateWindowEx(
        0,
        TOOLTIPS_CLASS,
        _T(""),
        WS_POPUP | TTS_ALWAYSTIP, // | TTS_BALLOON,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        GetParent(hCtrlWnd),
        NULL,
        hInstance,
        NULL
    );

    if (!hTipWnd)
    {
        return NULL;
    }

    TOOLINFO ti;
    memset(&ti, 0, sizeof(TOOLINFO));

    ti.cbSize = sizeof(TOOLINFO);
    ti.hwnd = GetParent(hCtrlWnd);
    ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    ti.uId = (UINT_PTR)hCtrlWnd;
    ti.lpszText = (LPSTR)pszText;

    SendMessage(hTipWnd, TTM_ADDTOOL, 0, (LPARAM)&ti);

    return hTipWnd;
}

String GetThisPath(HINSTANCE hInstance)
{
    TCHAR szTemp[2048] = { 0 };
    if (GetModuleFileName(hInstance, szTemp, 2048) > 0)
    {
        PathRemoveFileSpec(szTemp);
        return szTemp;
    }
    return _T("");
}

String BuildPath(const String& sPath1, const String& sPath2)
{
    if (sPath1.empty())
    {
        return sPath2;
    }

    if (sPath2.empty())
    {
        return sPath1;
    }

    if (*sPath1.rbegin() != '\\' && *sPath2.begin() != '\\')
    {
        return sPath1 + _T("\\") + sPath2;
    }

    if (*sPath1.rbegin() == '\\' && *sPath2.begin() == '\\')
    {
        return sPath1.substr(0, sPath1.size() - 1) + sPath2;
    }

    return sPath1 + sPath2;
}

bool FileExists(const String& sFileName)
{
    if (sFileName.empty())
    {
        return false;
    }

    WIN32_FIND_DATA wfd;
    HANDLE hFile = FindFirstFile(sFileName.c_str(), &wfd);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        FindClose(hFile);
        return true;
    }
    return false;
}

bool DirExists(const String& sDirName)
{
    DWORD dwAttr = GetFileAttributes(sDirName.c_str());
    if (dwAttr == INVALID_FILE_ATTRIBUTES)
    {
        return false;
    }
    if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
    {
        return true;
    }
    return false;
}

String GetFileExtension(const String& sFileName)
{
    TCHAR* lpStr = PathFindExtension(sFileName.c_str());
    return !lpStr ? _T("") : lpStr;
}

BOOL DrawLine(HDC hDC, int x0, int y0, int x1, int y1)
{
    MoveToEx(hDC, x0, y0, NULL);
    return LineTo(hDC, x1, y1);
}
