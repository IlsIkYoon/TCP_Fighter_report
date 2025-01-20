#pragma once

#include "resource.h"
#include "Session.h"

struct Session
{

public:
	SOCKET _socket;
	DWORD _ip;
	DWORD _port;
	RingBuffer _recvQ;
	RingBuffer _sendQ;
	Player* _player;

	bool _delete;
	//Session* _next; 배열로 가기 때문에 필요 없음

};