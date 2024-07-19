// MulticastTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//disable deprecated API warning on windows platform
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2ipdef.h>

#pragma comment(lib, "ws2_32.lib")

#define MOCK_DATA       "I am the swarm!"
#define TEMPLATE_SEND_DATA "[from %d.%d.%d.%d] %s\n"
#define MY_MCAST_TTL    8
#define BUFFER_SIZE     256
#define MAX_RECV_COUNT  3       //max receive packets from multicast

enum CMD_ACT {
    UNKNOWN,
    SEND,
    RECV
};

struct CMD_OPTION {
    CMD_ACT Cmd;
    struct ip_mreq ReqAddr;
    unsigned short MulticastPort;
};


bool JoinMulticastGroup(SOCKET sock, struct ip_mreq &addr)
{
    int rc = setsockopt(sock, 
                IPPROTO_IP, IP_ADD_MEMBERSHIP,
                (char*)&addr, sizeof(addr));

    if(0 != rc)
    {
        //printf("join multicast group failed, WSALastError()=%d\n", WSAGetLastError());
        printf("join multicast group failed\n");
        //windows debug
        printf("WSAGetLast()=%d\n", WSAGetLastError());
    }

    return (0 == rc);
}
bool SetOutIfForMulticast(SOCKET sock, struct ip_mreq& addr)
{
    int rc = setsockopt(sock,
        IPPROTO_IP, IP_MULTICAST_IF,
        (char*)&addr.imr_interface.S_un.S_addr, 
        sizeof(addr.imr_interface.S_un.S_addr));

    if (0 != rc)
    {
        //printf("set outgoing interface of multicast failed, WSALastError()=%d\n", WSAGetLastError());
        printf("set outgoing interface of multicast failed\n");
        //windows debug
        printf("WSAGetLast()=%d\n", WSAGetLastError());
    }

    return (0 == rc);
}
bool SetMulticastTTL(SOCKET sock, int ttl)
{
    int rc = setsockopt(sock,
        IPPROTO_IP, IP_MULTICAST_TTL,
        (char*)&ttl,
        sizeof(ttl));

    if (0 != rc)
    {
        //printf("set multicast ttl failed, WSALastError()=%d\n", WSAGetLastError());
        printf("set multicast ttl failed\n");
        //windows debug
        printf("WSAGetLast()=%d\n", WSAGetLastError());
    }

    return (0 == rc);
}
bool PrepareMulticastSocket(SOCKET sock, struct ip_mreq& addr)
{
    //SetMulticastTTL() is optional... skipping it is ok.
    return JoinMulticastGroup(sock, addr) &&
            SetOutIfForMulticast(sock, addr) &&
            SetMulticastTTL(sock, MY_MCAST_TTL);
}

void DoSendToMulticast(struct ip_mreq& addr, unsigned short port)
{
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    char buffer[BUFFER_SIZE] = {0};
    int rc = 0, send_size = 0;

    if(INVALID_SOCKET == sock)
    {
        printf("create socket failed.\n");
        return;
    }

    if(!PrepareMulticastSocket(sock, addr))
        goto END;

    if(0 != rc)
    {
        printf("connect to multicast addr failed.\n");
        //windows debug
        printf("WSAGetLast()=%d\n", WSAGetLastError());
        goto END;
    }

    struct sockaddr_in to_addr;
    to_addr.sin_family = AF_INET;
    to_addr.sin_port = port;
    to_addr.sin_addr.S_un.S_addr = addr.imr_multiaddr.S_un.S_addr;

    send_size = snprintf(buffer, BUFFER_SIZE, TEMPLATE_SEND_DATA,
        to_addr.sin_addr.S_un.S_un_b.s_b1, 
        to_addr.sin_addr.S_un.S_un_b.s_b2, 
        to_addr.sin_addr.S_un.S_un_b.s_b3, 
        to_addr.sin_addr.S_un.S_un_b.s_b4,
        MOCK_DATA);
    rc = sendto(sock, buffer, send_size, 0, (SOCKADDR*)&to_addr, sizeof(to_addr));
    if (0 > rc)
        printf("send data to multicast addr failed.\n");

END:
    closesocket(sock);
}
void DoRecvFromMulticast(struct ip_mreq& addr, unsigned short port)
{
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    int rc = 0, recv_size = 0, recv_count = 0;

    if (INVALID_SOCKET == sock)
    {
        printf("create socket failed.\n");
        return;
    }
    struct sockaddr_in bind_addr;
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = port;
    bind_addr.sin_addr.S_un.S_addr = addr.imr_interface.S_un.S_addr;

    struct sockaddr_in from_addr;
    from_addr.sin_family = AF_INET;
    from_addr.sin_port = port;
    from_addr.sin_addr.S_un.S_addr = addr.imr_multiaddr.S_un.S_addr;
    int from_size = sizeof(from_addr);

    rc = bind(sock, (SOCKADDR*)&bind_addr, sizeof(bind_addr));
    if (0 != rc)
    {
        printf("bind to multicast addr failed.\n");
        //windows debug
        printf("WSAGetLast()=%d\n", WSAGetLastError());
        goto END;
    }

    if (!PrepareMulticastSocket(sock, addr))
        goto END;

    while(recv_count++ < MAX_RECV_COUNT)
    {
        char buffer[BUFFER_SIZE] = { 0 };
        //rc = recv(sock, buffer, BUFFER_SIZE, 0);
        rc = recvfrom(sock, buffer, BUFFER_SIZE, 0, 
                (sockaddr*)&from_addr, &from_size);
        if(rc > 0)
        {
            printf("Got Message: %s\n", buffer);
        }
        else if(rc < 0)
        {
            printf("recv() failed. \n");
            break;
        }
        else
        {
            //connection closed, do nothing...
        }
    }

END:
    closesocket(sock);
}

