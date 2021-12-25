#include <tchar.h>
#include <string>
#include <windows.h>
#include <windowsx.h>

#include "add_comment_dlg_proc.h"
#include "comments_settings_dlg_proc.h"
#include "resource.h"

static BOOL DlgOnInitDialog(HWND, HWND, LPARAM);                            // WM_INITDIALOG
static void DlgOnCommand(HWND, int, HWND, UINT);                            // WM_COMMAND
static void DlgOnPaint(HWND);                                               // WM_PAINT

#define IDC_LANG_NAME                   1001
#define IDC_FILE_EXTENSION              1002
#define IDC_BEGIN_COMMENT               1003
#define IDC_END_COMMENT                 1004
#define IDC_END_COMMENT_AT_END_LINE     1005

static HBRUSH g_hBackgroundBrush = NULL;
static HFONT g_hDefaultFont = NULL;


// [AddComment_DialogProcedure]:
BOOL CALLBACK AddComment_DialogProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG, DlgOnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, DlgOnCommand);
        HANDLE_MSG(hWnd, WM_PAINT, DlgOnPaint);

    case WM_CTLCOLORSTATIC:
    {
        if ((HWND)lParam == GetDlgItem(hWnd, IDC_END_COMMENT_AT_END_LINE))
        {
            SetBkMode((HDC)wParam, TRANSPARENT);
            return (BOOL)g_hBackgroundBrush;
        }
    }
    break;

    case WM_ERASEBKGND:
        return TRUE;

    default:
        return FALSE;
    }

    return TRUE;
}
// [/AddComment_DialogProcedure]


// [DlgOnInitDialog]: WM_INITDIALOG
static BOOL DlgOnInitDialog(HWND hWnd, HWND, LPARAM)
{
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(GetParent(hWnd), GWL_HINSTANCE);

    SetClassLongPtr(hWnd, GCL_HICON, (LONG)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)));
    SetClassLongPtr(hWnd, GCL_HICONSM, (LONG)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1)));

    RECT rc;

    GetWindowRect(GetParent(hWnd), &rc);
    const int iParentWindowX = rc.left;
    const int iParentWindowY = rc.top;
    const int iParentWindowWidth = rc.right - rc.left;
    const int iParentWindowHeight = rc.bottom - rc.top;

    SetRect(&rc, 0, 0, 300, 300);
    AdjustWindowRectEx(
        &rc,
        GetWindowLongPtr(hWnd, GWL_STYLE),
        FALSE,
        GetWindowLongPtr(hWnd, GWL_EXSTYLE)
    );

    MoveWindow(
        hWnd,
        iParentWindowX + (iParentWindowWidth >> 1) - ((rc.right - rc.left) >> 1),
        iParentWindowY + (iParentWindowHeight >> 1) - ((rc.bottom - rc.top) >> 1),
        rc.right - rc.left,
        rc.bottom - rc.top,
        TRUE
    );

    GetClientRect(hWnd, &rc);
    const int iWindowWidth = rc.right - rc.left;
    const int iWindowHeight = rc.bottom - rc.top;

    g_hBackgroundBrush = CreateSolidBrush(RGB(210, 220, 220));

    g_hDefaultFont = CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("Arial"));


    CreateWindowEx(0, _T("edit"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        10, 30, iWindowWidth - 20, 20, hWnd, (HMENU)IDC_LANG_NAME, hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_LANG_NAME), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);
    Edit_LimitText(GetDlgItem(hWnd, IDC_LANG_NAME), 120);

    CreateWindowEx(0, _T("edit"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        10, 90, iWindowWidth - 20, 20, hWnd, (HMENU)IDC_FILE_EXTENSION, hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_FILE_EXTENSION), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);
    Edit_LimitText(GetDlgItem(hWnd, IDC_FILE_EXTENSION), 120);

    CreateWindowEx(0, _T("edit"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        10, 150, iWindowWidth - 20, 20, hWnd, (HMENU)IDC_BEGIN_COMMENT, hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_BEGIN_COMMENT), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);
    Edit_LimitText(GetDlgItem(hWnd, IDC_BEGIN_COMMENT), 120);

    CreateWindowEx(0, _T("edit"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL,
        10, 210, iWindowWidth - 20, 20, hWnd, (HMENU)IDC_END_COMMENT, hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_END_COMMENT), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);
    Edit_LimitText(GetDlgItem(hWnd, IDC_END_COMMENT), 120);

    CreateWindowEx(0, _T("button"), _T("Конец комментария в конце строки"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
        10, 240, iWindowWidth - 20, 20, hWnd, (HMENU)IDC_END_COMMENT_AT_END_LINE, hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_END_COMMENT_AT_END_LINE), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);

    
    CreateWindowEx(0, _T("button"), _T("ОК"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        iWindowWidth - 220, iWindowHeight - 30, 100, 20, hWnd, (HMENU)IDOK, hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDOK), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);

    CreateWindowEx(0, _T("button"), _T("Отмена"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        iWindowWidth - 110, iWindowHeight - 30, 100, 20, hWnd, (HMENU)IDCANCEL, hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDCANCEL), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);

    return TRUE;
}
// [/DlgOnInitDialog]


