// NetworkingSockets.cpp : Defines the entry point for the console application.
//
//Thomas Gilman
//Networking 3201.01
//3rd May 2018
//FINAL problem 16

#include "stdafx.h"
#include <stdio.h>
#include <WinSock2.h>
#include "Ws2tcpip.h"
#include <time.h>

#define BUFF_SIZE 5000
#define SERVER_PORT 5020

char stringCheckBuff[9];
int clientOrServer = 0;

using namespace std;

void setAddress(struct sockaddr_in &address, ADDRESS_FAMILY family, unsigned short port, PCWSTR netAddress)
{
	address.sin_family = family;							//address family
	address.sin_port = htons(port);							//htons is host to network short port
	InetPton(AF_INET, netAddress, &address.sin_addr.s_addr);//puts into bytes destination IP
	memset(address.sin_zero, 0, sizeof(address.sin_addr));	//fill padding with 0's

	if (address.sin_addr.s_addr < 0)
	{
		printf("Failed to pass netAddress!\n");
		exit(-1);
	}
}
//wait loop for reading window shell
void wait()
{
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
	SOCKET sock_fd;
	PCWSTR IPaddress = L"127.0.0.1"; //my IPv4
	srand(time(0));

	char message[65] = { 0 };
	char recv_buffer[BUFF_SIZE];
	int recv_size;
	const char* getMessage = "GET / HTTP/1.1\r\n\r\n\0";
	const char* exitMessage = "quite";

	while (1)
	{
		printf("please specify client or server\n");
		fgets(stringCheckBuff, sizeof(stringCheckBuff), stdin);
		if (strcmp(stringCheckBuff, "client\n") == 0)
		{
			clientOrServer = 1;
			printf("specified client use, clientOrServer:%d\n",clientOrServer);
			break;
		}
		else if (strcmp(stringCheckBuff, "server\n") == 0)
		{
			clientOrServer = 2;
			printf("specified server use, clientOrServer:%d\n",clientOrServer);
			break;
		}
	}

	printf("clientOrServer : %d\n", clientOrServer);


	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) //windows specific
	{
		printf("Failed. Error Code : %d\n", WSAGetLastError());
		wait();
		return -1;
	}printf("Winsock Initialised!\n");
	

	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Error: Could not create socket. Code=%d\n", WSAGetLastError());
		wait();
		return -1;
	}printf("Socket created!\n");

	//specify server use or client use
	switch (clientOrServer)
	{
	//CLIENT CODE v
	//connect
	case 1:																				//CLIENT CASE HERE
		struct sockaddr_in serverAddress;

		//setup server struct
		//IPaddress = L"216.58.214.78"; //googles IP

		setAddress(serverAddress, AF_INET, SERVER_PORT, IPaddress);
		//Connect to remote server
		if (connect(sock_fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
		{
			printf("Error: couldn't connect. Code=%d\n", WSAGetLastError());
			wait();
			return -1;
		}printf("Connected\n");
		
		
		printf("please specify GET or exit\n");
		memset(stringCheckBuff, 0, sizeof(stringCheckBuff));
		fgets(stringCheckBuff, sizeof(stringCheckBuff), stdin);
		if (strcmp(stringCheckBuff, "GET\n") == 0)
		{
			strcat_s(message, sizeof(message), getMessage);
			break;
		}
		else if (strcmp(stringCheckBuff, "quite\n") == 0)
		{
			strcat_s(message, sizeof(message), exitMessage);
			break;
		}
		//Send request to server
		printf("sending message: %s\n", message);
		if (send(sock_fd, message, strlen(message)+1, 0) < 0)
		{
			printf("Error: send failed. Code=%d\n", WSAGetLastError());
			wait();
			return -1;
		}printf("Data Sent\n");

		//Receive a reply from the server
		if ((recv_size = recv(sock_fd, recv_buffer, sizeof(recv_buffer), 0)) == SOCKET_ERROR)
		{
			printf("Error: recv failed. Code=%d\n", WSAGetLastError());
			wait();
			return -1;
		}printf("Reply recieved. Bytes received = %d\n", recv_size);

		//add NULL terminator to end to make proper string before printing
		recv_buffer[recv_size-1] = '\0';
		printf(recv_buffer);
		break;																		//END CLIENT CASE HERE

	//Server Code v
	//reciever
	case 2:																			//SERVER CASE HERE
		int serverLoop = 1;
		SOCKET new_fd;
		struct sockaddr_in myAddress, theirAddress;
		int their_addr_size;
		setAddress(myAddress, AF_INET, SERVER_PORT, IPaddress); //0 sets addressing to default

		if (bind(sock_fd, (struct sockaddr*)&myAddress, sizeof(struct sockaddr)) == -1)
		{
			printf("Error: Could not bind. Code=%d\n", WSAGetLastError());
			wait();
			return -1;
		}printf("Socket bound.\n");
		printf("now looping\n");
		do
		{
			//listen with a back-log of 10 pending connections
			if (listen(sock_fd, 10) == -1)
			{
				printf("Error: Could not listen. Code=%d\n", WSAGetLastError());
				wait();
				return -1;
			}printf("Socket listening.\n");

			//accept connections
			their_addr_size = sizeof(theirAddress);
			new_fd = accept(sock_fd, (struct sockaddr*)&theirAddress, &their_addr_size);
			printf("Connection accepted.\n");

			//Receive request from client
			if ((recv_size = recv(new_fd, recv_buffer, BUFF_SIZE, 0)) == SOCKET_ERROR)
			{
				printf("Error: recv failed. Code=%d\n", WSAGetLastError());
				wait();
				return -1;
			}printf("Reply received. Bytes received = %d\n", recv_size);
			printf("Request Received: %s\n", recv_buffer);

			char response[200] = { 0 };
			if (strcmp("quit\n", recv_buffer) == 0) //user wants to exit
			{
				serverLoop = 0;
				strcat_s(response, sizeof(response), "HTTP/1.0 200 Ok\r\n\r\n<html><body><c><H1>You have quite the server!</H1></c></body></html>\0");
			}
			else
			{
				int randomResponse = rand() % 7;
				const char *eightBallMessage;
				switch (randomResponse)
				{
				case 0:
					eightBallMessage = "you may or may not fail!";
					break;
				case 1:
					eightBallMessage = "you shall pass this exam!";
					break;
				case 2:
					eightBallMessage = "you will suffer for all of eternity!";
					break;
				case 3:
					eightBallMessage = "you are very fortunate!";
					break;
				case 4:
					eightBallMessage = "congrats, you have gotten a message!";
					break;
				case 5:
					eightBallMessage = "success is earned through hard work and constant failure!";
					break;
				case 6:
					eightBallMessage = "this may or may not be a fortune!";
					break;
				case 7:
					eightBallMessage = "this is message eight congrats on your luck...";
					break;
				}
				const char* TopHTML = "HTTP/1.0 200 Ok\r\n\r\n<html><body><c><H1>";
				const char* BotHTML = "</H1></c></body></html>\0";
				strcat_s(response, sizeof(response), TopHTML);
				strcat_s(response, sizeof(response), eightBallMessage);
				strcat_s(response, sizeof(response), BotHTML);
			}
			//send response to client
			if (send(new_fd, response, sizeof(response), 0) < 0)
			{
				printf("Error: send failed. Code=%d\n", WSAGetLastError());
				wait();
				return -1;
			}printf("Response Sent\n");
			printf("Response Sent: %s\n", response);
		} while (serverLoop);
		printf("closking sockets and cleaning up\n");
		closesocket(new_fd);
		break;																		//END SERVER CASE HERE
	}
	wait();
	closesocket(sock_fd); //windows specific
	//shutdown(sock_fd, 2); //UNIX specific: dont use sock_fd, for both reading/writing
	WSACleanup();
	printf("Sockets closed and winsock cleaned up.\n");
    return 0;
}