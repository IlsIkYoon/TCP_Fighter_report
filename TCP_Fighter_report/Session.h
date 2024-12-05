#ifndef __SESSION__
#define __SESSION__

#include "resource.h"
#include "RingBuffer.h"
#include "PacketDefine.h"

extern DWORD playerCount;

struct Player {

private:
	bool _move;
	
public:
	char _hp;
	short _x;
	short _y;
	BYTE _direction;
	int _ID;
	


	Player() {
		_hp = dfHP;
		_x = 320;
		_y = 240;
		_direction = LL;
		_move = false;
		_ID = playerCount;
	}

	
	bool MoveStart(BYTE Direction, short X, short Y) {
		_direction = Direction;
		_x = X;
		_y = Y;
		_move = true;

		return true;
	}

	bool Move() {
		if (_move == false) return false;

		if (_x > dfRANGE_MOVE_RIGHT || _x < dfRANGE_MOVE_LEFT || _y > dfRANGE_MOVE_BOTTOM || _y < dfRANGE_MOVE_TOP) return false;

		
		switch (_direction) {
		case dfPACKET_MOVE_DIR_LL:
		{
			_x -= 3;;
		}
			break;

		case dfPACKET_MOVE_DIR_LU:
		{
			_x -= 3;
			_y -= 2;
		}

			break;

		case dfPACKET_MOVE_DIR_UU:
		{
			_y -= 2;
		}

			break;

		case dfPACKET_MOVE_DIR_RU:
		{
			_x += 3;
			_y -= 2;
		}
				break;

		case dfPACKET_MOVE_DIR_RR:
		{
			_x += 3;
		}
			break;

		case dfPACKET_MOVE_DIR_RD:
		{
			_x += 3;
			_y += 2;
		}
			break;

		case dfPACKET_MOVE_DIR_DD:
		{
			_y += 2;
		}
			break;

		case dfPACKET_MOVE_DIR_LD:
		{
			_x -= 3;
			_y += 2;
		}
			break;

		defalut : 

			break;
		}

		return true;
	}

	void MoveStop() 
	{
		_move = false;
	}


	//공격 (3가지)


};

struct Session {
	
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
bool DecodePacket(Session* _session);






#endif