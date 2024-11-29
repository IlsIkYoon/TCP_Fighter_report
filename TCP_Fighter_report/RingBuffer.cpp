#include "RingBuffer.h"



bool RingBuffer::IsEmpty() {
	if (_rear == _front) {
		return true;
	}
	else {
		return false;
	}
}

bool RingBuffer::IsFull() {
	if (_front - 1 == _rear || (_front == 0 && _rear == _bufsize - 1)) {
		return true;
	}
	else {
		return false;
	}
}

//링버퍼 인큐 디큐 에러... 예외처리 더 해야 함 
bool RingBuffer::Enqueue(CHAR* chpData, DWORD size) {
	CHAR* pos; //링버퍼 포인터
	if (IsFull()) return false;

	if (_rear == _bufsize - 1) {
		pos = &_buf[0];
	}
	else { pos = &_buf[_rear + 1]; }


	char* pDest = chpData;

	DWORD extraBuf = _bufsize - 1 - _rear; //배열의 0전까지의 남은 공간
	if (GetBufferFree() >= size)
	{
		if (extraBuf < size)
		{
			DWORD extraData = size - extraBuf;
			memcpy_s(pos, GetBufferFree(), pDest, extraBuf);
			pos = &_buf[0];
			pDest += extraBuf;
			memcpy_s(pos, GetBufferFree() - extraBuf, pDest, extraData);
			_rear = extraData - 1;
		}
		else
		{
			memcpy_s(pos, GetBufferFree(), pDest, size);
			_rear += size;
		}
	}
	else {
		return false;
	}

	return true;
}

bool RingBuffer::Dequeue(CHAR* chpData, DWORD size) {

	if (IsEmpty()) return false;

	DWORD extraBuf = _bufsize - 1 - _front; //배열의 0전까지의 남은 공간 //16

	char* pDest = chpData;


	if (GetBufferUsed() < size) return false;


	if (size > extraBuf) {
		memcpy(pDest, &_buf[_front + 1], extraBuf);
		pDest += extraBuf;
		memcpy(pDest, &_buf[0], size - extraBuf);
		_front = size - extraBuf - 1; //front에 1이 들어가야 되는데 ? 

	}
	else {
		memcpy(pDest, &_buf[_front + 1], size); //front에 데이터가 없는게 맞는지 확인 필요
		_front += size;
	}

	return true;
}

DWORD RingBuffer::Peek(char* chpDest, int iSize) {
	DWORD extraBuf = _bufsize - 1 - _front; //배열의 0전까지의 남은 공간

	char* pDest;
	pDest = chpDest;
	if (GetBufferUsed() < iSize) return false;


	if (iSize > extraBuf) {
		memcpy(pDest, &_buf[_front + 1], extraBuf);
		pDest += extraBuf;
		memcpy(pDest, &_buf[0], iSize - extraBuf);

	}
	else {
		memcpy(pDest, &_buf[_front + 1], iSize);
	}



}


void RingBuffer::ClearBuffer(void) {
	_front = 0;
	_rear = 0;
}


DWORD RingBuffer::DirectEnqueueSize(void) {

	if (_rear < _front) {
		return _front - _rear - 1;
	}
	/*else if (_rear == _front) {

		_rear = 0;
		_front = 0;

		return _bufsize - 1;
	}*/
	else {
		return _bufsize - 1 - _rear;
	}
}

DWORD RingBuffer::DirectDequeueSize(void) {

	if (_rear < _front) {
		return _bufsize - 1 - _front;

	}

	/*
	else if (_rear == _front) {
		_rear = 0;
		_front = 0;
		return 0;
	}
	*/

	else {
		return _rear - _front;
	}

}

bool	RingBuffer::MoveRear(int iSize) {

	if (iSize > GetBufferFree()) {
		return false;
	}


	DWORD sum = _rear + iSize;

	if (sum > _bufsize - 1) {
		_rear = sum - (_bufsize - 1) - 1;
	}
	else {
		_rear = sum;
	}


	return true;
}
bool RingBuffer::MoveFront(int iSize) {

	if (iSize > GetBufferUsed()) {
		return false;
	}

	DWORD sum = _front + iSize;

	if (sum > _bufsize - 1) {
		_front = sum - (_bufsize - 1) - 1;
	}
	else {
		_front = sum;
	}

	return true;
}







char* RingBuffer::GetFrontBufferPtr(void) {



	return &_buf[_front];
}
char* RingBuffer::GetRearBufferPtr(void) {

	if (_rear == _bufsize - 1) {
		return &_buf[0];
	}
	else {
		return &_buf[_rear];
	}
}
