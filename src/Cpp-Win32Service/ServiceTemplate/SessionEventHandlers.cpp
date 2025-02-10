#include "pch.h"

DWORD HandleSessionChangeEvent(
    DWORD event_type,
    LPVOID event_data)
{
    DWORD ret = NO_ERROR;
    DWORD session_id = PtrToUint(event_data);
    switch (event_type)
    {
    case WTS_CONSOLE_CONNECT:
        //physical console connected?
        break;
    case WTS_CONSOLE_DISCONNECT:
        //physical console disconnected?
        break;
    case WTS_SESSION_LOGON:
        //user login event
        break;
    case WTS_SESSION_LOGOFF:
        //user logout event
        break;
    case WTS_SESSION_LOCK:
        //session or console locked the screen.
        //e.g. when you press window-L combination keys.
        break;
    case WTS_SESSION_UNLOCK:
        //session or console unlocked screen.
        //you are already loged in, but screen locked. 
        //when you enter password to unlock screen, this event fired.
        break;
    default:
        ret = ERROR_INVALID_FUNCTION;
        break;
    }
    return ret;
}
