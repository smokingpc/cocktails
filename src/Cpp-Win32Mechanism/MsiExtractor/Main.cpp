// MsiExtractor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <pathcch.h>
#include <Msi.h>
#include <MsiQuery.h>

#pragma comment(lib, "msi.lib")
#pragma comment(lib, "pathcch.lib")

#define MSI_FIELD_FILENAME          1
#define MSI_FIELD_BINARY_STREAM     2

__inline BOOL IsFileExist(LPTSTR filepath)
{
    DWORD ret = GetFileAttributes(filepath);
    if(INVALID_FILE_ATTRIBUTES == ret)
        return FALSE;

    return ((FILE_ATTRIBUTE_DIRECTORY & ret) != FILE_ATTRIBUTE_DIRECTORY);
}
__inline BOOL IsDirExist(LPTSTR path)
{
    DWORD ret = GetFileAttributes(path);
    if (INVALID_FILE_ATTRIBUTES == ret)
        return FALSE;

    return ((FILE_ATTRIBUTE_DIRECTORY & ret) == FILE_ATTRIBUTE_DIRECTORY);
}

void PrintUsage()
{
    _tprintf(_T("MsiExtractor.exe is a simple tool to extract content of MSI package.\n"));
    _tprintf(_T("Usage:\n"));
    _tprintf(_T("\tMsiExtractor.exe <source msi file> <extract to dir>\n"));
    _tprintf(_T("Example:\n"));
    _tprintf(_T("\tIf want to extract c:\\data\\test.msi to c:\\output, type\n"));
    _tprintf(_T("\tMsiExtractor.exe \"c:\\data\\test.msi \"c:\\output\"\"\n"));
    _tprintf(_T("\n"));
}

BOOL AppendToFile(LPTSTR filepath, LPBYTE buffer, DWORD buf_size)
{
    HANDLE file = CreateFile(filepath, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (NULL == file)
    {
        _tprintf(_T("CreateFile(%s) failed, err(%d)\n"), filepath, GetLastError());
        return FALSE;
    }

    DWORD ret = 0;
    DWORD wrote = 0;
    ret = SetFilePointer(file, 0, NULL, FILE_END);
    if(INVALID_SET_FILE_POINTER == ret)
    {
        _tprintf(_T("SetFilePointer(%s) failed, err(%d)\n"), filepath, GetLastError());
        CloseHandle(file);
        return FALSE;
    }

    if(!WriteFile(file, buffer, buf_size, &wrote, NULL))
    {
        _tprintf(_T("WriteFile(%s) failed, err(%d)\n"), filepath, GetLastError());
        CloseHandle(file);
        return FALSE;
    }

    CloseHandle(file);
    return TRUE;
}

void ExtractMsiFiles(LPTSTR src_file, LPTSTR dest_dir)
{
    MSIHANDLE db = NULL;
    MSIHANDLE view = NULL;
    MSIHANDLE record = NULL;

    //MSI content just like a database.
    //open a db-> open a view -> execute SQL cmd for this view-> get data you want.
    DWORD ret = MsiOpenDatabase(src_file, MSIDBOPEN_DIRECT, &db);
    if(ERROR_SUCCESS != ret) 
    {
        _tprintf(_T("MsiOpenDatabase() failed, err(%d)\n"), ret);
        goto END;
    }

    //SQL cmd here can use "?" as parameter.
    //Parameter input from MsiViewExecute();
    ret = MsiDatabaseOpenView(db, _T("Select * From _Streams"), &view);
    if (ERROR_SUCCESS != ret)
    {
        _tprintf(_T("MsiDatabaseOpenView() failed, err(%d)\n"), ret);
        goto END;
    }

    ret = MsiViewExecute(view, NULL);
    if (ERROR_SUCCESS != ret)
    {
        _tprintf(_T("MsiViewExecute() failed, err(%d)\n"), ret);
        goto END;
    }

    while(ERROR_SUCCESS == MsiViewFetch(view, &record))
    {
        TCHAR outpath[MAX_PATH] = {0};
        TCHAR filename[MAX_PATH] = {0};
        BYTE buffer[4096] = {0};
        DWORD buf_size = 0;
        DWORD name_cch = _countof(filename); //cch stands for "Count of CHaracters"
        ret = MsiRecordGetString(record, MSI_FIELD_FILENAME, filename, &name_cch);
        if (ERROR_SUCCESS != ret)
        {
            _tprintf(_T("MsiRecordGetString(MSI_FIELD_FILENAME) failed, err(%d)\n"), ret);
            break;
        }
        _tprintf(_T("Found File : %s\n"), filename);

        PathCchCombine(outpath, _countof(outpath), dest_dir, filename);

        buf_size = sizeof(buffer);
        memset(buffer, 0, buf_size);
        while (ERROR_SUCCESS == MsiRecordReadStream(record, MSI_FIELD_BINARY_STREAM, (char*)buffer, &buf_size))
        {
            if (0 == buf_size)
                break;

            if(!AppendToFile(outpath, buffer, buf_size))
            {
                _tprintf(_T("AppendToFile() failed, stop extracting...\n"));
                break;
            }

            buf_size = sizeof(buffer);
            memset(buffer, 0, buf_size);
        }
    }

END:
    if (NULL != view)
    {
        MsiViewClose(view);
        view = NULL;
    }
    if (NULL != db)
    {
        MsiCloseHandle(db);
        db = NULL;
    }
}

int _tmain(int argc, TCHAR* argv[])
{
    TCHAR src_file[MAX_PATH] = { 0 };
    TCHAR dest_dir[MAX_PATH] = { 0 };

    if (argc != 3)
    {
        PrintUsage();
        return -1;
    }
    _tcscpy_s(src_file, MAX_PATH, argv[1]);
    _tcscpy_s(dest_dir, MAX_PATH, argv[2]);

    if(!IsFileExist(src_file))
    {
        _tprintf(_T("Specified MSI file(%s) not exist, abort...\n"), src_file);
        return -1;
    }
    if (!IsDirExist(dest_dir))
    {
        //create output dir
        if(!CreateDirectory(dest_dir, NULL))
        {
            _tprintf(_T("Create output dir failed(%s). abort...\n"), dest_dir);
            return -1;
        }
    }

    ExtractMsiFiles(src_file, dest_dir);
    return 0;
}
