#include "pch.h"

DWORD HandleModernStandbyEvent(PPOWERBROADCAST_SETTING setting)
{
    //going into S0 Low Power Idle state. (enter AOAC state)
    //wakeup from S0 Low Power Idle state. (leave AOAC state)
    AOAC_STATE*state = (AOAC_STATE*)&setting->Data;

    switch(*state)
    {
        //system entering ACTIVE state.
        //After register power event, this event will be fired once immediately.
    case AOAC_STATE::ACTIVE:
        break;
        //system entering ModernStandby mode
    case AOAC_STATE::STANDBY:
        break;
    }

    return NO_ERROR;
}
DWORD HandleConsoleDisplayEvent(PPOWERBROADCAST_SETTING setting)
{
    MONITOR_STATE* state = (MONITOR_STATE*)&setting->Data;
    switch(*state)
    {
    case MONITOR_STATE::ON:
        break;
    case MONITOR_STATE::OFF:
        break;
    //(notebook)console monitor become darker(low light mode) but still on.
    case MONITOR_STATE::DIMMED:
        break;
    }

    return NO_ERROR;
}
DWORD HandleLegacySuspendEvent(PPOWERBROADCAST_SETTING setting)
{
    return NO_ERROR;
}
DWORD HandleLegacyWakeupByAutoEvent(PPOWERBROADCAST_SETTING setting)
{
    return NO_ERROR;
}
DWORD HandleLegacyWakeupByUserEvent(PPOWERBROADCAST_SETTING setting)
{
    return NO_ERROR;
}
DWORD HandlePowerSourceChangeEvent(PPOWERBROADCAST_SETTING setting)
{
    return NO_ERROR;
}

DWORD HandleControlPowerEvent(
    DWORD event_type,
    LPVOID event_data)
{
    DWORD ret = NO_ERROR;
    PPOWERBROADCAST_SETTING setting = (PPOWERBROADCAST_SETTING)event_data;

    switch (event_type)
    {
    case PBT_APMRESUMEAUTOMATIC:
        //resume without any user activities. e.g. magic packet or BIOS wakeup event.
        //This event is also "pre-event" of PBT_APMRESUMESUSPEND
        ret = HandleLegacyWakeupByAutoEvent(setting);
        break;
    case PBT_APMRESUMESUSPEND:
        //resumed by user activities.```
        ret = HandleLegacyWakeupByUserEvent(setting);
        break;
#if 0
    case PBT_APMQUERYSUSPEND:
        //https://learn.microsoft.com/en-us/windows/win32/power/pbt-apmquerysuspend
        //this event is obsoleted since VISTA.
        break;
#endif
    case PBT_APMSUSPEND:
        //computer is entering suspend mode.
        ret = HandleLegacySuspendEvent(setting);
        break;
    case PBT_APMPOWERSTATUSCHANGE:
        //power source AC to DC, or DC to AC event.
        //this event also sent when DC power(battery) is lower than "low power threshold".
        ret = HandlePowerSourceChangeEvent(setting);
        break;
    case PBT_POWERSETTINGCHANGE:    //modern standby 
        if (GUID_MONITOR_POWER_ON == setting->PowerSetting)
            ret = HandleModernStandbyEvent(setting);
        else if (GUID_CONSOLE_DISPLAY_STATE == setting->PowerSetting)
            ret = HandleConsoleDisplayEvent(setting);
        break;
    }

    return ret;
}
