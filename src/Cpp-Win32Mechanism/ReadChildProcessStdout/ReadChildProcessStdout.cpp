#include <windows.h>
#include <string>

//#define DEFAULT_CMD "\%WINDIR\%\\System32\\robocopy.exe"
#define DEFAULT_CMD "C:\\Windows\\System32\\robocopy.exe"

void ReadAndPrintChildStdout(HANDLE child_stdout)
{
    char buffer[65] = { 0 };
    DWORD ret_size = 0;

    while(TRUE == ReadFile(child_stdout, buffer, 64, &ret_size, NULL))
    {
        printf(buffer);
        ZeroMemory(buffer, 65);
    }
    //BOOL ok = ReadFile(child_stdout, buffer, 16, &ret_size, NULL);
    //if(!ok)
    //    printf("Read from child stdout failed, lasterror=(0x%08X)\n", GetLastError());
    //else
    //    printf("%s\n", buffer);
}

int main(int argc , char* argv[])
{
    char *cmd = NULL;

    if(argc == 1)
        cmd = (char*)DEFAULT_CMD;
    else
        cmd = argv[1];

    char *exe_name = strrchr(cmd, '\\')+1;
    STARTUPINFOA startinfo = {0};
    PROCESS_INFORMATION procinfo = { 0 };
    HANDLE read_pipe = INVALID_HANDLE_VALUE;      //read from stdout endpoint
    HANDLE write_pipe = INVALID_HANDLE_VALUE;     //write to stdout endpoint
    
    //PIPE should be able to inherite parent handle in next API
    SECURITY_ATTRIBUTES attr = {0};
    attr.nLength = sizeof(SECURITY_ATTRIBUTES);
    attr.bInheritHandle = TRUE;
    attr.lpSecurityDescriptor = NULL;

    BOOL ok = CreatePipe(&read_pipe, &write_pipe, &attr, 0);
    if(!ok)
    {
        printf("Create stdout pipe pair failed, lasterror=(0x%08X), exit....\n", GetLastError());
        return -1;
    }
    //if (!SetHandleInformation(read_pipe, HANDLE_FLAG_INHERIT, 0))
    //{
    //    printf("SetHandleInformation on pipe pair failed, lasterror=(0x%08X), exit....\n", GetLastError());
    //}
    startinfo.cb = sizeof(STARTUPINFO);
    startinfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    startinfo.wShowWindow = SW_HIDE; //I don't want to see child window :p
    startinfo.hStdOutput = write_pipe;

    ok = CreateProcessA(NULL, cmd, &attr, NULL, TRUE, 0, NULL, NULL, &startinfo, &procinfo);
    if(!ok)
    {
        printf("Create process (%s)failed, lasterror=(0x%08X), exit....\n", cmd, GetLastError());
    }

    //wait child process finish.
    WaitForSingleObject(procinfo.hProcess, INFINITE);
    //Before we read pipe, we have to close process and write_pipe first.
    //If not to do so, read pipe will be NEVER reach EOF.
    //Tt will be fall into infinite wait in ReadFile.
    if (NULL != procinfo.hThread && INVALID_HANDLE_VALUE != procinfo.hThread)
        CloseHandle(procinfo.hThread);
    if (NULL != procinfo.hProcess && INVALID_HANDLE_VALUE != procinfo.hProcess)
        CloseHandle(procinfo.hProcess);
    CloseHandle(startinfo.hStdOutput);

    ReadAndPrintChildStdout(read_pipe);
    if (NULL != read_pipe && INVALID_HANDLE_VALUE != read_pipe)
        CloseHandle(read_pipe);
    return 0;
}
