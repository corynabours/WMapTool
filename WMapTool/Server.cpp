#include "stdafx.h"
#include "Server.h"
#include "Application.h"
#include "Ws2tcpip.h"

Server* Server::_instance = 0; 
#define DEFAULT_PORT "51234"
#define DEFAULT_BUFLEN 512
Server::Server()
{
	Closing = false;
	WSADATA wsaData;
	struct addrinfo hints;
	struct addrinfo *localAddress = NULL;
	int result, iResult;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		Application *application = Application::Instance();
		MessageBox(application->hWnd, L"Could not start Personal Server: WinSock could not initialize.", L"Error", MB_ICONERROR);
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &localAddress);
	if (iResult != 0) {
		Application *application = Application::Instance();
		MessageBox(application->hWnd, L"Could not start Personal Server: Failure to get address.", L"Error", MB_ICONERROR);
		WSACleanup();
		return;
	}

	ListenSocket = socket(localAddress->ai_family, localAddress->ai_socktype, localAddress->ai_protocol);
	if (ListenSocket < 0)
	{
		Application *application = Application::Instance();
		MessageBox(application->hWnd, L"Could not start Personal Server", L"Error", MB_ICONERROR);
		WSACleanup();
		return;
	}

	//bind socket
	result = bind(ListenSocket, localAddress->ai_addr, localAddress->ai_addrlen);
	if (result < 0)
	{
		Application *application = Application::Instance();
		MessageBox(application->hWnd, L"Could not start Personal Server (error on socket bind)", L"Error", MB_ICONERROR);
		WSACleanup();
		return;
	}

	freeaddrinfo(localAddress);
	listen(ListenSocket, SOMAXCONN);
	HANDLE listenForNewConnectionThread = CreateThread(NULL, 0, listenForNewConnections, NULL, 0, NULL);
}

DWORD WINAPI Server::listenForNewConnections(LPVOID lpParam)
{
	Server *server = Server::PersonalServer();
	SOCKET ClientSocket;
	do 	
	{
		ClientSocket = accept(server->ListenSocket, NULL, NULL);
		HANDLE ClientThread = CreateThread(NULL, 0, listenForClientMessages, (LPVOID)ClientSocket, 0, NULL);
	} while (!server->Closing);
	return 0;
}

DWORD WINAPI Server::listenForClientMessages(LPVOID lpParam)
{
	int iResult;
	int iSendResult;
	SOCKET ClientSocket = (SOCKET)lpParam;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				//closesocket(ClientSocket);
				//WSACleanup();
				//return 1;
			}
		}
		else if (iResult == 0)
		{
		}
		else  {
			//closesocket(ClientSocket);
			//WSACleanup();
			//return 1;
		}

	} while (iResult > 0);
	return 0;
}

Server::~Server()
{
	closesocket(ListenSocket);
}

Server * Server::PersonalServer()
{
	if (_instance == 0)
	{
		_instance = new Server();
	}
	return _instance;
}
