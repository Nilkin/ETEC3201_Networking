//Thomas Gilman
//Networking 3201.01
//17th April, 2018
//Lab 8 - TCP/IP: GetHostByName

#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)

//wait loop for reading window shell
void wait()
{
	char stringCheckBuff[9];
	while (1)
	{
		printf("waiting, type continue to continue\n");
		fgets(stringCheckBuff, sizeof(stringCheckBuff), stdin);
		if (strcmp(stringCheckBuff, "continue\n"))
			break;
	}
}

int main(int argc, char **argv)
{
	WSADATA wsa; //windows specific
	struct hostent* addressReq;
	char *host_name, **pAlias;
	struct in_addr addr;
	int index = 0;

	//Setup Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d\n", WSAGetLastError());
		wait();
		return -1;
	}printf("Winsock Initialised!\n");

	host_name = argv[2];
	printf("arg passed: %s\n",host_name);
	addressReq = gethostbyname(host_name);

	if (addressReq == NULL) {
		printf("gethostbyname() failed\n");
		wait();
		return -1;
	}
	else {
		printf("Official Host Name : %s\n", addressReq->h_name);
		for (pAlias = addressReq->h_aliases; *pAlias != 0; pAlias++)
		{
			printf("Alternate Name #%d: %s\n", index, *pAlias); //print out alternate names
		}
		if (addressReq->h_addrtype == AF_INET)
		{
			index = 0;
			while (addressReq->h_addr_list[index] != 0) {
				addr.s_addr = *(u_long*)addressReq->h_addr_list[index++];
				printf("IP Address #%d: %s\n", index, inet_ntoa(addr));
			}
		}
		else
			printf("IP family %d returned\n", addressReq->h_addrtype);
	}
	wait();

	printf("Sockets closed and winsock cleaned up.\n");

	return 0;
}