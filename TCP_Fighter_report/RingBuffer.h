#pragma once

#include <iostream>
#include <stdlib.h>

#define DefaultBufSize 4 * 1024 * 10



class RingBuffer
{

	//환형큐 방식. rear와 front가 같으면 empty, rear == front - 1이면 full

	char* _buf;
	int _rear;
	int _front;
	int _bufSize;


public:

	RingBuffer();
	RingBuffer(int size);
	~RingBuffer();

	bool Enqueue(char* src, unsigned int dataSize, unsigned int* enqueResult);
	bool Dequeue(char* dest, unsigned int dataSize, unsigned int* dequeResult);
	bool Peek(char* dest, unsigned int dataSize, unsigned int* dequeResult);

	//Todo//Peek 만들기//
	bool IsEmpty();
	bool IsFull();

	unsigned int GetSizeTotal();
	unsigned int GetSizeFree();
	unsigned int GetSizeUsed();

	unsigned int GetDirectEnqueSize();
	unsigned int GetDirectDequeSize();


	void MoveRear(int val);
	void MoveFront(int val);
	char* GetRear();
	char* GetFront();
};


