// ConsoleRegisterDeviceEvents.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <tchar.h>
#include <windows.h>
#include <stdlib.h>
#include <Dbt.h>

HANDLE EventStop = INVALID_HANDLE_VALUE;
HANDLE ThreadHandle = INVALID_HANDLE_VALUE;
DWORD ThreadId = 0;

BOOL WINAPI ConsoleCtrlHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT) 
    {
        _tprintf(_T("Ctrl-C pressed...\n"));
        SetEvent(EventStop);
    }

    //this event is handled(consumed) by app...
    return TRUE;
}

LPTSTR GetDeviceTypeStr(DWORD type)
{
    switch(type)
    {
    case DBT_DEVTYP_DEVICEINTERFACE:
        return (LPTSTR)_T("DeviceInterface");
    case DBT_DEVTYP_HANDLE:
        return (LPTSTR)_T("Handle");
    case DBT_DEVTYP_OEM:
        return (LPTSTR)_T("OEM?");
    case DBT_DEVTYP_PORT:
        return (LPTSTR)_T("Port");
    case DBT_DEVTYP_VOLUME:
        return (LPTSTR)_T("Volume");
    }

    return (LPTSTR)_T("UNKNOWN device type");
}

void HandleDeviceArriveEvent(DEV_BROADCAST_HDR* event_hdr)
{
//identify event type and print to console
    LPTSTR type_str = GetDeviceTypeStr(event_hdr->dbch_devicetype);
    _tprintf(_T("got Arrive event: %s\n"), type_str);

    PDEV_BROADCAST_DEVICEINTERFACE event_devif = 
        (PDEV_BROADCAST_DEVICEINTERFACE) event_hdr;

    _tprintf(_T("=>DevIf [%s] arrived.\n"), event_devif->dbcc_name);
}
void HandleDeviceRemoveEvent(DEV_BROADCAST_HDR* event_hdr)
{
    //identify event type and print to console
    LPTSTR type_str = GetDeviceTypeStr(event_hdr->dbch_devicetype);
    _tprintf(_T("got DeviceRemove event: %s\n"), type_str);

    PDEV_BROADCAST_DEVICEINTERFACE event_devif =
        (PDEV_BROADCAST_DEVICEINTERFACE)event_hdr;

    _tprintf(_T("=>DevIf [%s] Removed.\n"), event_devif->dbcc_name);
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_DEVICECHANGE:
        {
            DEV_BROADCAST_HDR *event_hdr = (DEV_BROADCAST_HDR*)lParam;
            if (DBT_DEVICEARRIVAL == wParam)
                HandleDeviceArriveEvent(event_hdr);
            else if (DBT_DEVICEREMOVECOMPLETE == wParam)
                HandleDeviceRemoveEvent(event_hdr);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI ThreadWindow(LPVOID arg)
{
    const wchar_t *CLASS_NAME = L"ConsoleRegisterDeviceEvents";

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = NULL;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        NULL,
        CLASS_NAME,
        NULL,
        WS_MINIMIZE,
        0, 0, 0, 0, 
        HWND_MESSAGE,   //message only window, no GUI.
        NULL,
        NULL,
        NULL);

    _tprintf(_T("CreateWindow => hwnd=%p\n"), hwnd);

    DEV_BROADCAST_DEVICEINTERFACE filter = {0};

    filter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    filter.dbcc_classguid = GUID_DEVINTERFACE_DISK;

    HDEVNOTIFY notify = RegisterDeviceNotification(
                hwnd, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);

    _tprintf(_T("RegisterDeviceNotification=> notify=%p\n"), notify);
    _tprintf(_T("Window Message Pumping begin...\n"));
    while(WaitForSingleObject(EventStop, 5) != WAIT_OBJECT_0)
    {
    //windows message pump loop
        MSG msg = {0};
        if(GetMessage(&msg, hwnd, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    _tprintf(_T("Window Message Pumping end...\n"));

    UnregisterDeviceNotification(notify);
    return 0;
}

int _tmain(int argc, TCHAR* argv[])
{
    _tprintf(_T("**App started\n"));
    EventStop = CreateEvent(NULL, TRUE, FALSE, NULL);
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
    ThreadHandle = CreateThread(NULL, 0, ThreadWindow, NULL, 0, &ThreadId);
    _tprintf(_T("Created ThreadWindows, thread=%p ...\n"), ThreadHandle);
    WaitForSingleObject(EventStop, INFINITE);
    Sleep(3000);

    CloseHandle(EventStop);
    CloseHandle(ThreadHandle);
    _tprintf(_T("**App closed\n"));
}
