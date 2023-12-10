#include <tchar.h>
#include <windows.h>
#include <stdio.h>

#define RIFF_WAV_SIGNATURE      "RIFF"
#define WAV_FORMAT_SIGNATURE    "fmt "
#define WAV_DATA_SIGNATURE      "data"

#pragma push(1)
typedef struct _RIFF_WAV_HDR {
    char Signature[4];
    int Size;   //total file size - sizeof(_RIFF_WAV_HDR)
}RIFF_WAV_HDR, *PRIFF_WAV_HDR;
#pragma pop()

void Usage()
{
    _tprintf(_T("Usage: LoadWavFile.exe <fullpath of wav file>\n"));
    _tprintf(_T("LoadWavFile.exe d:\\mydata\\test.wav\n\n"));
}

__inline BOOL GetRiffHeader(HANDLE file, RIFF_WAV_HDR &riff)
{
    DWORD ret_size = 0;
    DWORD read_size = sizeof(RIFF_WAV_HDR);
    BOOL ok = ReadFile(file, &riff, read_size, &ret_size, NULL);
    if (!ok || ret_size != read_size)
    {
        _tprintf(_T("Read RIFF_WAV_HDR failed. LastError=%d\n"), GetLastError());
        return FALSE;
    }
    if (0 != strnicmp(riff.Signature, RIFF_WAV_SIGNATURE, 4))
    {
        _tprintf(_T("RIFF WAV SIGNATURE UNMATCHED, Is this file really a WAV?\n"));
        return FALSE;
    }
    return TRUE;
}
__inline BOOL GetWavFormatHeader(HANDLE file, WAVEFORMATEX& fmt, PVOID &ext)
{
    DWORD ret_size = 0;
    DWORD read_size = sizeof(WAVEFORMATEX);
    char fmt_sign[4] = {0};

    ext = NULL;
    read_size = 4;
    BOOL ok = ReadFile(file, fmt_sign, read_size, &ret_size, NULL);
    if (!ok || ret_size != read_size)
    {
        _tprintf(_T("Read WAVEFORMATEX SIGNATURE failed. LastError=%d\n"), GetLastError());
        return FALSE;
    }
    if (0 != strnicmp(fmt_sign, WAV_FORMAT_SIGNATURE, 4))
    {
        _tprintf(_T("WAVEFORMATEX SIGNATURE UNMATCHED, Is this file really a WAV?\n"));
        return FALSE;
    }
    
    ret_size = 0;
    read_size = sizeof(WAVEFORMATEX);
    ok = ReadFile(file, &fmt, read_size, &ret_size, NULL);
    if (!ok || ret_size != read_size)
    {
        _tprintf(_T("Read WAVEFORMATEX failed. LastError=%d\n"), GetLastError());
        return FALSE;
    }

    if(fmt.cbSize > 0)
    {
        ret_size = 0;
        read_size = fmt.cbSize;
        ext = new UCHAR[read_size];
        ok = ReadFile(file, &fmt, read_size, &ret_size, NULL);
        if (!ok || ret_size != read_size)
        {
            _tprintf(_T("Read extension data of WAVFORMATEX failed. LastError=%d\n"), GetLastError());
            delete[] ext;
            ext = NULL;
            return FALSE;
        }
    }

    return TRUE;
}
__inline BOOL GetWavRawData(HANDLE file, PVOID &ret_data, size_t ret_data_size)
{
    DWORD ret_size = 0;
    DWORD read_size = 0;
    char data_sign[4] = {0};
    int len = 0;
    read_size = sizeof(data_sign);
    BOOL ok = ReadFile(file, data_sign, read_size, &ret_size, NULL);
    if (!ok || ret_size != read_size)
    {
        _tprintf(_T("Read WAV data section signature failed. LastError=%d\n"), GetLastError());
        return FALSE;
    }
    if (0 != strnicmp(data_sign, WAV_DATA_SIGNATURE, 4))
    {
        _tprintf(_T("WAV DATA SECTION SIGNATURE UNMATCHED, Is this file really a WAV?\n"));
        return FALSE;
    }
    return TRUE;

    read_size = sizeof(len);
    ok = ReadFile(file, &len, read_size, &ret_size, NULL);
    if (!ok || ret_size != read_size)
    {
        _tprintf(_T("Read WAV data section size failed. LastError=%d\n"), GetLastError());
        return FALSE;
    }

    ret_data_size = len;
    ret_data = new UCHAR[ret_data_size];
    ok = ReadFile(file, ret_data, ret_data_size, &ret_size, NULL);
    if (!ok || ret_size != read_size)
    {
        _tprintf(_T("Read WAV data failed. LastError=%d\n"), GetLastError());
        return FALSE;
    }

    return TRUE;
}

BOOL GetWavData(TCHAR *filepath, WAVEFORMATEX &format, 
            PVOID &raw_data, size_t &raw_data_size,
            PVOID& ext_data, size_t& ext_data_size)
{
    RIFF_WAV_HDR riff = {0};
    HANDLE file = CreateFile(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(INVALID_HANDLE_VALUE == file)
    {
        _tprintf(_T("Open WAV file failed. LastError=%d\n"), GetLastError());
        return FALSE;
    }

    BOOL ok = GetRiffHeader(file, riff);
    if(!ok)
        return FALSE;

    ok = GetWavFormatHeader(file, format, ext_data);
    if(!ok)
        return FALSE;
    ext_data_size = format.cbSize;

    ok = GetWavRawData(file, raw_data, raw_data_size);
    if (!ok)
        return FALSE;

    return TRUE;
}
void FreeWavData(PVOID raw_data, PVOID ext_data)
{
    if(NULL != raw_data)
        delete[] raw_data;
    if (NULL != ext_data)
        delete[] ext_data;
}
int _tmain(int argc, _TCHAR* argv[])
{
    PVOID raw_data = NULL;
    size_t raw_data_size = 0;
    PVOID ext_data = NULL;
    size_t ext_data_size = 0;
    WAVEFORMATEX format = {0};
    TCHAR *filepath = NULL;
    if(argc < 2)
    {
        Usage();
        return ERROR_INVALID_PARAMETER;
    }
    
    filepath = argv[1];
    BOOL ok = GetWavData(filepath, format, 
                raw_data, raw_data_size, ext_data, ext_data_size);
    if(ok && NULL != raw_data)
        FreeWavData(raw_data, ext_data);

    return ERROR_SUCCESS;
}
