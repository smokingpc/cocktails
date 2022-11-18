#pragma once

//must define this if use ntddscsi.h in usermode app.
#define _NTSCSI_USER_MODE_

#include <tchar.h>
#include <windows.h>
#include <ntddscsi.h>
#include <ntddstor.h>
#include <scsi.h>
#include <Setupapi.h>
#include <stdio.h>
#include <string>
#include <list>
#include <vector>

using namespace std;
typedef basic_string<TCHAR> tstring;
#include "pbkdf2\bitops.h"
#include "pbkdf2\blockwise.h"
#include "pbkdf2\chash.h"
#include "pbkdf2\handy.h"
#include "pbkdf2\hmac.h"
#include "pbkdf2\pbkdf2.h"
#include "pbkdf2\sha1.h"
#include "pbkdf2\tassert.h"

#include "Utils.h"
#include "RFC2898-PBKDF2-HMAC.h"
#include "TCG_OPAL_SSC.h"
#include "Win32_StorageDisk.h"

#define BIG_BUFFER_SIZE         4096
#define SMALL_BUFFER_SIZE       256
#define TINY_BUFFER_SIZE        128

//physical drive name example:  "\\.\PhysicalDrive2"
#define PHYSICAL_DISK_FORMAT    L"\\\\.\\PhysicalDrive%d"


