#pragma once
#include "EventMessage.h"

#define EVENT_REPORT_SOURCE_SUBKEY _T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application")
#define REG_EVT_MSGFILE _T("EventMessageFile")
#define REG_EVT_SUPPORT_TYPE _T("TypesSupported")

VOID ReportEventLog(DWORD event_id, LPTSTR msg, DWORD last_error = 0);
BOOL SetupEventReporter(TCHAR* source_name, TCHAR* source_filepath);
VOID TeardownEventReporter();
