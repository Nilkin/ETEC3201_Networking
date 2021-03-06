// UDP_Server.cpp : Defines the entry point for the console application.
//
//Thomas Gilman
//Networking
//ETEC 3201
//1
//Lab 9 UDP DGRAMS
//SERVER!!!!!!!!!!!!!!!!!!
#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include <conio.h>

#pragma comment(lib, "ws2_32.lib") //include lib

#define BUFF_SIZE 150
#define SERVER_PORT 5020

char stringCheckBuff[9];
int numUsers = 0;

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

void setAddress(struct sockaddr_in &address, ADDRESS_FAMILY family, unsigned short port, PCWSTR netAddress)
{
	memset(&address, 0, sizeof(address));					//fill padding with 0's
	address.sin_family = family;							//address family
	address.sin_port = htons(port);							//htons is host to network short port
	InetPton(AF_INET, netAddress, &address.sin_addr.s_addr);//puts into bytes destination IP

	if (address.sin_addr.s_addr < 0)
	{
		printf("Failed to pass netAddress!\n");
		wait();
		exit(-1);
	}
}


typedef struct CHAT_USER
{
	unsigned char user_active;				//0 if inactive, 1 if active.
	char user_name[16];
	struct sockaddr_in user_address;		//comes from recvfrom
	unsigned short user_port;				//comes from recvfrom
};

