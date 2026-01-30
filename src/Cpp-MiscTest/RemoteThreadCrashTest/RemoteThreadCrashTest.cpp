// RemoteThreadCrashTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

DWORD WINAPI ThreadCrashing(LPVOID arg) {
    Sleep(10 * 1000);
    char temp[10] = "";
    char* src = NULL;
    memcpy(temp, src, sizeof(temp));

    return 0;
}

BOOL InjectCrashThreadToProcess(DWORD pid, LPTHREAD_START_ROUTINE thread_func) {
    _tprintf(_T("Injecting thread to make process(%d) crashing\n"), pid);
    BOOL ret = TRUE;
    HANDLE proc = OpenProcess(
        PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION |
        PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
        FALSE,
        pid);

    if (!proc || INVALID_HANDLE_VALUE == proc) {
        _tprintf(_T("OpenProcess() failed. error = %d\n"), GetLastError());
        return FALSE;
    }

    // create remote thread
    HANDLE thread = CreateRemoteThread(proc, NULL, 0, ThreadCrashing, NULL, 0, NULL);
    if (NULL == thread || INVALID_HANDLE_VALUE == thread) {
        _tprintf(_T("CreateRemoteThread() failed. error = %d\n"), GetLastError());
        ret = FALSE;
    }

    if (NULL != thread && INVALID_HANDLE_VALUE != thread)
        CloseHandle(thread);
    if (NULL != proc && INVALID_HANDLE_VALUE != proc)
        CloseHandle(proc);
    return ret;
}

DWORD GetProcessIdByName(TCHAR* procname) {
    //this function get process id by process name.
    //e.g. get process id of "notepad.exe" process
    _tprintf(_T("Searching process(%s)\n"), procname);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == snapshot) {
        _tprintf(_T("CreateToolhelp32Snapshot() failed, error=%d\n"), GetLastError());
        return 0;
    }

    PROCESSENTRY32 pe = { sizeof(pe) };
    DWORD found = 0;
    if (Process32First(snapshot, &pe)) {
        do {
            if (0 == _tcsicmp(pe.szExeFile, procname)) {
                _tprintf(_T("found Process(%s) ID=%X\n"), procname, pe.th32ProcessID);
                found = pe.th32ProcessID;
                break;
            }
            memset(&pe, 0, sizeof(pe));
            pe.dwSize = sizeof(pe);
        } while (Process32NextW(snapshot, &pe));
    }
    else {
        _tprintf(_T("Process32First() failed, error=%d\n"), GetLastError());
    }

    return found;
}

BOOL EnableDebugPrivilege() {
    HANDLE token = NULL;
    TOKEN_PRIVILEGES tp = {0};
    LUID luid;

    if (!OpenProcessToken(GetCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
    {
        printf("OpenProcessToken failed. error=%d\n", GetLastError());
        return FALSE;
    }

    if (!LookupPrivilegeValueW(NULL, SE_DEBUG_NAME, &luid))
    {
        printf("LookupPrivilegeValue failed. error = %d\n", GetLastError());
        CloseHandle(token);
        return FALSE;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(token, FALSE, &tp, sizeof(tp), NULL, NULL))
    {
        printf("AdjustTokenPrivileges() failed. error = %d\n", GetLastError());
        CloseHandle(token);
        return FALSE;
    }

    CloseHandle(token);
    printf("SeDebugPrivilege turned on\n");
    return TRUE;
}

int _tmain(int argc, TCHAR* argv[]) {

    if (argc < 2) {
        _tprintf(_T("Usage: RemoteThreadCrashTest.exe <target process name>\ne.g. RemoteThreadCrashTest.exe notepad.exe\n"));
        return -1;
    }
    
    TCHAR* procname = argv[1];

    DWORD pid = GetProcessIdByName(procname);
    if (0 == pid) {
        _tprintf(_T("target process not found. exit...\n\n"));
        return -1;
    }

    if (!EnableDebugPrivilege()) {
        _tprintf(_T("Failed to debug privilege in current process. exit...\n\n"));
        return -1;
    }

    if (!InjectCrashThreadToProcess(pid, ThreadCrashing)) {
        _tprintf(_T("Inject to process failed. exit...\n\n"));
        return -1;
    }

    return 0;
}
