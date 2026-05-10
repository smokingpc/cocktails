#include "pch.h"

HANDLE EventSource = NULL;

static BOOL SetReporterSource(TCHAR* source_name, TCHAR* source_filepath, DWORD support_types)
{
    CRegHelper reg;
    DWORD size = static_cast<DWORD>(_tcslen(source_filepath) * sizeof(TCHAR));
    std::unique_ptr<TCHAR> keypath(new TCHAR[GENERIC_BUFFER_SIZE]);
    memset(keypath.get(), 0, GENERIC_BUFFER_SIZE * sizeof(TCHAR));
    _stprintf_s(keypath.get(), GENERIC_BUFFER_SIZE, _T("%s\\%s"), EVENT_REPORT_SOURCE_SUBKEY, source_name);

    DWORD err = reg.OpenHKLM(keypath.get(), 0, false);
    if (ERROR_SUCCESS != err) {
        return FALSE;
    }
    
    if (reg.WriteValue((TCHAR*)REG_EVT_MSGFILE, source_filepath, size) && 
        reg.WriteValue((TCHAR*)REG_EVT_SUPPORT_TYPE, support_types)) {
        return TRUE;
    }

    return FALSE;
}

BOOL SetupEventReporter(TCHAR *source_name, TCHAR* source_filepath)
{
    //put report source to corresponding registry
    //when calling RegisterEventSource(), system will rescan this registry again.
    DWORD flag = EVENTLOG_SUCCESS | EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE | EVENTLOG_AUDIT_SUCCESS;
    SetReporterSource(source_name, source_filepath, flag);

    //register EventSource and get handle.
    //It is used to call ReportEvent(), which report to event log of system.
    EventSource = RegisterEventSource(NULL, source_name);
    return (NULL != EventSource);
}
void TeardownEventReporter()
{
    DeregisterEventSource(EventSource);
    EventSource = NULL;
}

VOID ReportEventLog(DWORD event_id, LPTSTR msg, DWORD last_error)
{
    LPCTSTR report[2] = { 0 };
    TCHAR buffer[SMALL_BUFFER_SIZE] = { 0 };

    StringCchPrintf(buffer, SMALL_BUFFER_SIZE, _T("%s, error = %d"), msg, last_error);

    report[0] = SVCNAME;
    report[1] = buffer;

    ReportEvent(EventSource,        // event log handle
        EVENTLOG_ERROR_TYPE, // event type
        0,                   // event category
        event_id,           // event identifier
        NULL,                // no security identifier
        2,                   // size of lpszStrings array
        0,                   // no binary data
        report,              // array of report strings
        NULL);               // no binary data

}
