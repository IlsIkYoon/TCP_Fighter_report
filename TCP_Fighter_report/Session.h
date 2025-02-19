#ifndef __SESSION__
#define __SESSION__

#include "resource.h"
#include "RingBuffer.h"
#include "PacketDefine.h"


extern DWORD playerCount;
extern int playerID;

class Player;

struct Session
{

public:
	SOCKET _socket;
	DWORD _ip;
	DWORD _port;
	RingBuffer _recvQ;
	RingBuffer _sendQ;
	Player* _player;

	DWORD _timeout;
	bool _delete;
	//Session* _next; 배열로 가기 때문에 필요 없음
	Session() {
		_timeout = timeGetTime();
		_delete = false;
		CreateCount = 0;
		DeleteCount = 0;
	}

	//*
	//Debug

	long long CreateCount;
	long long DeleteCount;
	
	//*/


};

class Player {

private:
	
public:
	bool _move;
	char _hp;
	short _x;
	short _y;
	BYTE _direction;
	int _ID;

	Session* pSession;
	


	Player() {
		_hp = dfHP;
		_x = 500;// rand() % 6400;
		_y = 500;// rand() % 6400;
		_direction = (rand() % 2) * 4; //LL == 0, RR == 4
		_move = false;
		_ID = playerID++;
	}

	Player(Session* pSes) {
		_hp = dfHP;
		_x = 500;// rand() % 6400;
		_y = 500;// rand() % 6400;
		_direction = (rand() % 2) * 4; //LL == 0, RR == 4
		_move = false;
		_ID = playerID++;
		pSession = pSes;
	}

	
	bool MoveStart(BYTE Direction, short X, short Y);

	bool Move(DWORD deltaTime);
	void MoveStop(int Dir, int x, int y);

	


};




bool CreateNewCharacter(Session* _session);
bool DecodeMessages(Session* _session);

bool DeleteSession(Session* _session);

void FlushDeleteArr();
void TimeOutCheck();


#endif