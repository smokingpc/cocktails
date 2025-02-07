#pragma once
#include <strsafe.h>
#include <stdio.h>
#include <tchar.h>

#include <Windows.h>
#include <powerbase.h>
#include <Wtsapi32.h>
#include <Dbt.h>
#include <winioctl.h>

#include "Service.h"
#include "ServiceEventHandler.h"
#include "RegisterServiceEvents.h"
#include "DeviceEventHandlers.h"
#include "PowerEventHandlers.h"
#include "Utils.h"
#include "EventMessage.h"
