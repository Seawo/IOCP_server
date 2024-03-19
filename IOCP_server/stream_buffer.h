#pragma once

#define STREAM_BUFSIZE 4096
// 버퍼 사이즈 키우기
class StreamBuffer
{
public:
	StreamBuffer();
	~StreamBuffer();

public:
	size_t Enqueue(char* data, size_t size);		  // 입력 버퍼
	void Dequeue(char* get_buffer, size_t get_size);  // 출력 버퍼

	inline size_t GetSize()
	{
		return size_;
	}

	template<typename T>
	inline T PeekBuffer()
	{
		T return_data;
		size_t type_size = sizeof(T);

		for (size_t i = 0; i < type_size; i++)
		{
			size_t peek_pos = ((head_ + i) % max_);
			memcpy_s(((char*)(&return_data)) + i, 1, &buffer_[peek_pos], 1);
		}

		return return_data;
	}

private:
	char buffer_[STREAM_BUFSIZE];
	size_t head_;
	size_t tail_;
	size_t size_;
	const size_t max_;
};