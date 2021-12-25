#ifndef _NTDLL_H_
#define _NTDLL_H_

#include <windows.h>

bool NTDLL_Init();
void NTDLL_Release();

INT RtlComputeCrc32(INT, const BYTE*, UINT);

#endif
