#ifndef __SESSION__
#define __SESSION__

#include "resource.h"
#include "RingBuffer.h"
#include "PacketDefine.h"


extern DWORD playerIdex;


struct Session;


class Player {

private:
	bool _move;
	
public:
	char _hp;
	short _x;
	short _y;
	BYTE _direction;
	int _ID;

	Session* pSession;
	


	Player() {
		_hp = dfHP;
		_x = rand() % 6400;
		_y = rand() % 6400;
		_direction = (rand() % 2) * 4; //LL == 0, RR == 4
		_move = false;
		_ID = playerIdex;
	}

	Player(Session* pSes) {
		_hp = dfHP;
		_x = rand() % 6400;
		_y = rand() % 6400;
		_direction = (rand() % 2) * 4; //LL == 0, RR == 4
		_move = false;
		_ID = playerIdex;
		pSession = pSes;
	}

	
	bool MoveStart(BYTE Direction, short X, short Y) {
		_direction = Direction;
		_x = X;
		_y = Y;
		_move = true;

		return true;
	}

	bool Move();
	void MoveStop() 
	{
		_move = false;
	}


	//공격 (3가지)


};

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



bool CreateNewCharacter(Session* _session);
bool DecodeMessages(Session* _session);

bool DeleteSession(Session* _session);




#endif