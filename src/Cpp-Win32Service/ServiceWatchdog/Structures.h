#pragma once
typedef struct _WATCHDOG_OPTIONS {
    DO_ACTION   Action;
    TCHAR       SvcName[MAX_PATH];
}WATCHDOG_OPTIONS, * PWATCHDOG_OPTIONS;
