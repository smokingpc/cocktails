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
DWORD WINAPI SvcCtrlHandlerEx(
    DWORD ctrl_code,
    DWORD event_type,
    LPVOID event_data,
    LPVOID context);

VOID WINAPI ServiceMain(DWORD argc, LPTSTR argv[]);
BOOL WINAPI InitService();
VOID WINAPI ShutdownService();
VOID ReportCurrentSvcStatus();
VOID ReportSvcStatus(
    DWORD current_state, 
    DWORD win32_exit, 
    DWORD svc_exit, 
    DWORD wait = DEFAULT_SVC_WAIT);
VOID ReportEventLog(DWORD event_id, LPTSTR msg, DWORD last_error = 0);
BOOL SetupEventReporter();
VOID TeardownEventReporter();

