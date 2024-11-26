#pragma once

#define SVCNAME_A   "watchsvc"
#define SVCNAME     _T(SVCNAME_A)
#define SVC_REGKEY_A    "SYSTEM\\CurrentControlSet\\Services\\" SVCNAME_A "\\Parameters"
#define SVC_REGKEY      _T(SVC_REGKEY_A)

#define REG_TARGET_NAME _T("MonitorSvcTarget")

#define SVCTYPE     SERVICE_WIN32_OWN_PROCESS
#define EVTMSG(x)   (TCHAR*)_T(x)

#define DEFAULT_SVC_WAIT    5000        //in milliseconds
#define SMALL_BUFFER_SIZE   256

#define EXIT_INIT_FAILED  -3
#define EXIT_SHUTDOWN_FAILED  -4
#define EXIT_SETUPJOB_FAILED  -5

#define MS_TO_SEC       (1000)
#define CHECK_INTERVAL  (5*MS_TO_SEC)   //check target every 5 seconds.

#define SVC_ERROR   ((DWORD)0xC0020001L)

#define ERR_MSG_RED     _T("\x1b[91m %s \x1b[0m\n")
