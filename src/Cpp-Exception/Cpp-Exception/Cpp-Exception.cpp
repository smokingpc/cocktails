// Cpp-Exception.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <exception>
#include <tchar.h>
#include <windows.h>

int _tmain(int argc, TCHAR* argv[])
{
    throw new std::exception("Test Exception");
}
