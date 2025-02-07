// Win32ServiceStatusTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "CommonInclude.h"
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "Shlwapi.lib")

int _tmain(int argc, TCHAR* argv[]) 
{
    // ServiceMain is entry function of service. 
    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { (LPTSTR)SVCNAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };

    // This call returns when the service has stopped. 
    // The process should simply terminate when the call returns.
    return (TRUE == StartServiceCtrlDispatcher(DispatchTable))? 0 : 1;
}
