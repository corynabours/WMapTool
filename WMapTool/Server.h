#pragma once
#include <winsock2.h>
class Server
{
private:
	static Server* _instance;
	SOCKET ListenSocket;
	static DWORD WINAPI listenForNewConnections(LPVOID lpParam);
	static DWORD WINAPI listenForClientMessages(LPVOID lpParam);
protected:
	Server();
	~Server();
	bool Closing;
public:
	static Server* PersonalServer();
};

