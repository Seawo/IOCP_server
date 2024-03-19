#include "stream_buffer.h"
#include <windows.h>

StreamBuffer::StreamBuffer()
	:max_(STREAM_BUFSIZE)
{
	ZeroMemory(buffer_, STREAM_BUFSIZE);
	head_ = 0;
	tail_ = 0;
	size_ = 0;
}

StreamBuffer::~StreamBuffer()
{

}

size_t StreamBuffer::Enqueue(char* data, size_t data_size)
{
	if (max_ - size_ < data_size)
		return 0;

	// 복사 가능한 사이즈
	size_t available_size = 0;

	// 복사 시작위치
	size_t start_pos = 0;

	size_t copy_size = data_size;

	// tail이 head보다 클떄 (일반적인 상황)
	if (tail_ >= head_)
	{
		available_size = max_ - tail_;
		start_pos = tail_;
	}
	// 버퍼가 뒤로 한바퀴 돌아간 경우
	else
	{
		available_size = head_ - tail_;
		start_pos = tail_;
	}

	if (copy_size > available_size)
		copy_size = available_size;


	// 위치 증가
	tail_ += copy_size;
	size_ += copy_size;

	if (tail_ == max_)
	{
		tail_ = 0;
	}

	// 복사 가능한 사이즈 만큼 복사
	memcpy_s(&buffer_[start_pos], available_size, data, available_size);


	// 복사한 사이즈가 들어온 사이즈 보다 작다면 재귀호출
	size_t call_size = 0;
	if (copy_size < data_size)
	{
		call_size = Enqueue(&data[copy_size], data_size - copy_size);
	}

	return (copy_size + call_size);
}

void StreamBuffer::Dequeue(char* get_buffer, size_t get_size)
{
	// 복사 가능한 사이즈
	size_t available_size = 0;

	if (size_ < get_size)
		get_size = size_;

	// tail이 head보다 클떄 (일반적인 상황)
	if (tail_ >= head_)
		available_size = tail_ - head_;
	// 버퍼가 뒤로 한바퀴 돌아간 경우
	else
		available_size = max_ - head_;


	if (available_size > get_size)
		available_size = get_size;

	memcpy_s(get_buffer, get_size, &buffer_[head_], available_size);

	size_ -= available_size;
	head_ += available_size;
	get_size -= available_size;

	if (head_ == max_)
		head_ = 0;


	if (size_ == 0)
	{
		head_ = 0;
		tail_ = 0;
	}

	if (get_size > 0)
	{
		Dequeue(&get_buffer[available_size], get_size);
	}
}