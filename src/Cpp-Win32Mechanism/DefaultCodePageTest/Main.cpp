// DefaultCodePageTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

int _tmain(int argc, TCHAR* argv[])
{
    char host[64] = {0};
    WSADATA wsa = {0};
    int ret = WSAStartup(MAKEWORD(2, 2), &wsa);
    if(0!=ret)
    {
        printf("WSAStartup() error(%d)\n", ret);
        return -1;
    }

//NOTE: If "System default codepage in non-unicode app" is set to UTF8 in control panel, 
//      gethostname() will return UTF8 string.
//I wonder if all OS API returns UTF8 string under this config.

    ret = gethostname(host, sizeof(host));
    if(0 != ret)
    {
        printf("WSAStartup() error(%d)\n", ret);
    }

    printf("got hostname = %s\n", host);
    printf("Hex of hostname=\n");
    for(int i=0; i<sizeof(host); i++)
    {
        if(i > 0 && (i % 16 == 0))
            printf("\n");
        if(0x00 == host[i])
            break;
        printf("%02X ", host[i]);
    }

    WSACleanup();
    return 0;
}
