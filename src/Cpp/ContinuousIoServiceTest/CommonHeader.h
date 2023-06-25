#pragma once
#include <tchar.h>
#include <windows.h>
#include <string>
#include <iostream>

#include "EventMessageDefinition.h"
#include "ServiceFunctions.h"

#define SERVICE_NAME  (LPTSTR)_T("SpcIoTest")
#define IO_INTERVAL     100     //do I/O per 100 ms
#define IO_SIZE         64
#define IO_DATA         "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890+-"
#define DEFAULT_IO_TARGET   (LPTSTR)_T("X:\\TestIo.bin")
//sizeof also count the NULL-terminator in....
//it is NOT strlen
static_assert(sizeof(IO_DATA) == (IO_SIZE + 1), "[error] sizeof(IO_DATA) != IO_SIZE!!");
