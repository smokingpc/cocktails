#pragma once

DWORD HandleControlStop(
    DWORD event_type,
    LPVOID event_data);
DWORD HandleControlInterrogate(
    DWORD event_type,
    LPVOID event_data);
DWORD HandleControlPreshutdown(
    DWORD event_type,
    LPVOID event_data);
DWORD HandleControlShutdown(
    DWORD event_type,
    LPVOID event_data);

