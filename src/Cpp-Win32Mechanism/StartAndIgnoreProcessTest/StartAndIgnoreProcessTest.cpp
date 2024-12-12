// StartAndIgnoreProcessTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <windows.h>
#include <stdio.h>

int _tmain(int argc, TCHAR* argv[]) 
{
    TCHAR exepath[1024] = { _T("C:\\1\\HelloWorldTest.exe") };

    //note: don't use const to cmd. CreateProcess() will modify this string if exepath is NULL.
    TCHAR cmd[1024] = { _T("\"C:\\1\\HelloWorldTest.exe\" -pause") };
    BOOL ok = FALSE;
    STARTUPINFO start = { 0 };
    PROCESS_INFORMATION procinfo = { 0 };
    start.cb = sizeof(STARTUPINFO);
    start.lpTitle = (TCHAR*)_T("MyTest Window");
    start.dwFlags = STARTF_USESHOWWINDOW;
    start.wShowWindow = SW_SHOW;

    ok = CreateProcess(exepath, (LPTSTR)cmd, NULL, NULL, FALSE, 0, NULL, NULL, &start, &procinfo);
    if (ok)
    {
        _tprintf(_T("Close child process handles\n"));
        if(NULL == procinfo.hThread || INVALID_HANDLE_VALUE == procinfo.hThread)
            _tprintf(_T("INVALID procinfo.hThread!\n"));
        else
            CloseHandle(procinfo.hThread);
        if (NULL == procinfo.hProcess || INVALID_HANDLE_VALUE == procinfo.hProcess)
            _tprintf(_T("INVALID procinfo.hProcess!\n"));
        else
            CloseHandle(procinfo.hProcess);
    }
    else
    {
        _tprintf(_T("Create child process failed(%d)\n"), GetLastError());
    }
}
