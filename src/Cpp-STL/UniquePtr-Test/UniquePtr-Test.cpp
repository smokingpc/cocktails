// UniquePtr-Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <memory>
#include <windows.h>

int _tmain(int argc, TCHAR* argv[])
{
    char *buffer = new char[1024];
    {
        std::unique_ptr<char> ptr(buffer);
        char *cursor = ptr.get();
        RtlZeroMemory(cursor, 1024);
    }

    return 0;
}