int main(int argc, char **argv)
{
	WSADATA wsa; //windows specific
	SOCKET sock_fd;
	PCWSTR IPaddress = L"127.0.0.1"; //my IPv4
	struct sockaddr_in serverAddress;
	struct sockaddr_in addressIn;
	socklen_t addrlen = sizeof(addressIn);

	struct CHAT_USER *active_user_array; //up to 16 active users.

	const char *message;
	char recv_buffer[BUFF_SIZE];
	int recv_size;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d\n", WSAGetLastError());
		wait();
		WSACleanup();
		return -1;
	}printf("Winsock Initialised!\n");
	if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Error: Could not create socket. Code=%d\n", WSAGetLastError());
		wait();
		closesocket(sock_fd);
		WSACleanup();
		return -1;
	}printf("Socket created!\n");
	unsigned long mode = 1;
	if (ioctlsocket(sock_fd, FIONBIO, &mode) != 0)
	{
		printf("failed to set socket to nonBlocking. Code=%d\n", WSAGetLastError());
		wait();
		closesocket(sock_fd);
		WSACleanup();
		return -1;
	}
	//setUp Server addr and bind it
	setAddress(serverAddress, AF_INET, SERVER_PORT, INADDR_ANY);
	if (bind(sock_fd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
	{
		printf("Error binding the socket. ERROR CODE:%d\n", WSAGetLastError());
		wait();
		return -1;
	}

	//server Sending message stuff
	char serverMessage[128], serverMessageToSend[137];
	memset(serverMessage, 0, sizeof(serverMessage));
	memset(serverMessageToSend, 0, sizeof(serverMessageToSend));
	const char *serverName = "Server:";
	int charindex = 0;
	int inServer = 1;

	//sending loop
	printf("waiting on port %d\n", SERVER_PORT);
	while (inServer)
	{
		if (_kbhit())
		{
			char charin = _getch();
			
			if (((int)charin > 47 && (int)charin < 92) || ((int)charin > 92 && (int)charin < 127)		//add character except special chars to message buffer
				|| ((int)charin > 47 && (int)charin < 58) || ((int)charin > 31 && (int)charin < 37)
				|| ((int)charin > 37 && (int)charin < 47))
			{
				serverMessage[charindex++] = charin;
				printf("%c", charin);
			}
			else if ((int)charin == 27)
			{
				for (int userListIndex = 0; userListIndex < numUsers; userListIndex++)
				{
					while (sendto(sock_fd, "Server:Server has terminated.", 30, 0,
						(struct sockaddr*)&active_user_array[userListIndex].user_address,
						sizeof(active_user_array[userListIndex].user_address)) < 0)
					{;}
					if (numUsers > 0)
						free(active_user_array);
					inServer = 0;
				}

			}
			else if (((int)charin == 13) || charindex == 127)	//Carriage feed
			{
				printf("\n");
				serverMessage[charindex + 1] = '\0';												//null terminate message

				strcat_s(serverMessageToSend, sizeof(serverMessageToSend), serverName);				//create server message to send
				strcat_s(serverMessageToSend, sizeof(serverMessageToSend), serverMessage);
				serverMessageToSend[charindex + 9] = '\0';											//null terminate server message to send
				charindex = 0;																		//reset message char insert index

				for (int userListIndex = 0; userListIndex < numUsers; userListIndex++)
				{
					if (strcmp(serverMessage, "EXIT") == 0)												//server is exiting
					{
						if (sendto(sock_fd, serverMessageToSend, strlen(serverMessageToSend), 0,
							(struct sockaddr*)&active_user_array[userListIndex].user_address,
							sizeof(active_user_array[userListIndex].user_address)) < 0)
						{
							printf("could not send EXIT message. ERROR CODE:%d\n", WSAGetLastError());
						}
						if (numUsers > 0)
							free(active_user_array);
						inServer = 0;
					}
					if (sendto(sock_fd, serverMessageToSend, strlen(serverMessageToSend), 0,			//send message
						(struct sockaddr*)&active_user_array[userListIndex].user_address,
						sizeof(active_user_array[userListIndex].user_address)) < 0)
					{
						printf("could not send message. ERROR CODE:%d\n", WSAGetLastError());
					}
					printf("%s\n", serverMessageToSend);
				}
				
				memset(serverMessage, 0, sizeof(serverMessage));							//clear array for next message
				memset(serverMessageToSend, 0, sizeof(serverMessageToSend));
			}
		}
		if ((recv_size = recvfrom(sock_fd, recv_buffer, BUFF_SIZE, 0, (struct sockaddr *)&addressIn, &addrlen)) > 0)	//Recieveing a message
		{
			printf("message recieved:%s\n", recv_buffer);

			//message In Variables
			char userNameRec[16];
			char messageToCheck[128];
			int messIndex = 0, recvBuffIndex = 0;
			memset(userNameRec, 0, sizeof(userNameRec));
			memset(messageToCheck, 0, sizeof(messageToCheck));

			//SPLIT MESSAGE AND USERNAME FOR CHECKING
			while (recv_buffer[recvBuffIndex] != ':')													//get username
			{
				//printf("%c",recv_buffer[userNameIndex]);
				userNameRec[recvBuffIndex] = recv_buffer[recvBuffIndex++];
			}
			//printf("\n");
			userNameRec[recvBuffIndex] = '\0';															//null terminate username Comparison

			while (recv_buffer[recvBuffIndex] != '\0' || recvBuffIndex == BUFF_SIZE - 1)				//get message
			{
				messageToCheck[messIndex++] = recv_buffer[recvBuffIndex++];
			}																	
			messageToCheck[messIndex] = '\0';															//null terminate message comparison

			//CHECK USERS IN SERVER TO SEND MESSAGES TO OR HAVE EXIT
			if (numUsers > 0)	//check Users in chatroom to send message to
			{
				int userInChatRoom = 0;
				for (int userIndex = 0; userIndex < numUsers; userIndex++)
				{
					char *userName = active_user_array[userIndex].user_name;
					if (strcmp(userNameRec, userName) == 0)						//sender is an active user
					{
						userInChatRoom = 1;
						if (strcmp(messageToCheck, ":EXIT") == 0)												//user wants to leave
						{
							for (int index = userIndex; index < numUsers - 1; index++)							//write over client wanting to leave
								active_user_array[index] = active_user_array[index + 1];						//then resize array for new number of users in chat
							active_user_array = (CHAT_USER*)realloc(active_user_array, (--numUsers) * sizeof(CHAT_USER));
							printf(userNameRec); printf("left the chat room\n"); printf("Users left:%d", numUsers);
							break;
						}
					}
					else					//send message to other users
					{
						if (sendto(sock_fd, recv_buffer, strlen(recv_buffer), 0,								//send the message
							(struct sockaddr*)&(active_user_array[userIndex].user_address),						//tell server if could not send for some reason
							sizeof(active_user_array[userIndex].user_address)) < 0)
						{
							printf("could not send message. ERROR CODE:%d\n", WSAGetLastError());
						}
						printf("routed message\n");
					}
				}
				if (userInChatRoom = 0)					//user is not in chat room
				{
					if (strcmp(messageToCheck, ":JOIN") == 0 )	//add user if wanting to join
					{
						CHAT_USER newUser;
						newUser.user_active = 1; newUser.user_address = addressIn;
						strcpy_s(newUser.user_name, sizeof(newUser.user_name), userNameRec);	
						newUser.user_port = addressIn.sin_port;

						active_user_array = (CHAT_USER*)realloc(active_user_array, (++numUsers) * sizeof(CHAT_USER));
						active_user_array[numUsers - 1] = newUser;
						printf("NumUsers:%d User:%s entered the chat room\n", numUsers, newUser.user_name);
					}
				}
			}
			else
			{
				if (strcmp(messageToCheck, "JOIN"))	//no users, add a user if someone is wanting to join
				{
					CHAT_USER newUser;
					newUser.user_active = 1; newUser.user_address = addressIn;
					strcpy_s(newUser.user_name, sizeof(newUser.user_name),userNameRec);	
					newUser.user_port = addressIn.sin_port;
					numUsers++;
					active_user_array = (CHAT_USER*)malloc(sizeof(CHAT_USER));
					active_user_array[numUsers - 1] = newUser;
					printf("NumUsers:%d User:%s entered the chat room\n address:%lu\n userPort:%d\n", numUsers, newUser.user_name, newUser.user_address.sin_addr.S_un, newUser.user_port);
				}
			}
		}
		memset(recv_buffer, 0, sizeof(recv_buffer));
	}

	closesocket(sock_fd); //windows specific
						  //shutdown(sock_fd, 2); //UNIX specific: dont use sock_fd, for both reading/writing
	WSACleanup();
	printf("Sockets closed and winsock cleaned up.\n");

	return 0;
}
