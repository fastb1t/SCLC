#include <tchar.h>
#include <string>
#include <vector>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "comments_settings_dlg_proc.h"
#include "add_comment_dlg_proc.h"
#include "resource.h"
#include "algorithms.h"

static BOOL DlgOnInitDialog(HWND, HWND, LPARAM);                            // WM_INITDIALOG
static void DlgOnCommand(HWND, int, HWND, UINT);                            // WM_COMMAND
static void DlgOnPaint(HWND);                                               // WM_PAINT

#define IDC_LIST                    1001
#define IDC_LIST_ADD                1002
#define IDC_LIST_DEL                1003
#define IDC_LIST_CLEAR              1004

static HINSTANCE g_hInstance = NULL;
static HWND g_hWnd = NULL;

static HBRUSH g_hBackgroundBrush = NULL;
static HFONT g_hDefaultFont = NULL;

static String g_sSettingsFileName;

static std::vector<CommentInfo*> g_comments;
static std::vector<CommentInfo*> g_tmp_comments;


// [Comments_SettingsFile]:
void Comments_SettingsFile(const String& sFileName)
{
    g_sSettingsFileName = sFileName;
}
// [/Comments_SettingsFile]


// [Comments_Init]:
void Comments_Init()
{
    String sSection = _T("General");
    
    UINT nComments = GetPrivateProfileInt(sSection.c_str(), _T("Comments"), 0, g_sSettingsFileName.c_str());
    if (nComments == 0)
    {
        return;
    }

    CommentInfo* ci;
    String sKey;

    for (UINT i = 0; i < nComments; i++)
    {
        ci = new (std::nothrow) CommentInfo;
        if (ci == nullptr)
        {
            continue;
        }

        memset(ci, 0, sizeof(CommentInfo));

        sKey = _T("Comment");
        sKey += ToString(i + 1);

        if (GetPrivateProfileStruct(sSection.c_str(), sKey.c_str(), reinterpret_cast<LPVOID>(ci), sizeof(CommentInfo), g_sSettingsFileName.c_str()))
        {
            g_comments.push_back(ci);
        }
        else
        {
            delete ci;
        }
    }
}
// [/Comments_Init]


// [ClearMainData]:
static void ClearMainData()
{
    if (g_comments.size() > 0)
    {
        for (std::vector<CommentInfo*>::const_iterator it = g_comments.begin(); it != g_comments.end(); it++)
        {
            delete (*it);
        }

        g_comments.clear();
    }
}
// [/ClearMainData]


// [ClearTmpData]:
static void ClearTmpData()
{
    if (g_tmp_comments.size() > 0)
    {
        for (std::vector<CommentInfo*>::const_iterator it = g_tmp_comments.begin(); it != g_tmp_comments.end(); it++)
        {
            delete (*it);
        }

        g_tmp_comments.clear();
    }
}
// [/ClearTmpData]


// [Comments_Clear]:
void Comments_Clear()
{
    ClearMainData();
    ClearTmpData();
}
// [/Comments_Clear]


// [TmpToMain]:
static void TmpToMain()
{
    CommentInfo* ci;

    for (std::vector<CommentInfo*>::const_iterator it = g_tmp_comments.begin(); it != g_tmp_comments.end(); it++)
    {
        ci = new (std::nothrow) CommentInfo;
        if (ci == nullptr)
        {
            continue;
        }

        memcpy(ci, (*it), sizeof(CommentInfo));

        g_comments.push_back(ci);
    }
}
// [/TmpToMain]


// [MainToTmp]:
static void MainToTmp()
{
    CommentInfo* ci;

    for (std::vector<CommentInfo*>::const_iterator it = g_comments.begin(); it != g_comments.end(); it++)
    {
        ci = new (std::nothrow) CommentInfo;
        if (ci == nullptr)
        {
            continue;
        }

        memcpy(ci, (*it), sizeof(CommentInfo));

        g_tmp_comments.push_back(ci);
    }
}
// [/MainToTmp]


