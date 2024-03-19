#pragma once
#include <functional>
#include <unordered_map>

class TcpSession;

#pragma pack(push, 1)
struct BasePacket
{
	unsigned short packet_len;
	unsigned short packet_id;
};
#pragma pack(pop)

#define PACKET_HEADER_SIZE sizeof(BasePacket)
#define REGIST_PACKET(h,i,f) h->AddPacket(i,std::bind(&f, this, std::placeholders::_1, std::placeholders::_2))

typedef std::function<void(TcpSession*, BasePacket*)> PACKET_FUNC;

class PacketHandler
{
public:
	PacketHandler();
	~PacketHandler();

public:
	bool AddPacket(unsigned short packet_id, PACKET_FUNC callback);
	bool CallPacket(TcpSession* tcp_session);

private:
	std::unordered_map<unsigned short, PACKET_FUNC> callback_list_;
};

