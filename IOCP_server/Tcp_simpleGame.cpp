#include "Tcp_simpleGame.h"

#include <iostream>

#include "packet_handler.h"
#include "tcp_session.h"

#include "mySQL\include\mysql\jdbc.h"

#include "mySQL/include/mysql/jdbc.h"
#pragma comment(lib, "mySQL/lib64/vs14/debug/mysqlcppconn.lib")

//#include <string> // ���ڿ��� �ޱ� ���ؼ� ���
//#include <cstring>

enum GAME_PACKET
{
	GET_ID,
	POS,
	EXIT_USER,
	LOGIN,
	TEST,
};

#pragma pack(push, 1)
struct Get_GAME_IDPacket : public BasePacket
{
	Get_GAME_IDPacket()
	{
		id = 0;
		packet_id = GET_ID;
		packet_len = sizeof(Get_GAME_IDPacket);
	}
	int id;
};
struct PosPacket : public BasePacket
{
	PosPacket()
	{
		packet_id = POS;
		packet_len = sizeof(PosPacket);
	}

	int		id = 0;
	float	pos[3] = {};
	float	rot[4] = {};
};
struct ExitPacket : public BasePacket
{
	ExitPacket()
	{
		packet_id = EXIT_USER;
		packet_len = sizeof(ExitPacket);
	}
	int id = 0;
};
struct LoginPacket : public BasePacket
{
	LoginPacket()
	{
		packet_id = LOGIN;
		packet_len = sizeof(LoginPacket);
	}
	int		id = 0;
	int		check = 0;
	//char	userNum[100];
};
struct TestPacket : public BasePacket
{
	TestPacket()
	{
		packet_id = TEST;
		packet_len = sizeof(TestPacket);
	}
	int id = 0;
	
	//std::string test;
	//int test = 0;
	//char test[20];  // char ���´� �����͸� �޴´� ������ ���� 3/11
	//wchar_t test[256];  // wchar ���´� �����͸� �޴´�
};
#pragma pack(pop)

Tcp_simpleGame::Tcp_simpleGame()
{
}

#define NEWINT(x) int num##x;
#define SIMPLEREGI(packName) REGIST_PACKET(packek_handler_, GAME_PACKET::##packName, Tcp_simpleGame::##packName)

void Tcp_simpleGame::Init()
{
	NEWINT(count);
	numcount = 0;

	SIMPLEREGI(GET_ID);

	REGIST_PACKET(packek_handler_, GAME_PACKET::POS, Tcp_simpleGame::Pos);

	REGIST_PACKET(packek_handler_, GAME_PACKET::LOGIN, Tcp_simpleGame::Login);
	REGIST_PACKET(packek_handler_, GAME_PACKET::TEST, Tcp_simpleGame::Test);

}

void Tcp_simpleGame::ExitSession(TcpSession* exitSession)
{
	ExitPacket response;
	response.id = (int)exitSession->GetSocket();

	std::cout << "������ ���� ID : " << response.id << std::endl;

	// ��ü ����
	for (auto iter = session_map_.begin(); iter != session_map_.end(); iter++)
	{
		TcpSession* other = reinterpret_cast<TcpSession*>(iter->second);

		if (iter->first == exitSession->GetSocket())
			continue;

		other->CallSend(&response, sizeof(ExitPacket));
	}
}

void Tcp_simpleGame::Pos(TcpSession* session, BasePacket* packet)
{
	PosPacket* pos_packet = reinterpret_cast<PosPacket*>(packet);

	PosPacket response;
	memcpy_s(&response, sizeof(PosPacket), packet, sizeof(PosPacket));

	//std::cout << "pos : " << response.pos << std::endl;
	//std::cout << "rot : " << response.rot << std::endl;

	// ��ü ����
	for (auto iter = session_map_.begin(); iter != session_map_.end(); iter++)
	{
		TcpSession* other = reinterpret_cast<TcpSession*>(iter->second);

		// ����� �Ⱥ����� == ������ �� ��ġ�� ���� �ʿ�� �����ϱ�
		//if (iter->first == session->GetSocket())
		//	continue;

		other->CallSend(&response, sizeof(PosPacket));
	}
}

