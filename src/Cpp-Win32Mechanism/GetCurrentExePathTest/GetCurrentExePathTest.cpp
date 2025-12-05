// GetCurrentExePathTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int _tmain(int argc, TCHAR* argv[])
{
    TCHAR path[MAX_PATH + 1] = { 0 };
    TCHAR drive[_MAX_DRIVE + 1] = { 0 };
    TCHAR dir[_MAX_DIR + 1] = { 0 };

    GetModuleFileName(NULL, path, MAX_PATH);
    _tsplitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);

    RtlZeroMemory(path, sizeof(path));
    _stprintf_s(path, MAX_PATH, _T("%s%s"), drive, dir);

    //result of path has a "\\" on tail.
    _tprintf(_T("Dir of current exe = %s\n"), path);
}
