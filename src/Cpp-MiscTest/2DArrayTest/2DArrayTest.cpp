// 2DArrayTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int _tmain(int argc, TCHAR* argv[]) 
{
    char data[2][128] = {"12345678", "abcdefgh"};
    char *ptr[2] = {data[0], data[1]};
    printf("data[0] = %s\n", data[0]);
    printf("data[1] = %s\n", data[1]);
    printf("ptr[0] = %s\n", ptr[0]);
    printf("ptr[1] = %s\n", ptr[1]);
}