// [Comments_Add]:
void Comments_Add(
    const TCHAR* szLangName,
    const TCHAR* szFileExtension,
    const TCHAR* szBeginComment,
    const TCHAR* szEndComment,
    bool bEndCommentAtEndLine
)
{
    HWND hListViewWnd = GetDlgItem(g_hWnd, IDC_LIST);
    if (!hListViewWnd)
    {
        return;
    }

    CommentInfo* ci = new (std::nothrow) CommentInfo;
    if (ci == nullptr)
    {
        return;
    }

    memset(ci, 0, sizeof(CommentInfo));

    lstrcpy(ci->szLangName, szLangName);
    lstrcpy(ci->szFileExtension, szFileExtension);
    lstrcpy(ci->szBeginComment, szBeginComment);
    lstrcpy(ci->szEndComment, szEndComment);
    ci->bEndCommentAtEndLine = bEndCommentAtEndLine;

    g_tmp_comments.push_back(ci);

    LV_ITEM lvi;
    memset(&lvi, 0, sizeof(LV_ITEM));

    lvi.mask = LVIF_TEXT;
    lvi.iItem = ListView_GetItemCount(hListViewWnd);

    ListView_InsertItem(hListViewWnd, &lvi);
    ListView_SetItemText(hListViewWnd, lvi.iItem, 1, (LPSTR)szLangName);
    ListView_SetItemText(hListViewWnd, lvi.iItem, 2, (LPSTR)szFileExtension);

    String comment;
    comment = szBeginComment;
    comment += _T(" ... ");
    comment += !bEndCommentAtEndLine ? szEndComment : _T("до конца строки");

    ListView_SetItemText(hListViewWnd, lvi.iItem, 3, (LPSTR)comment.c_str());
}
// [/Comments_Add]


// [Comments_GetAllComments]:
const std::vector<CommentInfo*> Comments_GetAllComments()
{
    return g_comments;
}
// [/Comments_GetAllComments]


// [SynchronizeComments]:
static void SynchronizeComments()
{
    HWND hListViewWnd = GetDlgItem(g_hWnd, IDC_LIST);
    if (!hListViewWnd)
    {
        return;
    }

    LV_ITEM lvi;
    memset(&lvi, 0, sizeof(LV_ITEM));

    lvi.mask = LVIF_TEXT;
    lvi.iItem = -1;

    String comment;

    for (std::vector<CommentInfo*>::const_iterator it = g_comments.begin(); it != g_comments.end(); it++)
    {
        lvi.iItem++;
        ListView_InsertItem(hListViewWnd, &lvi);
        ListView_SetItemText(hListViewWnd, lvi.iItem, 1, (LPSTR)(*it)->szLangName);
        ListView_SetItemText(hListViewWnd, lvi.iItem, 2, (LPSTR)(*it)->szFileExtension);

        comment = (*it)->szBeginComment;
        comment += _T(" ... ");
        comment += !(*it)->bEndCommentAtEndLine ? (*it)->szEndComment : _T("до конца строки");

        ListView_SetItemText(hListViewWnd, lvi.iItem, 3, (LPSTR)comment.c_str());
    }
    
    ClearTmpData();
    MainToTmp();
}
// [/SynchronizeComments]


// [IsChanged]:
static bool IsChanged()
{
    bool bIsChanged = false;

    if (g_comments.size() != g_tmp_comments.size())
    {
        bIsChanged = true;
    }
    else
    {
        for (std::vector<CommentInfo*>::const_iterator
            it1 = g_comments.begin(), it2 = g_tmp_comments.begin();
            it1 != g_comments.end() && it2 != g_tmp_comments.end();
            it1++, it2++)
        {
            if (lstrcmp((*it1)->szLangName, (*it2)->szLangName))
            {
                bIsChanged = true;
                break;
            }

            if (lstrcmp((*it1)->szFileExtension, (*it2)->szFileExtension))
            {
                bIsChanged = true;
                break;
            }

            if (lstrcmp((*it1)->szBeginComment, (*it2)->szBeginComment))
            {
                bIsChanged = true;
                break;
            }

            if (lstrcmp((*it1)->szEndComment, (*it2)->szEndComment))
            {
                bIsChanged = true;
                break;
            }

            if ((*it1)->bEndCommentAtEndLine != (*it2)->bEndCommentAtEndLine)
            {
                bIsChanged = true;
                break;
            }
        }
    }

    return bIsChanged;
}
// [/IsChanged]


