#pragma once
#include <mutex>

struct IO_DATA;
class  StreamBuffer;

class SpinLock {
	std::atomic<bool> lockFlag = false;

public:
	void lock() {
		bool expected = false;
		while (!lockFlag.compare_exchange_strong(expected, true)) {
			expected = false;
		}
	}

	void unlock() {
		lockFlag.store(false);
	}
};

class TcpSession
{
public:
	TcpSession();
	~TcpSession();

public:
	void Init(SOCKET socket);
	void Close();

	// recv ��û �Լ�
	const int CallRecv();

	// send ��û �Լ�
	const int CallSend(void* buffer, int size);

	// ���� recv�� �Ϸ�Ǿ�����
	size_t Recv(size_t recv_byte);

	// ���� send�� �Ϸ�Ǿ�����
	void Send();

public:
	inline const int GetLastError()
	{
		return last_error_code_;
	}

	inline const SOCKET GetSocket()
	{
		return socket_;
	}

	inline StreamBuffer* GetStreamBuffer()
	{
		return recv_buffer_;
	}

private:
	SOCKET socket_;

	/*
	WSAOVERLAPPED overlapped;
	char buffer[DATA_BUFSIZE];
	WSABUF dataBuf;
	IO_TYPE ioType;
	*/
	IO_DATA* send_info_;
	IO_DATA* recv_info_;
	StreamBuffer* recv_buffer_;

	unsigned long flags_;
	int last_error_code_;

	SpinLock send_lock_;
};

