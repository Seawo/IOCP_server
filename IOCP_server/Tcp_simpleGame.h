#pragma once

#include "Iocp_server.h"

struct BasePacket;

class Tcp_simpleGame : public IocpServer
{
public:
    std::string NickName = "";

public:
    Tcp_simpleGame();

    virtual void Init();
    virtual void ExitSession(TcpSession* exitSession);

    // 위치값 던지는거
    void Pos(TcpSession* session, BasePacket* packet);
    void GET_ID(TcpSession* session, BasePacket* packet);
    void Login(TcpSession* session, BasePacket* packet);
    void AniState(TcpSession* session, BasePacket* packet);
    void Test(TcpSession* session, BasePacket* packet);

    std::string SQLconnect(std::string id);
};

