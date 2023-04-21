#include "test.h"
#pragma lib("user32.lib")

void TestMsg(TCHAR *msg)
{
    MessageBox(NULL, msg, NULL, MB_OK);
}