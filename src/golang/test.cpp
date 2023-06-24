
#pragma lib("user32.lib")
extern "C" {

#include "test.h"
void TestMsg(TCHAR *msg)
{
    MessageBox(NULL, msg, NULL, MB_OK);
}

}