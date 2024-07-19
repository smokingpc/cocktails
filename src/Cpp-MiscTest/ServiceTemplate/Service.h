#pragma once

#define SVCNAME     (LPWSTR)TEXT("MyService")
#define SVCTYPE     SERVICE_WIN32_OWN_PROCESS
#define EVTMSG(x)   (TCHAR*)_T(x)

#define DEFAULT_SVC_WAIT    5000        //in milliseconds
#define SMALL_BUFFER_SIZE   256

#define EXIT_INIT_FAILED  -3
#define EXIT_SHUTDOWN_FAILED  -4
#define EXIT_SETUPJOB_FAILED  -5

VOID SvcInstall();
VOID WINAPI SvcCtrlHandler(DWORD dwCtrl);
VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv[]);
BOOL WINAPI InitService();
void WINAPI ShutdownService();
VOID ReportSvcStatus(DWORD current_state, DWORD win32_exit, DWORD svc_exit, DWORD wait = DEFAULT_SVC_WAIT);
VOID ReportEventLog(DWORD event_id, LPTSTR msg, DWORD last_error = 0);
BOOL SetupEventReporter();
void TeardownEventReporter();

