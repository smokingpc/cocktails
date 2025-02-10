#pragma once
DWORD HandleModernStandbyEvent(PPOWERBROADCAST_SETTING setting);

//console display event handler handled 3 different events:
//1.display monitor off
//2.display monitor on
//3.display monitor dimmed
//they are easily found in laptop.
DWORD HandleConsoleDisplayEvent(PPOWERBROADCAST_SETTING setting);

DWORD HandleLegacySuspendEvent(PPOWERBROADCAST_SETTING setting);

//There are two types of wakeup event in legacy suspend:
//one is for automatic event(e.g. BIOS wakeup timer, network magic packet...etc)
//another is for user interaction.(e.g. user type a keystroke, moving mouse, press power button....etc.)
DWORD HandleLegacyWakeupByAutoEvent(PPOWERBROADCAST_SETTING setting);

//If user interaction happend, system will fire a auto event before user interact event.
//It means :
//[Auto Wakeup] there is only one wakeup event.
//[User interact Wakeup] system fire a auto event, then fire a user interact event.
DWORD HandleLegacyWakeupByUserEvent(PPOWERBROADCAST_SETTING setting);

//PowerSourceChange means power from AC to DC battery, or from DC battery back to AC.
DWORD HandlePowerSourceChangeEvent(PPOWERBROADCAST_SETTING setting);

DWORD HandleControlPowerEvent(
    DWORD event_type,
    LPVOID event_data);
