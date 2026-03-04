// WinVerifyTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <windows.h>

#pragma comment (lib, "wintrust")

void Usage() {
    _tprintf(_T("Usage: WinVerifyTest.exe <file full path>\n"));
    _tprintf(_T("Example: WinVerifyTest.exe \"c:\\data\\myapp.exe\"\n\n"));
}

BOOL CheckSignatureByWinTrust(TCHAR* filepath) {
    _tprintf(_T("Check Signature for file=\"%s\"\n"), filepath);

}

int _tmain(int argc, TCHAR* argv[])
{
    if (argc < 2) {
        Usage();
        return -1;
    }

    TCHAR* binfile = argv[1];

    if (!CheckSignatureByWinTrust(binfile)) {
        _tprintf(_T("CheckSignatureByWinTrust() returned FALSE.\n"));
    }
}
