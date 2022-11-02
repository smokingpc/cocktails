#pragma once

#include <tchar.h>
#include <windows.h>
#include <ntddscsi.h>
#include <ntddstor.h>

#include <string>
#include "TCG_OPAL_SSC.h"

using namespace std;
typedef basic_string<TCHAR> tstring;

typedef struct _DISK_INFO{
    tstring Name;

}DISK_INFO, *PDISK_INFO;