#pragma once

VOID WINAPI SvcCtrlHandler(DWORD ctrl_code, DWORD type, LPVOID data, LPVOID ctx);
VOID ReportSvcStatus(DWORD state, DWORD exit_code, DWORD wait_hint);
void ServiceMain(DWORD argc, LPTSTR *argv);
VOID ReportSvcEvent(WORD event_type, DWORD event_id, LPTSTR msg);

__inline VOID ReportSvcErrorEvent(LPTSTR msg)
{
    ReportSvcEvent(EVENTLOG_ERROR_TYPE, SVC_EVENT_ERROR, msg);
}

__inline VOID ReportSvcInfoEvent(LPTSTR msg)
{
    ReportSvcEvent(EVENTLOG_INFORMATION_TYPE, SVC_EVENT_INFO, msg);
}
