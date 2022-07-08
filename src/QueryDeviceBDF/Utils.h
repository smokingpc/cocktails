#pragma once

TCHAR tcslower(TCHAR in);
//outbuf_len and src_len are both "size in characters"
int StrToWStr(OUT wstring& result, string& src);
inline int StrToWStr(wchar_t* outbuf, int outbuf_len, char* src, int src_len)
{
    return MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, src, src_len, outbuf, outbuf_len);
}

//outbuf_len and src_len are both "size in characters"
int WStrToStr(OUT string& result, wstring& src);
inline int WStrToStr(char* outbuf, int outbuf_len, wchar_t* src, int src_len)
{
    BOOL use_defchar = FALSE;
    return WideCharToMultiByte(CP_ACP, WC_ERR_INVALID_CHARS, src, src_len, outbuf, outbuf_len, NULL, &use_defchar);
}

inline void ParseEnumeratorByInstanceid(OUT tstring& enumerator, tstring& id)
{
    enumerator = _T("");
    size_t found = id.find(_T('\\'), 0);
    if (tstring::npos != found)
    {
        enumerator = id.substr(0, found);
    }
}
inline void ParseEnumeratorByHwid(OUT tstring& enumerator, tstring& hwid)
{
    enumerator = _T("");
    size_t found = hwid.find(_T('\\'), 0);
    if (tstring::npos != found)
    {
        enumerator = hwid.substr(0, found);
    }
}

void ParseBDF(wchar_t* src, BDF_INFO& info);
void ParseBDF(char* src, BDF_INFO& info);