// [CommentsSettings_DialogProcedure]:
BOOL CALLBACK CommentsSettings_DialogProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(hWnd, WM_INITDIALOG, DlgOnInitDialog);
        HANDLE_MSG(hWnd, WM_COMMAND, DlgOnCommand);
        HANDLE_MSG(hWnd, WM_PAINT, DlgOnPaint);

    case WM_NOTIFY:
    {
        LPNMHDR lpNMHDR = reinterpret_cast<LPNMHDR>(lParam);
        if (lpNMHDR)
        {
            if (lpNMHDR->code == NM_CUSTOMDRAW && lpNMHDR->idFrom == IDC_LIST)
            {
                LPNMLVCUSTOMDRAW lpNMLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(lParam);
                if (lpNMLVCD)
                {
                    switch (lpNMLVCD->nmcd.dwDrawStage)
                    {
                    case CDDS_PREPAINT:
                    {
                        SetWindowLongPtr(hWnd, DWLP_MSGRESULT, (LONG)CDRF_NOTIFYITEMDRAW);
                    }
                    break;

                    case CDDS_ITEMPREPAINT:
                    {
                        lpNMLVCD->nmcd.uItemState &= ~CDIS_SELECTED;
                        if (ListView_GetItemState(lpNMLVCD->nmcd.hdr.hwndFrom, lpNMLVCD->nmcd.dwItemSpec, LVIS_SELECTED))
                        {
                            lpNMLVCD->clrTextBk = RGB(220, 220, 210);
                        }
                        SetWindowLongPtr(hWnd, DWLP_MSGRESULT, (LONG)CDRF_NOTIFYSUBITEMDRAW);
                    }
                    break;

                    case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
                    {
                        if (lpNMLVCD->iSubItem == 0)
                        {
                            TCHAR szText[12] = { 0 };
                            wsprintf(szText, _T("%d"), lpNMLVCD->nmcd.dwItemSpec + 1);

                            SIZE size;
                            GetTextExtentPoint32(lpNMLVCD->nmcd.hdc, szText, lstrlen(szText), &size);
                            TextOut(
                                lpNMLVCD->nmcd.hdc,
                                lpNMLVCD->nmcd.rc.left,
                                lpNMLVCD->nmcd.rc.top + ((lpNMLVCD->nmcd.rc.bottom - lpNMLVCD->nmcd.rc.top) >> 1) - (size.cy >> 1),
                                szText,
                                lstrlen(szText)
                            );

                            SetWindowLongPtr(hWnd, DWLP_MSGRESULT, (LONG)CDRF_SKIPDEFAULT);
                        }
                        else
                        {
                            SetWindowLongPtr(hWnd, DWLP_MSGRESULT, (LONG)CDRF_DODEFAULT);
                        }
                    }
                    break;

                    default:
                        SetWindowLongPtr(hWnd, DWLP_MSGRESULT, (LONG)CDRF_DODEFAULT);
                    }
                }
            }
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
// [/CommentsSettings_DialogProcedure]


// [DlgOnInitDialog]:
static BOOL DlgOnInitDialog(HWND hWnd, HWND, LPARAM)
{
    g_hWnd = hWnd;

    g_hInstance = (HINSTANCE)GetWindowLongPtr(GetParent(hWnd), GWLP_HINSTANCE);

    SetClassLongPtr(hWnd, GCLP_HICON, (LONG)LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON1)));
    SetClassLongPtr(hWnd, GCLP_HICONSM, (LONG)LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON1)));

    RECT rc;

    GetWindowRect(GetParent(hWnd), &rc);
    const int iParentWindowX = rc.left;
    const int iParentWindowY = rc.top;
    const int iParentWindowWidth = rc.right - rc.left;
    const int iParentWindowHeight = rc.bottom - rc.top;

    SetRect(&rc, 0, 0, 550, 300);
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


    HWND hListViewWnd = CreateWindowEx(0, WC_LISTVIEW, _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL | LVS_REPORT,
        10, 40, iWindowWidth - 20, 220, hWnd, (HMENU)IDC_LIST, g_hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDC_LIST), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);

    ListView_SetExtendedListViewStyle(hListViewWnd, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    LVCOLUMN lvc;
    memset(&lvc, 0, sizeof(LVCOLUMN));

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;
    
    lvc.cx = 30;
    lvc.pszText = (LPSTR)_T("№");
    ListView_InsertColumn(hListViewWnd, 0, &lvc);

    lvc.cx = 160;
    lvc.pszText = (LPSTR)_T("Язык программирования");
    ListView_InsertColumn(hListViewWnd, 1, &lvc);

    lvc.cx = 150;
    lvc.pszText = (LPSTR)_T("Расширение файла");
    ListView_InsertColumn(hListViewWnd, 2, &lvc);

    lvc.cx = 170;
    lvc.pszText = (LPSTR)_T("Комментарий");
    ListView_InsertColumn(hListViewWnd, 3, &lvc);


    CreateWindowEx(0, _T("button"), _T(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_BITMAP,
        iWindowWidth - 102, 8, 24, 24, hWnd, (HMENU)IDC_LIST_ADD, g_hInstance, NULL);
    CreateToolTip(g_hInstance, GetDlgItem(hWnd, IDC_LIST_ADD), _T("Добавить"));
    SendMessage(GetDlgItem(hWnd, IDC_LIST_ADD), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_LIST_ADD)));


    CreateWindowEx(0, _T("button"), _T(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_BITMAP,
        iWindowWidth - 68, 8, 24, 24, hWnd, (HMENU)IDC_LIST_DEL, g_hInstance, NULL);
    CreateToolTip(g_hInstance, GetDlgItem(hWnd, IDC_LIST_DEL), _T("Удалить"));
    SendMessage(GetDlgItem(hWnd, IDC_LIST_DEL), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_LIST_DEL)));


    CreateWindowEx(0, _T("button"), _T(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_BITMAP,
        iWindowWidth - 34, 8, 24, 24, hWnd, (HMENU)IDC_LIST_CLEAR, g_hInstance, NULL);
    CreateToolTip(g_hInstance, GetDlgItem(hWnd, IDC_LIST_CLEAR), _T("Очистить"));
    SendMessage(GetDlgItem(hWnd, IDC_LIST_CLEAR), BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_LIST_CLEAR)));


    CreateWindowEx(0, _T("button"), _T("ОК"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        iWindowWidth - 220, iWindowHeight - 30, 100, 20, hWnd, (HMENU)IDOK, g_hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDOK), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);

    CreateWindowEx(0, _T("button"), _T("Отмена"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        iWindowWidth - 110, iWindowHeight - 30, 100, 20, hWnd, (HMENU)IDCANCEL, g_hInstance, NULL);
    SendMessage(GetDlgItem(hWnd, IDCANCEL), WM_SETFONT, (WPARAM)g_hDefaultFont, 0L);


    SynchronizeComments();

    return TRUE;
}
// [/DlgOnInitDialog]


