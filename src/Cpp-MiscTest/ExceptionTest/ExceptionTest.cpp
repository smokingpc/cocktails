// ExceptionTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <winioctl.h>
#include <string>
#include <iostream>

void Test1()
{
    __try
    {
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
    }
}
void Test2()
{
    try
    {
    }
    catch (std::exception& ex)
    {
    }

}

void Test3()
{
    __try
    {
    }
    __finally
    {
    }
}

int _tmain(int argc, TCHAR* argv[])
{
    Test1();
    Test2();
}
