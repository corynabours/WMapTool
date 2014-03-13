#include "stdafx.h"
#include "ServerConnection.h"
#include "Ws2tcpip.h"
#include "ChatWindow.h"

#define DEFAULT_PORT "51234"
#define DEFAULT_BUFLEN 512
ServerConnection* ServerConnection::_instance = 0;

ServerConnection * ServerConnection::Instance()
{
	if (_instance == 0)
	{
		_instance = new ServerConnection();
		_instance->ConnectToPersonalServer();
	}
	return _instance;
}

void ServerConnection::ConnectToPersonalServer()
{
	struct addrinfo hints;
	struct addrinfo *result = NULL;
	struct addrinfo *ptr = NULL;
	int iResult;
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		return;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			return;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET) {
		return;
	}
	HANDLE ClientThread = CreateThread(NULL, 0, listenForServerMessages, (LPVOID)ConnectSocket, 0, NULL);
}

DWORD WINAPI ServerConnection::listenForServerMessages(LPVOID lpParam)
{
	SOCKET ConnectSocket = (SOCKET)lpParam;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult;
	size_t charsConverted = 0;
	ChatWindow *chatWindow = ChatWindow::Instance();
	// Receive until the peer closes the connection
	do {
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			wchar_t * buffer = new wchar_t[iResult];
			mbstowcs_s(&charsConverted,buffer,iResult, recvbuf, iResult);
			chatWindow->DisplayMessage(buffer);
			delete buffer;
		}
		/*else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());*/

	} while (iResult > 0);

	// cleanup
	closesocket(ConnectSocket);
	return 0;
}

void ServerConnection::Send(LPWSTR message)
{
	size_t charsConverted = 0;
	if (ConnectSocket == INVALID_SOCKET) return;
	int length = lstrlenW(message)*sizeof(wchar_t)+1;
	char * buffer = new char[length];
	wcstombs_s(&charsConverted,buffer,length,message,length-1);
	int iResult = send(ConnectSocket, buffer, length, 0);
	delete buffer;
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		return;
	}
}

ServerConnection::ServerConnection()
{
	ConnectSocket = INVALID_SOCKET;
}


ServerConnection::~ServerConnection()
{
}
