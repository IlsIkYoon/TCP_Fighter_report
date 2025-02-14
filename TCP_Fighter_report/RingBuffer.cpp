#include "RingBuffer.h"
#include "resource.h"
#include "Log.h"

extern CRITICAL_SECTION g_lock;

extern std::list<std::string> LogQ; 

RingBuffer::RingBuffer()
{
	_buf = (char*)malloc(DefaultBufSize);
	_rear = 0;
	_front = 0;
	_bufSize = DefaultBufSize;
}
RingBuffer::RingBuffer(int size)
{
	_buf = (char*)malloc(size);
	_rear = 0;
	_front = 0;
	_bufSize = size;

}
RingBuffer::~RingBuffer()
{
	delete _buf;
}



bool RingBuffer::IsEmpty()
{
	if (_rear == _front) return true;

	return false;
}
bool RingBuffer::IsFull()
{
	if ((_rear == _bufSize - 1) && (_front == 0)) return true;
	if (_rear == _front - 1) return true;


	return false;
}

unsigned int RingBuffer::GetSizeTotal()
{
	return _bufSize - 1;

}
unsigned int RingBuffer::GetSizeFree()
{
	return GetSizeTotal() - GetSizeUsed();
}
unsigned int RingBuffer::GetSizeUsed()
{
	if (_rear > _front) return _rear - _front;
	if (_rear == _front) return 0;
	if (_front > _rear) return _bufSize - _front + _rear;


	std::string logEntry = std::format("RingBuffer Error || FILE : {}, Func : {} , Line : {} error : {}\n",
		getFileName(__FILE__), __func__, __LINE__, GetLastError());
	EnterCriticalSection(&g_lock);
	LogQ.push_back(logEntry);
	LeaveCriticalSection(&g_lock);

	return -1;
}

unsigned int RingBuffer::GetDirectEnqueSize()
{
	if (IsFull()) return 0;

	if (_front > _rear) return _front - _rear - 1;
	if (_front == _rear)
	{
		if (_rear == 0)
		{
			return GetSizeTotal();
		}

		return _bufSize - _rear;

	}

	if (_rear > _front)
	{
		if (_front == 0) return _bufSize - _rear - 1;

		//if (_bufSize - _rear == 0) return 1; //front가 0인 상황은 isFull에서 걸러짐

		return _bufSize - _rear;

	}


	std::string logEntry = std::format("RingBuffer Error || FILE : {}, Func : {} , Line : {} error : {}\n",
		getFileName(__FILE__), __func__, __LINE__, GetLastError());
	EnterCriticalSection(&g_lock);
	LogQ.push_back(logEntry);
	LeaveCriticalSection(&g_lock);
	return -1;
}
unsigned int RingBuffer::GetDirectDequeSize()
{
	if (IsEmpty()) return 0;

	if (_front == _rear) return 0;
	if (_rear > _front) return _rear - _front;
	if (_front > _rear)
	{

		return _bufSize - _front;
	}

	std::string logEntry = std::format("RingBuffer Error || FILE : {}, Func : {} , Line : {} error : {}\n",
		getFileName(__FILE__), __func__, __LINE__, GetLastError());
	EnterCriticalSection(&g_lock);
	LogQ.push_back(logEntry);
	LeaveCriticalSection(&g_lock);
	return -1;

}



void RingBuffer::MoveRear(int val)
{
	_rear = (_rear + val) % _bufSize;


}
void RingBuffer::MoveFront(int val)
{
	_front = (_front + val) % _bufSize;


}






bool RingBuffer::Enqueue(char* src, unsigned int dataSize, unsigned int* enqueResult)
{

	if (IsFull())
	{
		*enqueResult = 0;
		return false;
	}


	unsigned int Data;
	int out;

	if (GetSizeFree() < dataSize) Data = GetSizeFree();
	else { Data = dataSize; }

	out = Data;

	if (GetDirectEnqueSize() >= Data)
	{
		memcpy(&_buf[_rear], src, Data);
		MoveRear(Data);

	}
	else
	{
		int dir = GetDirectEnqueSize();
		memcpy(&_buf[_rear], src, dir);
		Data -= dir;
		_rear = 0;
		memcpy(&_buf[_rear], &src[dir], Data);
		MoveRear(Data);

	}

	*enqueResult = out;
	return true;

}
bool RingBuffer::Dequeue(char* dest, unsigned int dataSize, unsigned int* dequeResult)
{
	if (IsEmpty())
	{
		*dequeResult = 0;
		return false;
	}

	unsigned int Data;
	int out;

	if (GetSizeUsed() < dataSize) Data = GetSizeUsed();
	else { Data = dataSize; }

	out = Data;

	if (GetDirectDequeSize() >= Data)
	{
		memcpy(dest, &_buf[_front], Data);
		MoveFront(Data);

	}
	else
	{
		int dqDir = GetDirectDequeSize();
		memcpy(dest, &_buf[_front], dqDir);
		Data -= dqDir;
		_front = 0;
		memcpy(&dest[dqDir], &_buf[_front], Data);
		MoveFront(Data);

	}

	*dequeResult = out;
	return true;

}



char* RingBuffer::GetRear()
{
	return &_buf[_rear];
}
char* RingBuffer::GetFront()
{
	return &_buf[_front];
}


bool RingBuffer::Peek(char* dest, unsigned int dataSize, unsigned int* dequeResult)
{
	if (IsEmpty())
	{
		*dequeResult = 0;
		return false;
	}

	unsigned int Data;
	int out;

	if (GetSizeUsed() < dataSize) Data = GetSizeUsed();
	else { Data = dataSize; }

	out = Data;

	if (GetDirectDequeSize() >= Data)
	{
		memcpy(dest, &_buf[_front], Data);

	}
	else
	{
		int dqDir = GetDirectDequeSize();
		memcpy(dest, &_buf[_front], dqDir);
		Data -= dqDir;
		memcpy(&dest[dqDir], &_buf[0], Data);

	}

	*dequeResult = out;
	return true;

}