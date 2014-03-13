#pragma once
#include <winsock2.h>
class ServerConnection
{
public:
	static ServerConnection* Instance();
	void Send(LPWSTR message);
private:
	static ServerConnection* _instance;
	void ConnectToPersonalServer();
	SOCKET ConnectSocket;
	static DWORD WINAPI listenForServerMessages(LPVOID lpParam);
protected:
	ServerConnection();
	~ServerConnection();
};

