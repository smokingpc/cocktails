#include "Common.h"

TCHAR tcslower(TCHAR in)
{
    if (in <= _T('Z') && in >= _T('A'))
        return in - (_T('Z') - _T('z'));
    return in;
}


void ParseBDF(wchar_t* src, BDF_INFO& info)
{
    wregex test(L"^PCI[^0-9]*(\\d+)[^0-9]*(\\d+)[^0-9]*(\\d+)$");
    wsmatch match;
    wstring src_str = src;
    uint32_t bus = 0, dev = 0, func = 0;
    if (regex_search(src_str, match, test))
    {
        bus = _wtol(match[1].str().c_str());
        dev = _wtol(match[2].str().c_str());
        func = _wtol(match[3].str().c_str());

        info.Bus = bus & 0xFF;
        info.Dev = dev & 0xFF;
        info.Func = func & 0xFF;
    }
}
void ParseBDF(char* src, BDF_INFO& info)
{
    regex test("^PCI[^0-9]*(\\d+)[^0-9]*(\\d+)[^0-9]*(\\d+)$");
    smatch match;
    string src_str = src;
    uint32_t bus = 0, dev = 0, func = 0;
    if (regex_search(src_str, match, test))
    {
        bus = atol(match[1].str().c_str());
        dev = atol(match[2].str().c_str());
        func = atol(match[3].str().c_str());

        info.Bus = bus & 0xFF;
        info.Dev = dev & 0xFF;
        info.Func = func & 0xFF;
    }
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
