#pragma once
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
TCHAR tcsupper(TCHAR in);

bool StrCompare(tstring str1, tstring str2);
