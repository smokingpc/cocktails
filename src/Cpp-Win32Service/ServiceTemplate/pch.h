#pragma once
#include <strsafe.h>
#include <stdio.h>
#include <tchar.h>

#include <Windows.h>
#include <powerbase.h>
#include <Wtsapi32.h>
#include <Dbt.h>

#include <memory>

#include "AutoSvcHandle.hpp"
#include "RegHelper.h"
#include "ServiceMain.h"
#include "ServiceEventHandler.h"
#include "RegisterServiceEvents.h"
#include "DeviceEventHandlers.h"
#include "PowerEventHandlers.h"
#include "SessionEventHandlers.h"
#include "Utils.h"
#include "EventLogReporter.h"