// [DlgOnCommand]: WM_COMMAND
static void DlgOnCommand(HWND hWnd, int id, HWND hCtrlWnd, UINT)
{
    switch (id)
    {
    case IDC_END_COMMENT_AT_END_LINE:
    {
        if (Button_GetCheck(hCtrlWnd) == BST_CHECKED)
        {
            SetWindowText(GetDlgItem(hWnd, IDC_END_COMMENT), _T("\0"));
            EnableWindow(GetDlgItem(hWnd, IDC_END_COMMENT), FALSE);
        }
        else
        {
            EnableWindow(GetDlgItem(hWnd, IDC_END_COMMENT), TRUE);
        }
    }
    break;

    case IDOK:
    {
        TCHAR szLangName[128];
        memset(szLangName, 0, sizeof(szLangName));
        GetWindowText(GetDlgItem(hWnd, IDC_LANG_NAME), szLangName, 128);

        TCHAR szFileExtension[128];
        memset(szFileExtension, 0, sizeof(szFileExtension));
        GetWindowText(GetDlgItem(hWnd, IDC_FILE_EXTENSION), szFileExtension, 128);

        TCHAR szBeginComment[128];
        memset(szBeginComment, 0, sizeof(szBeginComment));
        GetWindowText(GetDlgItem(hWnd, IDC_BEGIN_COMMENT), szBeginComment, 128);

        TCHAR szEndComment[128];
        memset(szEndComment, 0, sizeof(szEndComment));
        GetWindowText(GetDlgItem(hWnd, IDC_END_COMMENT), szEndComment, 128);

        bool bEndCommentAtEndLine = Button_GetCheck(GetDlgItem(hWnd, IDC_END_COMMENT_AT_END_LINE)) == BST_CHECKED;

        if (!szLangName[0] || !szFileExtension[0] || !szBeginComment[0] || (!szEndComment[0] && !bEndCommentAtEndLine))
        {
            MessageBox(hWnd, _T("Нужно заполнить все поля!"), _T("SCLC"), MB_OK | MB_ICONINFORMATION);
            break;
        }
        else
        {
            Comments_Add(szLangName, szFileExtension, szBeginComment, szEndComment, bEndCommentAtEndLine);
        }
    }

    case IDCANCEL:
    {
        DeleteObject(g_hBackgroundBrush);
        DeleteObject(g_hDefaultFont);
        EndDialog(hWnd, 0);
    }
    break;
    }
}
// [/DlgOnCommand]


// [DlgOnPaint]: WM_PAINT
static void DlgOnPaint(HWND hWnd)
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
    
    TCHAR szText[256] = { 0 };

    SelectObject(hMemDC, g_hDefaultFont);

    lstrcpy(szText, _T("Язык программирования"));
    TextOut(hMemDC, 10, 10, szText, lstrlen(szText));

    lstrcpy(szText, _T("Расширение файла (например cpp)"));
    TextOut(hMemDC, 10, 70, szText, lstrlen(szText));

    lstrcpy(szText, _T("Начало комментария"));
    TextOut(hMemDC, 10, 130, szText, lstrlen(szText));

    lstrcpy(szText, _T("Конец комментария"));
    TextOut(hMemDC, 10, 190, szText, lstrlen(szText));

    SelectObject(hMemDC, hOldFont);
    SetBkMode(hMemDC, iOldBkMode);

    BitBlt(hDC, 0, 0, iWindowWidth, iWindowHeight, hMemDC, 0, 0, SRCCOPY);
    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    EndPaint(hWnd, &ps);
}
// [/DlgOnPaint]
