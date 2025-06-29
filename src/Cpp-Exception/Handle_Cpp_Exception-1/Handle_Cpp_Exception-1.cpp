// Handle_Cpp_Exception-1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <eh.h>
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <DbgHelp.h>
#include <vector>
#include <cstdlib>

#pragma comment(lib, "Dbghelp.lib")
#include <exception>
#include <cstdlib>

PVOID VEH = NULL;
TCHAR SehDumpFilePath[1024] = { 0 };
TCHAR VehDumpFilePath[1024] = { 0 };
MINIDUMP_EXCEPTION_INFORMATION ExParam = { 0 };

void SetupDumpSetting()
{
    GetModuleFileName(NULL, SehDumpFilePath, _countof(SehDumpFilePath));
    GetModuleFileName(NULL, VehDumpFilePath, _countof(SehDumpFilePath));
    _tcscat_s(SehDumpFilePath, 1024, _T("-SEH.dmp"));
    _tcscat_s(VehDumpFilePath, 1024, _T("-VEH.dmp"));
}

void TeardownDumpSetting()
{

}

void CreateDumpFile(PEXCEPTION_POINTERS ex, TCHAR *path)
{
    HANDLE file = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
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

DWORD WINAPI VehWriteDumpThread(_In_ LPVOID lpParameter)
{
    PEXCEPTION_POINTERS ex = (PEXCEPTION_POINTERS)lpParameter;
    CreateDumpFile(ex, VehDumpFilePath);
    return ERROR_SUCCESS;
}

LONG WINAPI SehExceptionHandler(PEXCEPTION_POINTERS ex)
{
//    switch (ex->ExceptionRecord->ExceptionCode)
//    {
//
//    case 0x19930520:    //C++ exceptions
//    case 0xE04D5343:    //C++ exceptions
//    case 0xe06d7363:    //C++ exceptions
//    {
////        DebugBreak();
//
//        ////C++ exceptions is treat as STATUS_STACK_BUFFER_OVERRUN since Win10.
//        ////so should handle it by another thread in VEH.
//        //DWORD tid = 0;
//        //HANDLE thread = CreateThread(NULL, 0, WriteDumpThread, ex, 0, &tid);
//        //WaitForSingleObject(thread, INFINITE);
//        //CloseHandle(thread);
//        break;
//    }
//    //case STATUS_STACK_BUFFER_OVERRUN:
//    case EXCEPTION_BREAKPOINT:
//    case EXCEPTION_ACCESS_VIOLATION:
//    case STATUS_HEAP_CORRUPTION:
//    case EXCEPTION_STACK_OVERFLOW:      //only VEH can catch this exeception.
//        //CreateDumpFile(ex);
//        //DebugBreak();
//        break;
//    }
    CreateDumpFile(ex, SehDumpFilePath);
    return EXCEPTION_CONTINUE_SEARCH;

}
LONG WINAPI VehExceptionHandler(PEXCEPTION_POINTERS ex)
{
    switch (ex->ExceptionRecord->ExceptionCode)
    {

    case 0x19930520:    //C++ exceptions
    case 0xE04D5343:    //C++ exceptions
    case 0xe06d7363:    //C++ exceptions
    //case STATUS_STACK_BUFFER_OVERRUN:
    case EXCEPTION_BREAKPOINT:
    case EXCEPTION_ACCESS_VIOLATION:
    case STATUS_HEAP_CORRUPTION:
    case EXCEPTION_STACK_OVERFLOW:      //only VEH can catch this exeception.
    {
        //C++ exceptions is treat as STATUS_STACK_BUFFER_OVERRUN since Win10.
        //so should handle it by another thread in VEH.
        DWORD tid = 0;
        HANDLE thread = CreateThread(NULL, 0, VehWriteDumpThread, ex, 0, &tid);
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
        break;
    }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

int _tmain(int argc, TCHAR* argv[])
{
    SetupDumpSetting();
    LPTOP_LEVEL_EXCEPTION_FILTER old = SetUnhandledExceptionFilter(SehExceptionHandler);
    //forget SEH, it can't intercept C++ exception.

    VEH = AddVectoredExceptionHandler(TRUE, VehExceptionHandler);
    throw std::exception("Test Exception");
}
