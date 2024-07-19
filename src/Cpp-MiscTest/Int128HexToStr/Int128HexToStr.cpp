
#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE     128

const BYTE EmptyInt128[128] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const BYTE EmptyInt64[64] = { 0, 0, 0, 0, 0, 0, 0, 0 };

const BYTE Int64_1[8] = {0x04,0xd3,0x02,0,0,0,0,0};
const BYTE Int64_2[8] = { 0x04,0xd3,0x02,0x93,0,0,0,0};
const BYTE Int64_3[8] = { 0x04,0xd3,0x02,0x93,0x0c,0,0,0};
const BYTE Int64_4[8] = { 0x04,0xd3,0x02,0x93,0xa5,0,0,0};
const BYTE Int128_5[16] = { 0x04,0xd3,0x02,0x12,0x05,0xac,0x8d,0x10,0xe3,0,0,0,0,0,0,0 };
const BYTE Int128_6[16] = { 0x04,0xd3,0x02,0x12,0x05,0xac,0x8d,0x10,0x09,0,0,0,0,0,0,0 };

//"be" stands for BigEndian
void HexInt128ToDecStr(TCHAR *outbuf, size_t outbuf_cch, BYTE* int128_be)
{
    int idx = 0;
    BYTE quotient_be[16] = { 0 };
    BYTE temp = 0;
    bool is_empty = true;

    do
    {
        temp = (temp << 4) + (int128_be[idx] >> 4);
        quotient_be[idx] = (temp / 10) << 4;
        temp = ((temp % 10) << 4) + (int128_be[idx] & 0x0F);
        quotient_be[idx] += (temp / 10);
        temp = (temp % 10);
        idx++;
    } while (idx < 16);

    TCHAR temp_str[2] = { 0 };
    _itot_s(temp, temp_str, 2, 10);
    outbuf[0] = temp_str[0];
    
    for(int i=0; i<16; i++)
    {
        if(quotient_be[i] != 0)
        {
            is_empty = false;
            break;
        }
    }

    //calculation finished if quotient is empty
    if (is_empty)
        return;
    else
        HexInt128ToDecStr(outbuf + 1, outbuf_cch - 1, quotient_be);
}
void HexInt64ToDecStr(TCHAR* outbuf, size_t outbuf_cch, BYTE* int64_be)
{
    int idx = 0;
    BYTE quotient_be[8] = {0};
    BYTE temp = 0;
    do
    {
        temp = (temp<<4) + (int64_be[idx] >> 4);
        quotient_be[idx] = (temp/10)<<4;
        temp = ((temp % 10) << 4) + (int64_be[idx] & 0x0F);
        quotient_be[idx] += (temp / 10);
        temp = (temp % 10);
        idx++;
    }while(idx < 8);

    TCHAR temp_str[2] = {0};
    _itot_s(temp, temp_str, 2, 10);
    outbuf[0] = temp_str[0];

    //calculation finished if quotient is empty
    if(0 == *(UINT64*)quotient_be)
        return;
    else
        HexInt64ToDecStr(outbuf+1, outbuf_cch-1, quotient_be);
}
void ReverseEndian(BYTE* src, BYTE* dst, size_t bytes)
{
    for(int i=0; i<bytes; i++)
    {
        dst[(bytes - 1 - i)] = src[i];
    }
}

int _tmain(int argc, TCHAR* argv[])
{
    BYTE int128_be[16] = {0};
    BYTE int64_be[16] = { 0 };
    TCHAR result[BUFFER_SIZE] = {0};

    ReverseEndian((BYTE*)Int64_1, int64_be, 8);
    ZeroMemory(result, BUFFER_SIZE);
    HexInt64ToDecStr(result, BUFFER_SIZE, int64_be);
    _tcsrev(result);
    _tprintf(_T("Int64_1 Decimal=%s\n"), result);

    ReverseEndian((BYTE*)Int64_2, int64_be, 8);
    ZeroMemory(result, BUFFER_SIZE);
    HexInt64ToDecStr(result, BUFFER_SIZE, int64_be);
    _tcsrev(result);
    _tprintf(_T("Int64_2 Decimal=%s\n"), result);

    ReverseEndian((BYTE*)Int64_3, int64_be, 8);
    ZeroMemory(result, BUFFER_SIZE);
    HexInt64ToDecStr(result, BUFFER_SIZE, int64_be);
    _tcsrev(result);
    _tprintf(_T("Int64_3 Decimal=%s\n"), result);

    ReverseEndian((BYTE*)Int64_4, int64_be, 8);
    ZeroMemory(result, BUFFER_SIZE);
    HexInt64ToDecStr(result, BUFFER_SIZE, int64_be);
    _tcsrev(result);
    _tprintf(_T("Int64_4 Decimal=%s\n"), result);

    ReverseEndian((BYTE*)Int128_5, int128_be, 16);
    ZeroMemory(result, BUFFER_SIZE);
    HexInt128ToDecStr(result, BUFFER_SIZE, int128_be);
    _tcsrev(result);
    _tprintf(_T("Int128_5 Decimal=%s\n"), result);

    ReverseEndian((BYTE*)Int128_6, int128_be, 16);
    ZeroMemory(result, BUFFER_SIZE);
    HexInt128ToDecStr(result, BUFFER_SIZE, int128_be);
    _tcsrev(result);
    _tprintf(_T("Int128_6 Decimal=%s\n"), result);

    return 0;
}
