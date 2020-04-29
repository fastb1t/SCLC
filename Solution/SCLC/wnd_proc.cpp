#include <tchar.h>
#include <windows.h>
#include <windowsx.h>

#include "wnd_proc.h"
#include "resource.h"

static BOOL OnCreate(HWND, LPCREATESTRUCT);                                     	// WM_CREATE
static void OnCommand(HWND, int, HWND, UINT);                                      	// WM_COMMAND
static void OnPaint(HWND);                                                        	// WM_PAINT
static void OnDestroy(HWND);                                                        // WM_DESTROY

#define IDC_LIST                    1001
#define IDC_LIST_ADD                1002
#define IDC_LIST_DEL                1003
#define IDC_LIST_CLEAR              1004
#define IDC_IGNORE_EMPTY_LINE       1005
#define IDC_IGNORE_COMMENT_LINE     1006
#define IDC_IGNORE_DUPLICATE_FILE   1007
#define IDC_COMMENT_SETTINGS        1008
#define IDC_RUN                     1009

static HBRUSH g_hBackgroundBrush = NULL;
static HFONT g_hDefaultFont = NULL;


// [WindowProcedure]: 
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hWnd, WM_PAINT, OnPaint);
        HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);

    case WM_CTLCOLORSTATIC:
    {
        if ((HWND)lParam == GetDlgItem(hWnd, IDC_IGNORE_EMPTY_LINE) ||
            (HWND)lParam == GetDlgItem(hWnd, IDC_IGNORE_COMMENT_LINE) ||
            (HWND)lParam == GetDlgItem(hWnd, IDC_IGNORE_DUPLICATE_FILE))
        {
            SetBkMode((HDC)wParam, TRANSPARENT);
            return (LRESULT)g_hBackgroundBrush;
        }
    }
    break;

    case WM_ERASEBKGND:
        return 1;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
// [/WindowProcedure]


// [OnCreate]: WM_CREATE
static BOOL OnCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
    RECT rc;
    GetClientRect(hWnd, &rc);
    const int iWindowWidth = rc.right - rc.left;
    const int iWindowHeight = rc.bottom - rc.top;

    
    g_hBackgroundBrush = CreateSolidBrush(RGB(210, 210, 210));

    g_hDefaultFont = CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial"));


    CreateWindowEx(0, _T("listbox"), _T(""), WS_CHILD | WS_VISIBLE | LBS_NOINTEGRALHEIGHT,
        10, 30, iWindowWidth - 20, 200, hWnd, (HMENU)IDC_LIST, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_LIST), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);


    int iWidth = (iWindowWidth - 40) / 3;

    CreateWindowEx(0, _T("button"), _T("Добавить"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 240, iWidth, 20, hWnd, (HMENU)IDC_LIST_ADD, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_LIST_ADD), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);

    CreateWindowEx(0, _T("button"), _T("Удалить"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        20 + iWidth, 240, iWidth, 20, hWnd, (HMENU)IDC_LIST_DEL, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_LIST_DEL), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);

    CreateWindowEx(0, _T("button"), _T("Очистить"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        30 + 2 * iWidth, 240, iWidth, 20, hWnd, (HMENU)IDC_LIST_CLEAR, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_LIST_CLEAR), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);


    CreateWindowEx(0, _T("button"), _T("Игнорировать пустые строки"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        10, 300, iWindowWidth - 20, 20, hWnd, (HMENU)IDC_IGNORE_EMPTY_LINE, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_IGNORE_EMPTY_LINE), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);

    CreateWindowEx(0, _T("button"), _T("Игнорировать строки содержащие лишь комментарий"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        10, 330, iWindowWidth - 130, 20, hWnd, (HMENU)IDC_IGNORE_COMMENT_LINE, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_IGNORE_COMMENT_LINE), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);

    CreateWindowEx(0, _T("button"), _T("Игнорировать файлы с одинаковым содержимым (сравнение файлов с одинаковыми именами)"), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        10, 360, iWindowWidth - 20, 20, hWnd, (HMENU)IDC_IGNORE_DUPLICATE_FILE, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_IGNORE_DUPLICATE_FILE), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);


    CreateWindowEx(0, _T("button"), _T("Настроить"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        iWindowWidth - 110, 330, 100, 20, hWnd, (HMENU)IDC_COMMENT_SETTINGS, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_COMMENT_SETTINGS), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);


    CreateWindowEx(0, _T("button"), _T("Запуск"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        (iWindowWidth >> 1) - 75, iWindowHeight - 30, 150, 20, hWnd, (HMENU)IDC_RUN, lpcs->hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_RUN), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);

    return TRUE;
}
// [/OnCreate]


