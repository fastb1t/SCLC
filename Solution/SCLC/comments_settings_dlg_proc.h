#ifndef _COMMENTS_SETTINGS_DLG_PROC_H_
#define _COMMENTS_SETTINGS_DLG_PROC_H_

#include <tchar.h>
#include <windows.h>

#include "algorithms.h"

BOOL CALLBACK CommentsSettings_DialogProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Comments_SettingsFile(const String& sFileName);
void Comments_Init();
void Comments_Clear();

void Comments_Add(
    const TCHAR* szLangName,
    const TCHAR* szFileExtension,
    const TCHAR* szBeginComment,
    const TCHAR* szEndComment,
    bool bEndCommentAtEndLine
);

#endif
