#include "server_defines.h"
#include "packet_handler.h"
#include "tcp_session.h"
#include "stream_buffer.h"

PacketHandler::PacketHandler()
{
}

PacketHandler::~PacketHandler()
{

}

bool PacketHandler::AddPacket(unsigned short packet_id, PACKET_FUNC callback)
{
	// 중복 id
	if (callback_list_.find(packet_id) != callback_list_.end())
		return false;

	callback_list_.insert({ packet_id , callback });

	return true;
}

bool PacketHandler::CallPacket(TcpSession* tcp_session)
{
	// 새션에서 버퍼 가져오기
	StreamBuffer* stream = tcp_session->GetStreamBuffer();

	// 버퍼 사이즈확인
	size_t buffer_size = stream->GetSize();
	while (buffer_size > PACKET_HEADER_SIZE)
	{
		// 헤더만 일부 가져오기 (스트림버퍼 사이즈 증감 없음)
		BasePacket header;
		header = stream->PeekBuffer<BasePacket>();

		// 헤더 크기 체크
		if (header.packet_len > buffer_size)
		{
			break;
		}

		// 등록된 패킷 찾기
		auto item = callback_list_.find(header.packet_id);

		// 없는 패킷
		if (item == callback_list_.end())
			return false;

		PACKET_FUNC cllback = item->second;

		// 패킷 데이터 복사 (스트림 버퍼 사이즈 감소)
		char buffer[DATA_BUFSIZE];
		stream->Dequeue(buffer, header.packet_len);

		BasePacket* packet = reinterpret_cast<BasePacket*>(buffer);

		// 등록된 함수 호출
		cllback(tcp_session, packet);

		// 버퍼 재확인
		buffer_size = stream->GetSize();
	}

	return true;
}