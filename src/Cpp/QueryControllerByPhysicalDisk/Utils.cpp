#include "Common.h"

const unsigned char upper_diff = (_T('z') - _T('Z'));
TCHAR tcsupper(TCHAR in)
{
    if (in <= _T('z') && in >= _T('a'))
        return in - upper_diff;
    return in;
}
