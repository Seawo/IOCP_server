#pragma once
#include <Windows.h>
#include <vector>
#include <thread>
#include <map>

#define WORKER_COUNT 8

class TcpSession;
class PacketHandler;

class IocpServer
{
public:
	IocpServer();
	~IocpServer();

public:
	virtual void Init() {};
	virtual void ExitSession(TcpSession* exitSession) {};
	void Start(unsigned short port);

public:
	virtual TcpSession* CreateSession();

private:
	int  initSocket();
	void InitIocp();
	void worker();
	void Run();
	void CloseSession(TcpSession*);


protected:
	std::map<SOCKET, TcpSession*>	session_map_;
	PacketHandler* packek_handler_;

private:
	unsigned short				port_;
	SOCKET						listen_socket_;
	HANDLE						iocp_;
	std::vector<std::thread*>	workers_;
};