void Tcp_simpleGame::GET_ID(TcpSession* session, BasePacket* packet)
{
	Get_GAME_IDPacket* recvPacket = reinterpret_cast<Get_GAME_IDPacket*>(packet);

	Get_GAME_IDPacket response;
	response.id = (int)session->GetSocket();

	std::cout << "������ ���� ID : " << response.id << std::endl;

	session->CallSend(&response, sizeof(Get_GAME_IDPacket));

}

void Tcp_simpleGame::Login(TcpSession* session, BasePacket* packet)
{
	LoginPacket* loginPacket = reinterpret_cast<LoginPacket*>(packet);

	LoginPacket response;

	// �޸� ���� -> ����Ƽ Mash�� ���� ���δ�
	memcpy_s(&response, sizeof(LoginPacket), packet, sizeof(LoginPacket));

	response.check = 2000;

	session->CallSend(&response, sizeof(LoginPacket));
	

	//// ��ü ����
	//for (auto iter = session_map_.begin(); iter != session_map_.end(); iter++)
	//{
	//	TcpSession* other = reinterpret_cast<TcpSession*>(iter->second);

	//	// �����Ը� ������
	//	if (iter->first == session->GetSocket())
	//	{
	//		std::cout << "���� BasePacket : " << packet->packet_id << std::endl;
	//		std::cout << "�α��� �� ID : " << response.id << std::endl;
	//		std::cout << "�α��� �� userID : " << response.userNum << std::endl;

	//		other->CallSend(&response, sizeof(LoginPacket));
	//	}
	//}

}

#include <locale> // �ѱ� �����°� �����ϱ����� wcout
#include <codecvt>

#include <sstream> // stringstream ����ϱ� ����
#include <iomanip> // str()
#include <cstring>

void Tcp_simpleGame::Test(TcpSession* session, BasePacket* packet)
{
	TestPacket* recvPacket = reinterpret_cast<TestPacket*>(packet);
	
	TestPacket response;
	
	memcpy_s(&response, sizeof(TestPacket), packet, sizeof(TestPacket));

	response.id = 1004;
	//strcpy_s(response.test, sizeof(response.test), "test");// �������� �߻�
	//strncpy_s(response.test, sizeof(response.test), "tas", _TRUNCATE);

	std::cout << "id : " << response.id << std::endl;
	//std::cout << "test : " << response.test << std::endl;

	/* DB ����
	std::locale::global(std::locale("en_US.UTF-8"));
	std::wcout.imbue(std::locale());

	//DB���� �����ʹ� �� �����´�
	char sqltest[256] = "test";
	//std::string sqltest = response.test; //"test";
	std::wstring sqlres;

	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	sqlres = converter.from_bytes(SQLconnect(sqltest));

	std::wcout << "sqlres : " << sqlres.c_str() << std::endl;
	*/
	
	session->CallSend(&response, sizeof(TestPacket));

	//// ��ü ����
	//for (auto iter = session_map_.begin(); iter != session_map_.end(); iter++)
	//{
	//	TcpSession* other = reinterpret_cast<TcpSession*>(iter->second);
	//	other->CallSend(&response, sizeof(TestPacket));
	//}
}

using std::string;

string Tcp_simpleGame::SQLconnect(std::string id)
{
	// executeQuery(string ���ڿ��̶� �������ߵȴ�)
	std::string select_SQL = "Select NickName From tb_info Where id=";
	std::string idstr = id;
	//std::cin >> idstr;
	select_SQL.append("'" + idstr + "'");

	auto driver = sql::mysql::get_driver_instance();
	// DB ������ ���� ���̵� ����� �Է�
	auto con = driver->connect("tcp://3.38.115.189:3306", "test", "1234");

	con->setSchema("Seungwoo"); // DB ����
	auto stmt = con->createStatement();

	// ������ ������
	auto res = stmt->executeQuery(select_SQL);

	auto metaData = res->getMetaData();
	auto columCnt = metaData->getColumnCount();

	while (res->next())
	{
		id = "";
		for (int i = 0; i < columCnt; ++i)
		{
			id += res->getString(i + 1) + " ";
		}
	}
	// ����
	delete res;
	delete stmt;
	delete con;

	//system("pause");
	// ���� �ý����� ��� �����. 
	return id;
}



