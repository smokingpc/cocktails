#pragma once

#define _NTSCSI_USER_MODE_

#include <tchar.h>
#include <windows.h>
#include <ntddscsi.h>
#include <ntddstor.h>
#include <scsi.h>

#include <string>
#include "TCG_OPAL_SSC.h"

#define BIG_BUFFER_SIZE         4096
#define SMALL_BUFFER_SIZE       256
#define TINY_BUFFER_SIZE        128

using namespace std;
typedef basic_string<TCHAR> tstring;

typedef struct _DISK_INFO{
    tstring Name;

}DISK_INFO, *PDISK_INFO;