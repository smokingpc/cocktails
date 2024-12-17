#include "pch.h"
#pragma comment(lib, "advapi32.lib")


//   Entry point of EXE
int __cdecl _tmain(int argc, TCHAR* argv[])
{
    SetupEventReporter();

    // ServiceMain is entry function of service. 
    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { SVCNAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };

    // This call returns when the service has stopped. 
    // The process should simply terminate when the call returns.
    if (!StartServiceCtrlDispatcher(DispatchTable))
        ReportEventLog(SVC_ERROR, EVTMSG("Start service failed"), GetLastError());

    TeardownEventReporter();
}
