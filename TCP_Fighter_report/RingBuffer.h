#ifndef __RingBuffer__
#define __RingBuffer__

#include "resource.h"

#define BUFSIZE 1024

class RingBuffer {
	//환형큐. empty일 때 _front == _rear; full이라면 _rear가 _front보다 한 칸 낮음
public:
	CHAR* _buf;
	DWORD _bufsize;
	DWORD _front; //front에 디큐
	DWORD _rear; //rear에 인큐
	//empty와 full의 구분을 어떻게 할까 

public:

	RingBuffer()
	{
		//생성자
		_buf = new CHAR[BUFSIZE + 1];
		_bufsize = BUFSIZE + 1;
		_front = 0;
		_rear = 0;

	}
	RingBuffer(int iBufSize)
	{
		_buf = new CHAR[iBufSize + 1];
		_bufsize = iBufSize + 1;
		_front = 0;
		_rear = 0;
	}
	~RingBuffer() {

		delete[] _buf;

		//소멸자
	}
	bool IsEmpty();
	bool IsFull();
	// WritePos 에 데이타 넣음.
	// Parameters: (char *)데이타 포인터. (int)크기. 
	// Return: (int)넣은 크기.
	//Rear가 이동후 해당 자리에 데이터 입력
	bool Enqueue(CHAR* chpData, DWORD size);

	// ReadPos 에서 데이타 가져옴. ReadPos 이동.
	//
	// Parameters: (char *)데이타 포인터. (int)크기.
	// Return: (int)가져온 크기.
	//front 이동 후 가리키는 데이터 삭제 
	bool Dequeue(CHAR* chpData, DWORD size);
	/////////////////////////////////////////////////////////////////////////
// ReadPos 에서 데이타 읽어옴. ReadPos 고정.
//
// Parameters: (char *)데이타 포인터. (int)크기.
// Return: (int)가져온 크기.
/////////////////////////////////////////////////////////////////////////
	DWORD Peek(char* chpDest, int iSize);


	/////////////////////////////////////////////////////////////////////////
	// 버퍼의 모든 데이타 삭제.
	//
	// Parameters: 없음.
	// Return: 없음.
	/////////////////////////////////////////////////////////////////////////
	void	ClearBuffer(void);

	DWORD GetBufferUsed()
	{
		if (_rear < _front) {
			//한바퀴를 넘어 있는 상황
			DWORD sum;
			sum = _bufsize - 1 - _front;
			sum += _rear + 1;
			return sum;
		}
		else {
			return _rear - _front;
		}
	}
	DWORD GetBufferFree()
	{
		return _bufsize - GetBufferUsed() - 1;
	}

	/////////////////////////////////////////////////////////////////////////
// 버퍼 포인터로 외부에서 한방에 읽고, 쓸 수 있는 길이.
// (끊기지 않은 길이)
//
// 원형 큐의 구조상 버퍼의 끝단에 있는 데이터는 끝 -> 처음으로 돌아가서
// 2번에 데이터를 얻거나 넣을 수 있음. 이 부분에서 끊어지지 않은 길이를 의미
//
// Parameters: 없음.
// Return: (int)사용가능 용량.
////////////////////////////////////////////////////////////////////////
	DWORD DirectEnqueueSize(void);
	DWORD DirectDequeueSize(void);
	/////////////////////////////////////////////////////////////////////////
// 원하는 길이만큼 읽기위치 에서 삭제 / 쓰기 위치 이동
//
// Parameters: 없음.
// Return: (int)이동크기
/////////////////////////////////////////////////////////////////////////
	bool MoveRear(int iSize);
	bool MoveFront(int iSize);


	/////////////////////////////////////////////////////////////////////////
	// 버퍼의 Front 포인터 얻음.
	//
	// Parameters: 없음.
	// Return: (char *) 버퍼 포인터.
	/////////////////////////////////////////////////////////////////////////
	char* GetFrontBufferPtr(void);


	/////////////////////////////////////////////////////////////////////////
	// 버퍼의 RearPos 포인터 얻음.
	//
	// Parameters: 없음.
	// Return: (char *) 버퍼 포인터.
	/////////////////////////////////////////////////////////////////////////
	char* GetRearBufferPtr(void);





};


#endif