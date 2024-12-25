// UniquePtr-Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <memory>
#include <string>
#include <windows.h>

int _tmain(int argc, TCHAR* argv[])
{
    char *buffer = new char[1024];
    {
        std::unique_ptr<char> ptr(buffer);
        char *cursor = ptr.get();
        RtlZeroMemory(cursor, 1024);
    }

    DWORD size_cch = 1024;
    TCHAR *buffer2 = new TCHAR[size_cch];
    std::basic_string<TCHAR> str;
    {
        std::unique_ptr<TCHAR> ptr(buffer2);
        RtlZeroMemory(ptr.get(), size_cch*sizeof(TCHAR));
        _tcscat_s(ptr.get(), size_cch, _T("test1234"));
        str = ptr.get();
    }

    return 0;
}
