#include <tchar.h>
#include <windows.h>

#include "wnd_proc.h"
#include "resource.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "comctl32.lib")


// [_tWinMain]: entry point.
int WINAPI _tWinMain(
    _In_        HINSTANCE hInstance,
    _In_opt_    HINSTANCE hPrevInstance,
    _In_        TCHAR* lpCmdLine,
    _In_        int nShowCmd
)
{
    const TCHAR szWindowName[] = _T("SCLC");
    const TCHAR szClassName[] = _T("__sclc__class__");
    const TCHAR szMutexName[] = _T("__sclc__mutex__");

    HANDLE hMutex = CreateMutex(NULL, FALSE, szMutexName);
    if (hMutex == NULL)
    {
        MessageBox(NULL, _T("Could not create mutex."), _T("Error!"), MB_OK | MB_ICONERROR | MB_TOPMOST);
        return 1;
    }
    else
    {
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            MessageBox(NULL, _T("Ёкземпл€р программы уже запущен."), _T("ќшибка!"), MB_OK | MB_ICONERROR | MB_TOPMOST);

            HWND hWnd = FindWindow(szClassName, szWindowName);
            if (hWnd)
            {
                if (IsIconic(hWnd))
                {
                    ShowWindow(hWnd, SW_RESTORE);
                }
                SetForegroundWindow(hWnd);
            }

            CloseHandle(hMutex);
            return 1;
        }
    }

    DWORD dwStyle = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME;
    DWORD dwExStyle = WS_EX_APPWINDOW;

    RECT rc;
    SetRect(&rc, 0, 0, 840, 500);
    AdjustWindowRectEx(&rc, dwStyle, FALSE, dwExStyle);

    const int iWindowWidth = rc.right - rc.left;
    const int iWindowHeight = rc.bottom - rc.top;

    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);

    WNDCLASSEX wcex;
    memset(&wcex, 0, sizeof(WNDCLASSEX));

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProcedure;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szClassName;
    wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, _T("Window Registration Failed."), _T("Error!"), MB_OK | MB_ICONERROR | MB_TOPMOST);
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        return 1;
    }

    HWND hWnd = CreateWindowEx(
        dwExStyle,
        szClassName,
        szWindowName,
        dwStyle,
        (GetSystemMetrics(SM_CXSCREEN) >> 1) - (iWindowWidth >> 1),
        ((rc.bottom - rc.top) >> 1) - (iWindowHeight >> 1),
        iWindowWidth,
        iWindowHeight,
        HWND_DESKTOP,
        NULL,
        hInstance,
        NULL);

    if (!hWnd)
    {
        MessageBox(NULL, _T("Window Creation Failed."), _T("Error!"), MB_OK | MB_ICONERROR | MB_TOPMOST);
        UnregisterClass(szClassName, hInstance);
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
        return 1;
    }

    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);

    MSG msg;
    BOOL bRet;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            break;
        }
        else
        {
            DispatchMessage(&msg);
            TranslateMessage(&msg);
        }
    }

    UnregisterClass(szClassName, hInstance);
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);
    return (int)msg.wParam;
}
// [/_tWinMain]
