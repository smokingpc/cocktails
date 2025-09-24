#include <tchar.h>
#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <iphlpapi.h>
#include <string>
#include <sstream>

#include "Structures.h"
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

using namespace std;

inline void GetLocalEPAddr(sockaddr_in& addr, PMIB_TCPROW_OWNER_PID entry)
{
	addr.sin_addr.S_un.S_addr = entry->dwLocalAddr;
	addr.sin_port= entry->dwLocalPort;
	addr.sin_family = AF_INET;
}
inline void GetRemoteEPAddr(sockaddr_in& addr, PMIB_TCPROW_OWNER_PID entry)
{
	addr.sin_addr.S_un.S_addr = entry->dwRemoteAddr;
	addr.sin_port = entry->dwRemotePort;
	addr.sin_family = AF_INET;
}
inline void GetLocalEPAddrStr(string& result, PMIB_TCPROW_OWNER_PID entry, ADDRESS_FAMILY af = AF_INET)
{
	stringstream ss;
	char ipbuf[INET6_ADDRSTRLEN+1] = {};
	if (AF_INET == af)
	{
		in_addr addr = { 0 };
		addr.S_un.S_addr = entry->dwLocalAddr;
		result = inet_ntop(af, &addr, ipbuf, sizeof(ipbuf));
	}
	else if(AF_INET6 == af)
	{
		_tprintf(_T("GetLocalEPAddrStr(IPv6) not implemented yet!!\n"));
	}

	ss << entry->dwLocalPort;
	result = result + std::string(":") + ss.str();
}
inline void GetRemoteEPAddrStr(string& result, PMIB_TCPROW_OWNER_PID entry, ADDRESS_FAMILY af = AF_INET) 
{
	stringstream ss;
	char ipbuf[INET6_ADDRSTRLEN + 1] = {};
	if (AF_INET == af)
	{
		in_addr addr = { 0 };
		addr.S_un.S_addr = entry->dwRemoteAddr;
		result = inet_ntop(af, &addr, ipbuf, sizeof(ipbuf));
	}
	else if (AF_INET6 == af)
	{
		_tprintf(_T("GetRemoteEPAddrStr(IPv6) not implemented yet!!\n"));
	}

	ss << entry->dwRemotePort;
	result = result + std::string(":") + ss.str();
}

DWORD IterateIPv4Connections(PMIB_TCPTABLE_OWNER_PID table)
{
	PMIB_TCPROW_OWNER_PID entry = nullptr;
	DWORD i = 0;
	for (i = 0; i < table->dwNumEntries; i++)
	{
		string src_ep = "";
		string dst_ep = "";
		entry = table->table + i;
		GetLocalEPAddrStr(src_ep, entry);
		GetRemoteEPAddrStr(dst_ep, entry);

		_tprintf(_T("[%d] PID= %d, %S->%S\n"),
			entry->dwState, entry->dwOwningPid, 
			src_ep.c_str(), dst_ep.c_str());
	}
	return i+1;
}

//return "total connections" count
DWORD ListConnectionInfo(ADDRESS_FAMILY af)
{
	DWORD size = 0;
	DWORD ret = 0;
	PVOID buffer = nullptr;
	PMIB_TCPTABLE_OWNER_PID table = nullptr;
	ret = GetExtendedTcpTable(buffer, &size, FALSE, af, TCP_TABLE_OWNER_PID_CONNECTIONS, 0);
	if (ERROR_INSUFFICIENT_BUFFER != ret)
	{
		_tprintf(_T("GetExtendedTcpTable() failed-1. err=%d\n"), ret);
		return 0;
	}

	size = sizeof(MIB_TCPTABLE_OWNER_PID);
	buffer = new UCHAR[size];
	if (nullptr == buffer)
	{
		_tprintf(_T("allocate buffer failed!\n"));
		return 0;
	}

	memset(buffer, 0, size);
	ret = GetExtendedTcpTable(buffer, &size, FALSE, af, TCP_TABLE_OWNER_PID_CONNECTIONS, 0);
	//if (ret != ERROR_SUCCESS)
	//{
	//	_tprintf(_T("GetExtendedTcpTable() failed-3. err=%d\n"), ret);
	//	return 0;
	//}
	table = (PMIB_TCPTABLE_OWNER_PID)buffer;
	size_t size1 = sizeof(MIB_TCPTABLE_OWNER_PID);
	size_t size2 = sizeof(_MIB_TCPROW_OWNER_PID);

	_tprintf(_T("Allocated Buffer (%d) bytes, entries(%d)\n"), size, table->dwNumEntries);
	if (AF_INET)
		return IterateIPv4Connections(table);
}

int _tmain(int argc, TCHAR* argv[]) 
{
	DWORD total = 0;
	for (int i = 0; i < 10000000; i++)
	{
		_tprintf(_T("[List IPv4 connections]\n"));
		total = ListConnectionInfo(AF_INET);
		_tprintf(_T("Total Connections=%d\n\n"), total);
	}
}

