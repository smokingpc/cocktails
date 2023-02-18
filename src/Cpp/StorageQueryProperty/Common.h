#pragma once

//must define this if use ntddscsi.h and scsi.h in usermode app.
#define _NTSCSI_USER_MODE_

#include <tchar.h>
#include <windows.h>
#include <scsi.h>
#include <string>

typedef std::basic_string<TCHAR> tstring;

#ifndef PAGE_SIZE
    #define PAGE_SIZE 4096
#endif

#include "Query_Functions.h"

