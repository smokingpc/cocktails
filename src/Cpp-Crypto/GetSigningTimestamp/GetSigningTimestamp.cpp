// GetSigningTimestamp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <windows.h>

void Usage() {
    _tprintf(_T("Usage: GetSigningTimestamp.exe <file full path>\n"));
    _tprintf(_T("Example: GetSigningTimestamp.exe \"c:\\data\\myapp.exe\"\n\n"));
}

int _tmain(int argc, TCHAR* argv[])
{
    if (argc < 2) {
        Usage();
        return -1;
    }

    TCHAR* binfile = argv[1];

}
