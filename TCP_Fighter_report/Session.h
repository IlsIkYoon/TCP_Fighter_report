#pragma once

#include "resource.h"
#include "RingBuffer.h"

class Player {
	DWORD _hp;
	DWORD _x;
	DWORD _y;
	DWORD _direction;



	//이동 
	//공격 (3가지)


};

struct Session {
	
	SOCKET _socket;
	DWORD _ip;
	DWORD _port;
	RingBuffer _recvQ;
	RingBuffer _sendQ;
	Player* _player;
	
	//Session* _next; 배열로 가기 때문에 필요 없음

};

