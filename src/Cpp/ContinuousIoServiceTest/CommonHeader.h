#pragma once
#include <tchar.h>
#include <windows.h>
#include <string>
#include <iostream>

#include "EventMessageDefinition.h"
#include "ServiceFunctions.h"

#define SERVICE_NAME  (LPWSTR)_T("SpcIoTest")
#define IO_INTERVAL     100     //do I/O per 100 ms