int PrintUsage()
{
    printf("Usage: MulticastTest.exe -send -recv [Multicast ip]:[port] [local interface ip]\n");
    printf("       -send    act as sender, send string to specified multicast address and port.\n");
    printf("       -recv    act as receiver, recv three strings from specified multicast address and port.\n");
    printf("Example: (assuming local interface nic is 10.0.7.22)\n");
    printf("1. If want to send data to 224.0.10.1:55688, execute \"MulticastTest.exe -send 224.0.10.1 55688 10.0.7.22\n\"");
    printf("   It will send one string then exit.\n");
    printf("2. If want to recv data from 224.0.10.2:1234, execute \"MulticastTest.exe -recv 224.0.10.2 1234 10.0.7.22\n\"");
    printf("   It will recv three strings and print it to console.\n");
    printf("Note: Local interface ip is necessary to assign\"which nic-if want to join multicast\".\n");
    printf("      It is necessary even there is only 1 NIC on this computer(loopback addr is not valid).\n");
    return -1;
}
bool ParseCmdArgs(struct CMD_OPTION *option, int argc, char* argv[])
{
    memset(option, 0, sizeof(CMD_OPTION));
    if(argv[1][0] == '-' || argv[1][0] == '/')
    {
        char *str = &argv[1][1];
        if(0==_stricmp(str, "send"))
            option->Cmd = CMD_ACT::SEND;
        else if (0 == _stricmp(str, "recv"))
            option->Cmd = CMD_ACT::RECV;
        else
            option->Cmd = CMD_ACT::UNKNOWN;

        option->ReqAddr.imr_multiaddr.S_un.S_addr = inet_addr(argv[2]);
        option->ReqAddr.imr_interface.S_un.S_addr = inet_addr(argv[4]);
        option->MulticastPort = htons((u_short)atoi(argv[3]));
    }

    return (CMD_ACT::UNKNOWN != option->Cmd) &&
           (0 != option->ReqAddr.imr_multiaddr.S_un.S_addr) &&
            (0 != option->ReqAddr.imr_interface.S_un.S_addr) &&
            (0 != option->MulticastPort);
}
int main(int argc, char* argv[])
{
    CMD_OPTION option;
    if(!ParseCmdArgs(&option, argc, argv))
    {
        return PrintUsage();
    }

    //debug info print
    printf("option parsed:\n");
    printf("    action=%d\n", (int)option.Cmd);
    printf("    multicast ip=%d.%d.%d.%d\n", 
        option.ReqAddr.imr_multiaddr.S_un.S_un_b.s_b1,
        option.ReqAddr.imr_multiaddr.S_un.S_un_b.s_b2,
        option.ReqAddr.imr_multiaddr.S_un.S_un_b.s_b3,
        option.ReqAddr.imr_multiaddr.S_un.S_un_b.s_b4);
    printf("    multicast port=%d\n", ntohs(option.MulticastPort));
    printf("    local interface ip=%d.%d.%d.%d\n",
        option.ReqAddr.imr_interface.S_un.S_un_b.s_b1,
        option.ReqAddr.imr_interface.S_un.S_un_b.s_b2,
        option.ReqAddr.imr_interface.S_un.S_un_b.s_b3,
        option.ReqAddr.imr_interface.S_un.S_un_b.s_b4);

    //Startup Winsock2 on windows platform.
    //Every process only have to call WSAStartup() once.
    //multiple call is not necessary.
    WSADATA wsa_data = { 0 };
    errno_t wsa_err;
    WORD wsa_ver = MAKEWORD(2, 2);
    wsa_err = WSAStartup(wsa_ver, &wsa_data);

    if(0 != wsa_err)
        return -2;

    //do command...
    switch (option.Cmd)
    {
    case CMD_ACT::SEND:
        DoSendToMulticast(option.ReqAddr, option.MulticastPort);
        break;
    case CMD_ACT::RECV:
        DoRecvFromMulticast(option.ReqAddr, option.MulticastPort);
        break;
    }

    //Shutdown Winsock2 on windows platform.
    //Every process only have to call it once, too.
    WSACleanup();
    return 0;
}
