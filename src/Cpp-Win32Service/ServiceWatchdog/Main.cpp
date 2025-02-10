// =========================================================================
// ProcessWatchdog is a SPECIAL SERVICE which monitor specified process.
// If monitored process gone, watchdog will relaunch it in 1~3 second.
// ProcessWatchdog can monitor process of app/service.
// =========================================================================

#include "CommonInclude.h"
#pragma comment(lib, "advapi32.lib")

TCHAR SvcMonitorTarget[MAX_PATH] = {0};

void Usage()
{
    printf("ServiceWatchdog is a util to monitor specified service,\nand restart it if it's gone.\n");
    printf("Usage: ServiceWatchdog.exe <cmd> [<target svc name>]\n");
    printf("cmd=>\n");
    printf("\tinstall\n");
    printf("\tuninstall\n");
    printf("target svc name=>(only valid for install cmd)\n");
    printf("\tservice name(not exe filename) which you want monitor.\n");
    printf("Example:\n");
    printf("[install]\n");
    printf("\tAssume you want to monitor \"mssql\" service, run\n");
    printf("\t  ServiceWatchdog.exe install mssql\n");
    printf("[uninstall]\n");
    printf("\tTo uninstall ServiceWatchdog, run\n");
    printf("\t  ServiceWatchdog.exe uninstall\n");
    printf("\n");
}

bool ParseCliOpions(PWATCHDOG_OPTIONS options, int argc, TCHAR* argv[])
{
    if (argc < 2)
    {
        _tprintf(ERR_MSG_RED, _T("Arguments not enough!"));
        return false;
    }

    options->Action = DO_ACTION::UNKNOWN;

    switch(argc)
    {
    case 2:
    {
        if (lstrcmpi(argv[1], TEXT("uninstall")) == 0)
        {
            options->Action = DO_ACTION::UNINSTALL;
            return true;
        }
        return false;
    }
        break;
    case 3:
    {
        if (lstrcmpi(argv[1], TEXT("install")) == 0)
        {
            options->Action = DO_ACTION::INSTALL;
            _tcscpy_s(options->SvcName, _countof(options->SvcName), argv[2]);
            return true;
        }
        else if (lstrcmpi(argv[1], TEXT("console")) == 0)
        {
            options->Action = DO_ACTION::CLI_MODE;
            _tcscpy_s(options->SvcName, _countof(options->SvcName), argv[2]);
            return true;
        }
        return false;
    }
        break;
    }
    return false;
}
void RunAsProcess(LPTSTR target_name)
{
    while(TRUE)
    {
        //if()
        RestartServiceIfNotExist(target_name);
        Sleep(CHECK_INTERVAL);
    }
}
void RunAsService()
{
    SetupEventReporter();

    // ServiceMain is entry function of service. 
    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        { (LPTSTR)SVCNAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };

    // This call returns when the service has stopped. 
    // The process should simply terminate when the call returns.
    if (!StartServiceCtrlDispatcher(DispatchTable))
        ReportEventLog(SVC_ERROR, EVTMSG("Start service failed"), GetLastError());

    TeardownEventReporter();
}

//   Entry point of EXE
int __cdecl _tmain(int argc, TCHAR* argv[])
{
    WATCHDOG_OPTIONS options = {};
    if(!ParseCliOpions(&options, argc, argv))
    {
        Usage();
        return -1;
    }
    int ret = 0;

    switch(options.Action)
    {
    case DO_ACTION::INSTALL:
        SvcInstall();
        break;
    case DO_ACTION::UNINSTALL:
        SvcUninstall();
        break;
    case DO_ACTION::CLI_MODE:
        RunAsProcess(options.SvcName);
        break;
    default:
        _tprintf(ERR_MSG_RED, _T("Invalid action specified!"));
        Usage();
        ret = -1;
    }

    return ret;
}
