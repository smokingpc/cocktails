// VectorExceptionHandlerTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <DbgHelp.h>

#pragma comment(lib, "Dbghelp.lib")

PVOID VEH = NULL;
TCHAR DumpFilePath[1024] = {0};
MINIDUMP_EXCEPTION_INFORMATION ExParam = {0};

void SetupDumpSetting()
{
    GetModuleFileName(NULL, DumpFilePath, _countof(DumpFilePath));
    _tcscat_s(DumpFilePath, 1024, _T(".dmp"));
}

void TeardownDumpSetting()
{
    //if(NULL != DumpHandle)
    //{
    //    CloseHandle(DumpHandle);
    //    DumpHandle = NULL;
    //}
}

void CreateDumpFile(PEXCEPTION_POINTERS ex)
{
    HANDLE file = CreateFile(DumpFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
    //DebugBreak();
    if(INVALID_HANDLE_VALUE != file)
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

DWORD WINAPI StackOverflowHandlerThread(_In_ LPVOID lpParameter)
{
    PEXCEPTION_POINTERS ex = (PEXCEPTION_POINTERS)lpParameter;
    CreateDumpFile(ex);
    return ERROR_SUCCESS;
}

LONG WINAPI ExceptionHandler(PEXCEPTION_POINTERS ex)
{
    switch (ex->ExceptionRecord->ExceptionCode)
    {
    case STATUS_HEAP_CORRUPTION:
    case STATUS_STACK_BUFFER_OVERRUN:
        CreateDumpFile(ex);
        break;
    case STATUS_STACK_OVERFLOW: //only VEH can catch this exeception.
    {
    //in stack overflow status_code, MiniDumpWriteDump() won't succeed
    //because this function also need consume much stack and heap.
    //So we have to put this exception to another thread with clean stack.
        DWORD tid = 0;
        HANDLE thread = CreateThread(NULL, 0, StackOverflowHandlerThread, ex, STACK_SIZE_PARAM_IS_A_RESERVATION, &tid);
        WaitForSingleObject(thread, INFINITE);
    }
        break;
    default:
        break;
    }

    return EXCEPTION_CONTINUE_SEARCH;

}

DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{
    //MAKE IT CRASH
    //DebugBreak();

    char crash1[1024] = { 0 };
    return ThreadProc(crash1);
}

int _tmain(int argc, TCHAR* argv[])
{
    SetupDumpSetting();

    //UINT mode = GetErrorMode();
    //mode |= SEM_NOGPFAULTERRORBOX;  //don't popup error window when crash.
    //SetErrorMode(mode);

    //register VEH
    VEH = AddVectoredExceptionHandler(TRUE, ExceptionHandler);
    SetUnhandledExceptionFilter(ExceptionHandler);

    DWORD tid = 0;
    HANDLE thread = CreateThread(NULL, 4096, ThreadProc, NULL, STACK_SIZE_PARAM_IS_A_RESERVATION, &tid);
    Sleep(1000*10);

    SetUnhandledExceptionFilter(NULL);
    //if app exit without error, remember unregister VEH!!
    if(NULL != VEH)
        RemoveVectoredExceptionHandler(VEH);

    TeardownDumpSetting();
}
