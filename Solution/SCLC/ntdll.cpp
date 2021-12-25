#include "ntdll.h"

#include <tchar.h>

typedef INT(WINAPI* pfnRtlComputeCrc32)(INT, const BYTE*, UINT);

static HMODULE hDLL = NULL;
static pfnRtlComputeCrc32 fnRtlComputeCrc32 = NULL;

bool NTDLL_Init()
{
    if (fnRtlComputeCrc32 != NULL)
    {
        return true;
    }

    if (hDLL != NULL)
    {
        FreeLibrary(hDLL);
    }

    hDLL = GetModuleHandle(_T("ntdll.dll"));
    if (hDLL == NULL)
    {
        return false;
    }

    fnRtlComputeCrc32 = (pfnRtlComputeCrc32)GetProcAddress(hDLL, "RtlComputeCrc32");
    if (fnRtlComputeCrc32 == NULL)
    {
        FreeLibrary(hDLL);
        return false;
    }

    return true;
}

void NTDLL_Release()
{
    fnRtlComputeCrc32 = NULL;

    if (hDLL != NULL)
    {
        FreeLibrary(hDLL);
        hDLL = NULL;
    }
}

INT RtlComputeCrc32(INT iInitialCRC32, const BYTE* pData, UINT iLength)
{
    return fnRtlComputeCrc32 != NULL ? fnRtlComputeCrc32(iInitialCRC32, pData, iLength) : 0;
}
