// GetSpecialDirTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <WtsApi32.h>
#include <ShlObj.h>
#include <Shlwapi.h>

#pragma comment(lib, "Wtsapi32.lib")

void TestProgramData()
{
    HRESULT hres = S_OK;
    PWCHAR result = NULL;
    //get "C:\ProgramData" => %ALLUSERSPROFILE%
    hres = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, NULL, &result);
    if (S_OK == hres)
    {
        wprintf(L"SHGetKnownFolderPath(FOLDERID_ProgramData) got dir=%s\n", result);
        CoTaskMemFree(result);
    }
    else
    {
        _tprintf(_T("SHGetKnownFolderPath(FOLDERID_ProgramData) failed, error=%08X\n"), hres);
    }
}
void TestProgramFiles()
{
    HRESULT hres = S_OK;
    PWCHAR result = NULL;

    //get "C:\Program Files" or "C:\Program Files (x86)", depends on process.
    // 32bit process will get "C:\Program Files (x86)".
    hres = SHGetKnownFolderPath(FOLDERID_ProgramFiles, 0, NULL, &result);
    if (S_OK == hres)
    {
        wprintf(L"SHGetKnownFolderPath(FOLDERID_ProgramFiles) got dir=%s\n", result);
        CoTaskMemFree(result);
    }
    else
    {
        _tprintf(_T("SHGetKnownFolderPath(FOLDERID_ProgramFiles) failed, error=%08X\n"), hres);
    }
}
void TestUserAppDataRoaming()
{
    HRESULT hres = S_OK;
    PWCHAR result = NULL;

    //this way get "C:\Users\<user id>\AppData\Roaming" from current user of process
    hres = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &result);
    if (S_OK != hres)
    {
        _tprintf(_T("SHGetKnownFolderPath(FOLDERID_RoamingAppData) failed, error=%08X\n"), hres);
        return;
    }
    wprintf(L"SHGetKnownFolderPath(FOLDERID_RoamingAppData) got dir=%s\n", result);
    CoTaskMemFree(result);
}

int _tmain(int argc, TCHAR* argv[])
{
    TestProgramData();
    TestProgramFiles();
    TestUserAppDataRoaming();
}
