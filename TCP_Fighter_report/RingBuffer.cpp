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


bool RingBuffer::Enqueue(CHAR* chpData, DWORD size) {
	CHAR* pos; //링버퍼 포인터
	if (IsFull()) return false;

	DWORD extraBuf;

	if (_front == 0)
	{
		extraBuf = _bufsize - _rear - 1;
	}
	else
	{
		extraBuf = _bufsize - _rear; //배열의 0전까지의 남은 공간
	}
	/*

	if (_rear == _bufsize - 1) {
		pos = &_buf[0];
	}
	else { pos = &_buf[_rear + 1]; }

	*/
	char* pDest = chpData;

	if (GetBufferFree() < size) return false;
		
	if (extraBuf < size)
		{
			DWORD extraData = size - extraBuf;
			memcpy_s(&_buf[_rear], GetBufferFree(), pDest, extraBuf);
			_rear = 0;
			pDest += extraBuf;
			memcpy_s(&_buf[_rear], GetBufferFree() - extraBuf, pDest, extraData);
			_rear = extraData;
		}
	else
	{
		memcpy_s(&_buf[_rear], GetBufferFree(), pDest, size);
		_rear += size;
	}


	return true;
}

bool RingBuffer::Dequeue(CHAR* chpData, DWORD size) {

	if (IsEmpty()) return false;

	DWORD extraBuf = _bufsize - _front; //배열의 0전까지의 남은 공간 //16

	char* pDest = chpData;


	if (GetBufferUsed() < size) return false;


	if (size > extraBuf) {
		memcpy(pDest, &_buf[_front], extraBuf);
		_front = 0;
		pDest += extraBuf;
		memcpy(pDest, &_buf[_front], size - extraBuf);
		_front = size - extraBuf;

	}
	else {
		
		memcpy(pDest, &_buf[_front], size); 
		_front += size;
	}

	return true;
}

DWORD RingBuffer::Peek(char* chpDest, int iSize) {
	DWORD extraBuf = _bufsize - _front; //배열의 0전까지의 남은 공간

	char* pDest;
	pDest = chpDest;
	if (GetBufferUsed() < iSize) return false;


	if (iSize > extraBuf) {
		memcpy(pDest, &_buf[_front], extraBuf);
		pDest += extraBuf;
		memcpy(pDest, &_buf[0], iSize - extraBuf);

	}
	else {
		memcpy(pDest, &_buf[_front], iSize);
	}



}


void RingBuffer::ClearBuffer(void) {
	_front = 0;
	_rear = 0;
}


DWORD RingBuffer::DirectEnqueueSize(void) {

	/*
	if (_rear == _bufsize - 1) {
		//rear가 마지막에 도달했을 때에 대한 예외처리

		return _front;

	}
	*/

	if (_front = 0)
	{
		return _bufsize - 1 - _rear;
	}

	if (_rear < _front) {
		return _front - _rear - 1;
	}
	/*else if (_rear == _front) {

		_rear = 0;
		_front = 0;

		return _bufsize - 1;
	}*/
	else {
		return _bufsize - _rear;
	}
}

DWORD RingBuffer::DirectDequeueSize(void) {




	if (_rear < _front) {
		return _bufsize - _front;

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
		printf("iSize : %d GetBufferFree : %d\n", iSize, GetBufferFree());

		return false;
	}


	DWORD sum = _rear + iSize;

	if (sum > _bufsize - 1 ) {
		_rear = sum - (_bufsize - 1) - 1;
	}
	else {
		_rear = sum;
	}


	printf("\n Move Rear/////// Front : %d, Rear : %d//////////\n", _front, _rear);
	printf("iSize : %d, GetBufferFree : %d\n\n", iSize, GetBufferFree());


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

	//printf(" Move Front/////// Front : %d, Rear : %d//////////\n", _front, _rear);

	return true;
}







char* RingBuffer::GetFrontBufferPtr(void) {







	return &_buf[_front];
}
char* RingBuffer::GetRearBufferPtr(void) {



	return &_buf[_rear];
}
