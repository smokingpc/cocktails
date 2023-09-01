// ConsoleCtrlHandlerTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <windows.h>
#include <stdlib.h>

HANDLE EventStop = INVALID_HANDLE_VALUE;

BOOL WINAPI ConsoleCtrlHandler(DWORD signal)
{
    switch (signal)
    {
    case CTRL_C_EVENT:
        _tprintf(_T("got Ctrl-C event.\n"));
        SetEvent(EventStop);
        break;
    case CTRL_BREAK_EVENT:
        _tprintf(_T("got Ctrl-BREAK event.\n"));
        SetEvent(EventStop);
        break;
    case CTRL_CLOSE_EVENT:  
    //Right click on console title and press "close" in context menu
        _tprintf(_T("got CTRL_CLOSE_EVENT.\n"));
        SetEvent(EventStop);
        break;
    case CTRL_LOGOFF_EVENT:
    //sent when current user logogg
        _tprintf(_T("got CTRL_LOGOFF_EVENT.\n"));
        SetEvent(EventStop);
        break;
    case CTRL_SHUTDOWN_EVENT:
    //sent when OS shutting down.
        _tprintf(_T("got CTRL_SHUTDOWN_EVENT.\n"));
        SetEvent(EventStop);
        break;
    default:
        _tprintf(_T("UNKNOWN Ctrl Event.\n"));
        break;
    }

    return TRUE;
}

//normal process exit handler
void __cdecl exit_handler()
{
    _tprintf(_T("Exit captured!!\n"));
}

int _tmain(int argc, TCHAR* argv[])
{
    EventStop = CreateEvent(NULL, TRUE, FALSE, NULL);
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
    atexit(exit_handler);   //normal process exit handler
    _tprintf(_T("Start....\n"));
    if(NULL != EventStop && INVALID_HANDLE_VALUE != EventStop)
    {
        
        WaitForSingleObject(EventStop, INFINITE);
        CloseHandle(EventStop);
        _tprintf(_T("=> WaitForSingleObject() got signal.\n"));
    }
    SetConsoleCtrlHandler(ConsoleCtrlHandler, FALSE);
    _tprintf(_T("End....\n"));
}
