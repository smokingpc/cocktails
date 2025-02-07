#pragma once

#define SVCNAME_A   "svctest"
#define SVCNAME     _T(SVCNAME_A)
#define SVCTYPE     SERVICE_WIN32_OWN_PROCESS
#define EVTMSG(x)   (TCHAR*)_T(x)

#define DEFAULT_SVC_WAIT    5000        //in milliseconds
#define SMALL_BUFFER_SIZE   256
#define BIG_BUFFER_SIZE   1024

#define EXIT_INIT_FAILED  -3
#define EXIT_SHUTDOWN_FAILED  -4
#define EXIT_SETUPJOB_FAILED  -5

#define MS_TO_SEC       (1000)
#define CHECK_INTERVAL  (5*MS_TO_SEC)   //check target every 5 seconds.

void WINAPI SvcCtrlHandler(DWORD dwCtrl);
void WINAPI ServiceMain(DWORD argc, LPTSTR argv[]);
BOOL WINAPI InitService();
void WINAPI ShutdownService();
void UpdateSvcStatus(
    DWORD current_state, 
    DWORD win32_exit, 
    DWORD svc_exit, 
    DWORD wait = DEFAULT_SVC_WAIT);
