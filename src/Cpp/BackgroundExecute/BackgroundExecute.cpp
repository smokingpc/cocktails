// BackgroundExecute.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "BackgroundExecute.h"

void LaunchCmdWithoutUI(LPWSTR cmd)
{
    BOOL ok = FALSE;
    STARTUPINFO start = {0};
    PROCESS_INFORMATION procinfo = {0};
    start.cb = sizeof(STARTUPINFO);
    start.lpTitle = (LPWSTR)L"BackgroundExecutor Launched App";
    start.dwFlags = STARTF_FORCEOFFFEEDBACK | STARTF_USESHOWWINDOW;
    start.wShowWindow = SW_HIDE;

    ok = CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &start, &procinfo);
    WaitForSingleObject(procinfo.hProcess, INFINITE);
    CloseHandle(procinfo.hThread);
    CloseHandle(procinfo.hProcess);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LPWSTR cmdline = GetCommandLine();
    int argc = 0;
    LPWSTR *argv = CommandLineToArgvW(cmdline, &argc);
    if(argc < 2)
        return -1;

    for(int i=1; i<argc; i++)
    {
        LaunchCmdWithoutUI(argv[i]);
    }
    return 0;
}
