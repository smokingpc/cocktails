// Handle_Cpp_Exception-2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <exception>
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <DbgHelp.h>
#include <vector>
#include <cstdlib>

#pragma comment(lib, "Dbghelp.lib")

TCHAR DumpFilePath[1024] = { 0 };
MINIDUMP_EXCEPTION_INFORMATION ExParam = { 0 };

void SetupDumpSetting()
{
    GetModuleFileName(NULL, DumpFilePath, _countof(DumpFilePath));
    _tcscat_s(DumpFilePath, 1024, _T(".dmp"));
}

void TeardownDumpSetting()
{

}

void CreateDumpFile(PEXCEPTION_POINTERS ex)
{
    HANDLE file = CreateFile(DumpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    //DebugBreak();
    if (INVALID_HANDLE_VALUE != file)
    {
        ExParam.ThreadId = GetCurrentThreadId();
        ExParam.ExceptionPointers = ex;
        ExParam.ClientPointers = TRUE;
        MiniDumpWriteDump(GetCurrentProcess(),
            GetCurrentProcessId(),
            file,
            MINIDUMP_TYPE::MiniDumpWithFullMemory,
            &ExParam,
            nullptr,
            nullptr);
        CloseHandle(file);
    }
}

DWORD WINAPI WriteDumpThread(_In_ LPVOID lpParameter)
{
    PEXCEPTION_POINTERS ex = (PEXCEPTION_POINTERS)lpParameter;
    CreateDumpFile(ex);
    return ERROR_SUCCESS;
}

//LONG WINAPI ExceptionHandler(PEXCEPTION_POINTERS ex)
//{
//    switch (ex->ExceptionRecord->ExceptionCode)
//    {
//    case 0xe06d7363:    //C++ exceptions
//    {
//        //C++ exceptions is treat as STATUS_STACK_BUFFER_OVERRUN since Win10.
//        //so should handle it by another thread in VEH.
//        DWORD tid = 0;
//        HANDLE thread = CreateThread(NULL, 0, WriteDumpThread, ex, 0, &tid);
//        WaitForSingleObject(thread, INFINITE);
//        CloseHandle(thread);
//        break;
//    }
//    //case STATUS_STACK_BUFFER_OVERRUN:
//    case EXCEPTION_BREAKPOINT:
//    case EXCEPTION_ACCESS_VIOLATION:
//    case STATUS_HEAP_CORRUPTION:
//    case EXCEPTION_STACK_OVERFLOW:      //only VEH can catch this exeception.
//        //CreateDumpFile(ex);
//        break;
//    }
//
//    return EXCEPTION_CONTINUE_SEARCH;
//
//}

void CppTerminateHandler()
{
    DWORD tid = 0;
    HANDLE thread = CreateThread(NULL, 0, WriteDumpThread, NULL, 0, &tid);
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
}

int _tmain(int argc, TCHAR* argv[])
{
    SetupDumpSetting();
    //std::set_unexpected() deprecated in C++ 11, use set_terminate()
    std::set_terminate(CppTerminateHandler);
    throw new std::exception("Test Exception");
}
