#pragma once

void WINAPI SvcCtrlHandler(DWORD dwCtrl);
void WINAPI ServiceMain(DWORD argc, LPTSTR argv[]);
BOOL WINAPI InitService();
void WINAPI ShutdownService();
void ReportSvcStatus(DWORD current_state, DWORD win32_exit, DWORD svc_exit, DWORD wait = DEFAULT_SVC_WAIT);
void ReportEventLog(DWORD event_id, LPTSTR msg, DWORD last_error = 0);
BOOL SetupEventReporter();
void TeardownEventReporter();

