#pragma once
#include <ws2tcpip.h>
#include <WinSock2.h>
#include <iostream>
#include <stdio.h>

#define DATA_BUFSIZE 2048

enum class IO_TYPE
{
	RECV,
	SEND
};

struct IO_DATA
{
	WSAOVERLAPPED overlapped;
	char buffer[DATA_BUFSIZE];
	WSABUF dataBuf;
	IO_TYPE ioType;
};