#include "server_defines.h"
#include "Iocp_server.h"
#include "packet_handler.h"
#include "tcp_session.h"

#pragma comment(lib, "Ws2_32.lib")

IocpServer::IocpServer()
	:iocp_(nullptr),
	listen_socket_(0)

{
	packek_handler_ = new PacketHandler;
}

IocpServer::~IocpServer()
{
	delete packek_handler_;
}

TcpSession* IocpServer::CreateSession()
{
	return new TcpSession;
}

void IocpServer::worker() {
	srand((unsigned int)time(nullptr));

	DWORD buffer_len;
	TcpSession* session;
	OVERLAPPED* overlapped;
	std::thread::id worker_number = std::this_thread::get_id();
	while (true) {
		// GetQueuedCompletionStatus 함수에서 멈춰있다가 이벤트가 발생되면 return함
		BOOL success = GetQueuedCompletionStatus(iocp_, &buffer_len, (PULONG_PTR)&session, &overlapped, INFINITE);
		IO_DATA* ioData = (IO_DATA*)overlapped;

		if (buffer_len == 0) // 강제 종료던 뭐던 상대가 종료 되었을때
		{
			ExitSession(session); // Session 소멸자 같은 역할
			CloseSession(session);
			continue;
		}

		if (ioData->ioType == IO_TYPE::RECV)
		{
			// recv 완료 처리
			size_t enq_size = session->Recv(buffer_len);

			// 버퍼가 풀난상황
			if (enq_size != buffer_len)
			{
				CloseSession(session);
				continue;
			}

			// 패킷 처리
			bool invalid = packek_handler_->CallPacket(session);
			if (invalid == false)
			{
				CloseSession(session);
				continue;
			}

			// 수신 재호출
			session->CallRecv();
		}
		else
		{
			session->Send();
		}
	}
}

void IocpServer::Start(unsigned short port)
{
	std::cout << "서버 시작" << std::endl;

	Init();

	port_ = port;
	initSocket();
	InitIocp();
	Run();
}

void IocpServer::Run()
{
	while (true)
	{
		SOCKET client_socket = accept(listen_socket_, nullptr, nullptr); //유저 대기

		if (client_socket == INVALID_SOCKET)
		{
			closesocket(listen_socket_);
			WSACleanup();
			return;
		}

		// 클라이언트 생성
		TcpSession* session = CreateSession();
		session->Init(client_socket);

		// 관리자에 추가
		session_map_.insert({ client_socket, session });

		// 클라이언트 소켓에 IOCP 연결
		CreateIoCompletionPort((HANDLE)client_socket, iocp_, (ULONG_PTR)session, 0);

		// 최초 recv
		int last_error_code = session->CallRecv();
	}

	for (int i = 0; i < WORKER_COUNT; i++)
	{
		workers_[i]->detach();
		delete workers_[i];
	}

	closesocket(listen_socket_);
	WSACleanup();
}

int IocpServer::initSocket()
{
	//	WSAStartup	
	WSADATA wsa;
	int		result;

	result = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (result != 0)
	{
		return -1;
	}

	//	비동기용 소캣 생성 .ssm
	listen_socket_ = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (listen_socket_ == INVALID_SOCKET)
	{
		WSACleanup();

		return -1;
	}

	// 수신자의 주소 설정
	sockaddr_in     receiverAddr = {};
	receiverAddr.sin_family = AF_INET;					//	수신 프로토콜
	receiverAddr.sin_port = htons(port_);				//	수신자의 포트 번호
	receiverAddr.sin_addr.s_addr = INADDR_ANY;			//	수신할 IP

	//	bind
	// 소켓 바인딩
	result = bind(listen_socket_, (sockaddr*)&receiverAddr, sizeof(receiverAddr));

	if (result != 0)
	{
		closesocket(listen_socket_);
		WSACleanup();
		return -1;
	}

	//	listen
	if (listen(listen_socket_, SOMAXCONN) == SOCKET_ERROR)
	{
		closesocket(listen_socket_);
		WSACleanup();
		return -1;
	}

	return 0;
}

void IocpServer::InitIocp()
{
	// IOCP 생성( 초기화 ) 
	iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// std::thread::hardware_concurrency();  내컴퓨터 기준 스레드가 몇개 필요해를 알려주는 함수

	for (int i = 0; i < WORKER_COUNT; i++) // WORKER_COUNT ??? 
	{
		std::thread* worker = new std::thread(&IocpServer::worker, this);
		workers_.push_back(worker);
	}
}

void IocpServer::CloseSession(TcpSession* session)
{
	session->Close();
	session_map_.erase(session->GetSocket());
	delete session;
}