// [OnCommand]: WM_COMMAND
static void OnCommand(HWND hWnd, int id, HWND hWndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDC_LIST_ADD:
    {

    }
    break;

    case IDC_LIST_DEL:
    {

    }
    break;

    case IDC_LIST_CLEAR:
    {
        ListBox_ResetContent(GetDlgItem(hWnd, IDC_LIST));
    }
    break;

    case IDC_COMMENT_SETTINGS:
    {

    }
    break;

    case IDC_RUN:
    {

    }
    break;
    }
}
// [/OnCommand]


// [OnPaint]: WM_PAINT
static void OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hDC = BeginPaint(hWnd, &ps);
    RECT rc;
    GetClientRect(hWnd, &rc);
    const int iWindowWidth = rc.right - rc.left;
    const int iWindowHeight = rc.bottom - rc.top;
    HDC hMemDC = CreateCompatibleDC(hDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hDC, iWindowWidth, iWindowHeight);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    FillRect(hMemDC, &ps.rcPaint, g_hBackgroundBrush);

    int iOldBkMode = SetBkMode(hMemDC, TRANSPARENT);
    HFONT hOldFont = (HFONT)GetCurrentObject(hMemDC, OBJ_FONT);
    HBRUSH hOldBrush = (HBRUSH)GetCurrentObject(hMemDC, OBJ_BRUSH);
    HPEN hOldPen = (HPEN)GetCurrentObject(hMemDC, OBJ_PEN);
    COLORREF clrOldColor = GetTextColor(hMemDC);

    TCHAR szText[1024] = { 0 };


    SelectObject(hMemDC, g_hDefaultFont);
    SetTextColor(hMemDC, RGB(0, 0, 0));

    lstrcpy(szText, _T("Файлы проекта"));
    TextOut(hMemDC, 10, 10, szText, lstrlen(szText));

    MoveToEx(hMemDC, 10, 270, NULL);
    LineTo(hMemDC, iWindowWidth - 10, 270);

    lstrcpy(szText, _T("Опции"));
    TextOut(hMemDC, 10, 280, szText, lstrlen(szText));

    MoveToEx(hMemDC, 10, 390, NULL);
    LineTo(hMemDC, iWindowWidth - 10, 390);

    lstrcpy(szText, _T("Количество файлов: 0"));
    TextOut(hMemDC, 10, 400, szText, lstrlen(szText));

    lstrcpy(szText, _T("Количество строк кода: 0"));
    TextOut(hMemDC, 10, 420, szText, lstrlen(szText));

    lstrcpy(szText, _T("Количество строк кода (без фильтров): 0"));
    TextOut(hMemDC, 10, 440, szText, lstrlen(szText));
    

    SetTextColor(hMemDC, clrOldColor);
    SelectObject(hMemDC, hOldPen);
    SelectObject(hMemDC, hOldBrush);
    SelectObject(hMemDC, hOldFont);
    SetBkMode(hMemDC, iOldBkMode);

    BitBlt(hDC, 0, 0, iWindowWidth, iWindowHeight, hMemDC, 0, 0, SRCCOPY);
    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    EndPaint(hWnd, &ps);
}
// [/OnPaint]


// [OnDestroy]: WM_DESTROY
static void OnDestroy(HWND hWnd)
{
    DeleteObject(g_hBackgroundBrush);
    DeleteObject(g_hDefaultFont);

    PostQuitMessage(0);
}
// [/OnDestroy]
