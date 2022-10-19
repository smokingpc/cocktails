#include "Common.h"

TCHAR tcsupper(TCHAR in)
{
    if (in <= _T('z') && in >= _T('a'))
        return in + (_T('Z') - _T('z'));
    return in;
}

//outbuf_len and src_len are both "size in characters"
int StrToWStr(OUT wstring& result, string& src)
{
    result = _T("");
    wchar_t temp[BIG_BUFFER_SIZE] = { 0 };
    int count = StrToWStr((wchar_t*)temp, BIG_BUFFER_SIZE, (char*)src.c_str(), src.size());
    if (count > 0)
        result = temp;
    return count;
}
//outbuf_len and src_len are both "size in characters"
int WStrToStr(OUT string& result, wstring& src)
{
    result = "";
    char temp[BIG_BUFFER_SIZE] = { 0 };
    int count = WStrToStr((char*)temp, BIG_BUFFER_SIZE, (wchar_t*)src.c_str(), src.size());
    if (count > 0)
        result = temp;
    return count;
}
