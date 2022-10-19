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

bool StrCompare(tstring str1, tstring str2)
{
    if (str1.size() != str2.size())
        return false;
    tstring temp1 = str1;
    tstring temp2 = str2;

    std::transform(temp1.begin(), temp1.end(), temp1.begin(), tcsupper);
    std::transform(temp2.begin(), temp2.end(), temp2.begin(), tcsupper);
    if (0 != temp1.compare(temp2))
        return false;

    return true;

}