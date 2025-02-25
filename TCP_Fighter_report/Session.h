#ifndef __SESSION__
#define __SESSION__

#include "resource.h"
#include "RingBuffer.h"
#include "PacketDefine.h"
#include <map>

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
		_ip = 0;
		_port = 0;
		_player = nullptr;
		_socket = 0;


		_timeout = timeGetTime();
		_delete = false;
	}

	~Session() {
		delete _player;

	}

	/*
	//Debug
	std::map<int, int> messageCount;
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
		_x = rand() % 6400;
		_y = rand() % 6400;
		_direction = (rand() % 2) * 4; //LL == 0, RR == 4
		_move = false;
		_ID = playerID++;
		pSession = nullptr;
	}

	Player(Session* pSes) {
		_hp = dfHP;
		_x = rand() % 6400;
		_y = rand() % 6400;
		_direction = (rand() % 2) * 4; //LL == 0, RR == 4
		_move = false;
		_ID = playerID++;
		pSession = pSes;
	}

	//------------------------------------------------------
	// 클라이언트에서 온 Direction만 반영하고 단순히 player의 moveFlag만 true로 바꿔줌
	//------------------------------------------------------
	bool MoveStart(BYTE Direction);

	bool Move(DWORD deltaTime);

	//------------------------------------------------------
// 클라이언트에서 온 Direction만 반영하고 단순히 player의 moveFlag만 false로 바꿔줌
//------------------------------------------------------
	void MoveStop(int Dir);

	


};




bool CreateNewCharacter(Session* _session);

//----------------------------------------------------------------
// 들어온 메세지의 헤더를 분리하고 헤더에 적힌 패킷 타입으로 switch case 분기를 탐
// 헤더에 오류가 있거나 덜 온 경우에 대한 예외처리 되어있음. 
// recvQ가 빌 때 까지 반복해서 Decode 작업
//----------------------------------------------------------------
bool DecodeMessages(Session* _session);
//----------------------------------------------------------------
// 나를 포함한 섹터 주변에 session의 Delete메세지를 보내고 DeleteArr에 _session push.
// _session의 delete flag를 true로 변경 후 프레임이 돌 때 지연 삭제를 기다림
//----------------------------------------------------------------
bool DeleteSession(Session* _session);

//----------------------------------------------------------------
// DeleteArr돌면서 Sector, Session Arr에서 삭제하고 CloseSocket까지 호출
// 만약 Sector에서 삭제가 안되면 (섹터에 이미 없었거나) Sector Delete error 로그 Enque
//----------------------------------------------------------------
void FlushDeleteArr();

//-----------------------------------------------------------------
// SessionArr다 돌면서 현재 시간 - TimeOut 기준 값 보다 TimeOut카운트가 낮으면 Delete
//-----------------------------------------------------------------
void TimeOutCheck();

//----------------------------------------------------------------
// _session의 좌표와 new좌표가 dfErrorRange이상으로 차이나면 클라이언트에 SyncMessage 전송
// dfErrorRange이하로 차이나면 클라이언트의 좌표 서버에 대입(todo)
//----------------------------------------------------------------
void SyncCheck(Session* _session, int newX, int newY, const char* File, const char* func, int Line, int ErrorCode);

#endif