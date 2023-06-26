#pragma once

VOID WINAPI SvcCtrlHandler(DWORD ctrl_code, DWORD type, LPVOID data, LPVOID ctx);
VOID ReportSvcStatus(DWORD state, DWORD exit_code, DWORD wait_hint);
VOID ReportSvcErrorEvent(LPTSTR msg);
VOID ReportSvcInfoEvent(LPTSTR msg);
void ServiceMain(DWORD argc, LPTSTR *argv);
