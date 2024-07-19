#include "CommonHeader.h"

void CheckAndSetShutdownParams()
{
    DWORD level = 0;
    DWORD flag = 0;
    TCHAR msg[MAX_PATH] = {0};
    GetProcessShutdownParameters(&level, &flag);
    _stprintf_s(msg, _countof(msg), _T("[SpcIoTest] GetProcessShutdownParameters()=> level=0x%08X, flag=0x%08X\n"), level, flag);
    ReportSvcInfoEvent(msg);

    level = 0xFF;
    flag = 0;
    SetProcessShutdownParameters(level, flag);

    level = 0;
    flag = 0;
    GetProcessShutdownParameters(&level, &flag);
    RtlZeroMemory(msg, sizeof(msg));
    _stprintf_s(msg, _countof(msg), _T("[SpcIoTest] 2nd GetProcessShutdownParameters()=> level=0x%08X, flag=0x%08X\n"), level, flag);
    ReportSvcInfoEvent(msg);
}
