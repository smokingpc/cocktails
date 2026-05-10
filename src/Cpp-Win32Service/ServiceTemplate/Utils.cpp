#include "pch.h"

BOOL GetCurrentModulePath(TCHAR* buffer, DWORD buflen_in_chars) {
    DWORD ret = GetModuleFileNameW(NULL, buffer, buflen_in_chars);
    DWORD err = GetLastError();

    if ((0 == ret) || (ERROR_INSUFFICIENT_BUFFER != err)) {
        return FALSE;       //string in buffer is truncated if err==ERROR_INSUFFICIENT_BUFFER.
    }
    return TRUE;
}