// [DlgOnCommand]:
static void DlgOnCommand(HWND hWnd, int id, HWND, UINT)
{
    switch (id)
    {
    case IDC_LIST_ADD:
    {
        DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ADD_COMMENT), hWnd, (DLGPROC)AddComment_DialogProcedure);

        InvalidateRect(hWnd, NULL, FALSE);
    }
    break;

    case IDC_LIST_DEL:
    {
        HWND hListViewWnd = GetDlgItem(hWnd, IDC_LIST);
        if (hListViewWnd)
        {
            int i = -1;
            while ((i = ListView_GetNextItem(hListViewWnd, -1, LVNI_SELECTED)) != -1)
            {
                ListView_DeleteItem(hListViewWnd, i);

                delete (*(g_tmp_comments.begin() + i));

                g_tmp_comments.erase(g_tmp_comments.begin() + i);
            }
        }

        InvalidateRect(hWnd, NULL, FALSE);
    }
    break;

    case IDC_LIST_CLEAR:
    {
        ListView_DeleteAllItems(GetDlgItem(hWnd, IDC_LIST));

        ClearTmpData();

        InvalidateRect(hWnd, NULL, FALSE);
    }
    break;

    case IDOK:
    {
        ClearMainData();
        TmpToMain();

        String sSection = _T("General");
        
        WritePrivateProfileString(sSection.c_str(), NULL, NULL, g_sSettingsFileName.c_str());

        WritePrivateProfileString(sSection.c_str(), _T("Comments"), ToString(g_comments.size()).c_str(), g_sSettingsFileName.c_str());

        String sKey;

        int iCommentCounter = 0;
        for (std::vector<CommentInfo*>::const_iterator it = g_comments.begin(); it != g_comments.end(); it++)
        {
            iCommentCounter++;
            
            sKey = _T("Comment");
            sKey += ToString(iCommentCounter);

            WritePrivateProfileStruct(sSection.c_str(), sKey.c_str(), reinterpret_cast<LPVOID>(*it), sizeof(CommentInfo), g_sSettingsFileName.c_str());
        }
    }

    case IDCANCEL:
    {
        if (IsChanged())
        {
            switch (MessageBox(hWnd, _T("Сохранить изменения?"), _T(""), MB_YESNOCANCEL | MB_ICONQUESTION))
            {
            case IDYES:
            {
                SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDOK, BN_CLICKED), (LPARAM)GetDlgItem(hWnd, IDOK));
            }
            break;

            case IDCANCEL:
            {
                return;
            }
            break;
            }
        }

        ClearTmpData();

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

    TCHAR szText[1024] = { 0 };

    SelectObject(hMemDC, g_hDefaultFont);

    wsprintf(szText, _T("Количество записей: %d"), ListView_GetItemCount(GetDlgItem(hWnd, IDC_LIST)));
    TextOut(hMemDC, 10, 10, szText, lstrlen(szText));

    SelectObject(hMemDC, hOldFont);
    SetBkMode(hMemDC, iOldBkMode);

    BitBlt(hDC, 0, 0, iWindowWidth, iWindowHeight, hMemDC, 0, 0, SRCCOPY);
    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    EndPaint(hWnd, &ps);
}
// [/DlgOnPaint]
