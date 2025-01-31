#include "Session.h"
#include "GameProc.h"
#include "Sector.h"
#include "Message.h"

//전역변수
std::list<Session*> SessionArr;
std::list<Session*>::iterator s_ArrIt;

DWORD timeOutCount = 0;
DWORD playerIdex = 0; //63명까지 접속을 받는 상황으로 가정

std::vector<Session*> DeleteArr; 

int playerID = 1;

extern DWORD SyncCount;

extern std::list<Session*> Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];
extern Session* pSector;


bool CreateNewCharacter(Session* _session) {

	PacketHeader pHeader;
	SC_CREATE_MY_CHARACTER CreatePacket;
	SC_CREATE_OTHER_CHARACTER other_myCharacter;
	SC_CREATE_OTHER_CHARACTER otherCharacter;
	std::list<Session*>::iterator it;

	unsigned int EnqueOut = 0;

	int ringbuffer_retval;


	//내 캐릭터 생성
	pHeader.byCode = 0x89;
	pHeader.bySize = sizeof(SC_CREATE_MY_CHARACTER); //12
	pHeader.byType = dfPACKET_SC_CREATE_MY_CHARACTER;
	
	CreatePacket.Direction = _session->_player->_direction;
	CreatePacket.HP = _session->_player->_hp;
	CreatePacket.X = _session->_player->_x;
	CreatePacket.Y = _session->_player->_y;
	CreatePacket.ID = playerID++;

	if (_session->_sendQ.GetSizeFree() < sizeof(pHeader) + pHeader.bySize)
	{
			//Todo//enque 할 수 없는 상황임 (일어날 가능성이 없음)
			printf("CreateMyCharacter buffer error : %d, LINE : %d\n", GetLastError(), __LINE__);
			return false;
	}


	if (_session->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &EnqueOut) == false)
	{
		printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), EnqueOut);
		return false;
	}

	if (_session->_sendQ.Enqueue((char*)&CreatePacket, pHeader.bySize, &EnqueOut) == false)
	{
		printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), EnqueOut);
		return false;
	}


	//----------------------------------------------//
	//Todo// 내 주변 캐릭터 생성 //Sector 기준

	pHeader.byCode = 0x89;
	pHeader.bySize = sizeof(otherCharacter);
	pHeader.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;
	


	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if ((_session->_player->_x + i) < 0 || _session->_player->_x + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
			if ((_session->_player->_y + j) < 0 || _session->_player->_y + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


			it = Sector[_session->_player->_x + i][_session->_player->_y + j].begin();

			for (; it != Sector[_session->_player->_x + i][_session->_player->_y + j].end(); it++)
			{
				otherCharacter.ID = (*it)->_player->_ID;
				otherCharacter.HP = (*it)->_player->_hp;
				otherCharacter.Direction = (*it)->_player->_direction;
				otherCharacter.X = (*it)->_player->_x;
				otherCharacter.Y = (*it)->_player->_y;

				if (otherCharacter.ID == _session->_player->_ID) continue;

				if (_session->_sendQ.GetSizeFree() < sizeof(pHeader) + pHeader.bySize)
				{
					printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
					//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
					return false;
				}

				if (_session->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &EnqueOut) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), EnqueOut);
					return false;
				}

				if (_session->_sendQ.Enqueue((char*)&otherCharacter, pHeader.bySize, &EnqueOut) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), EnqueOut);
					return false;
				}

			}
		}
	}



	//Todo// 주변 캐릭터들에게 나 생성하라고 메시지 보내기 // Sector 기준
	
	pHeader.bySize = sizeof(other_myCharacter);


	other_myCharacter.Direction = _session->_player->_direction;
	other_myCharacter.HP = _session->_player->_hp;
	other_myCharacter.ID = playerIdex;
	other_myCharacter.X = _session->_player->_x;
	other_myCharacter.Y = _session->_player->_y;

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if ((_session->_player->_x + i) < 0 || _session->_player->_x + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
			if ((_session->_player->_y + j) < 0 || _session->_player->_y + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


			it = Sector[_session->_player->_x + i][_session->_player->_y + j].begin();

			for (; it != Sector[_session->_player->_x + i][_session->_player->_y + j].end(); it++)
			{

				if ((*it)->_player->_ID == _session->_player->_ID) continue;

				if ((*it)->_sendQ.GetSizeFree() < sizeof(pHeader) + pHeader.bySize)
				{
					printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
					//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
					return false;
				}

				if ((*it)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &EnqueOut) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), EnqueOut);
					return false;
				}

				if ((*it)->_sendQ.Enqueue((char*)&other_myCharacter, pHeader.bySize, &EnqueOut) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), EnqueOut);
					return false;
				}

			}
		}
	}

	return true;
}


bool DecodeMessages(Session* _session) 
{

	//_session->_timeout = timeGetTime();

	unsigned int peekResult;
	PacketHeader pHeader;

	while (1)
	{
		ZeroMemory(&pHeader, sizeof(pHeader));

		if (_session->_recvQ.IsEmpty() == true) break; //버퍼 빌 때까지 

		_session->_recvQ.Peek((char*)&pHeader, sizeof(pHeader), &peekResult);

		if (peekResult < sizeof(pHeader))
		{
			printf("Line : %d, header error\n", __LINE__); //헤더도 안 온 경우엔 바로 리턴
			return false;
		}


		if (pHeader.byCode != 0x89)
		{
			printf("protocol code error : %d\n", pHeader.byCode);
			DeleteSession(_session); //맞는 코드가 아니라면 소켓 연결 종료
			return false;
		}

		_session->_recvQ.MoveFront(sizeof(pHeader));

		int bufUsed = _session->_recvQ.GetSizeUsed();
		if (bufUsed < pHeader.bySize) //PayLoad가 다 안왔으면 다시 헤더를 넣고 리턴
		{
			char* extraBuf = (char*)malloc(bufUsed);
			unsigned int dequeResult;
			unsigned int enqueResult;
			_session->_recvQ.Dequeue(extraBuf, bufUsed, &dequeResult);
			_session->_recvQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			_session->_recvQ.Enqueue(extraBuf, bufUsed, &enqueResult);
			return false;
		}




		switch (pHeader.byType)
		{



		case dfPACKET_CS_MOVE_START:

			MoveStart(_session);


			break;

		case dfPACKET_CS_MOVE_STOP:

			MoveStop(_session);

			break;


		case dfPACKET_CS_ATTACK1:

			Attack1(_session);

			break;


		case dfPACKET_CS_ATTACK2:

			Attack2(_session);

			break;
			
		case dfPACKET_CS_ATTACK3:

			Attack3(_session);

			break;


		case dfPACKET_CS_SYNC:
			SyncCount++;
			Sync(_session);
			break;

		case dfPACKET_CS_ECHO:
			Echo(_session);

			break;



		default:

			break;
		}
	}

		return true;

}


bool DeleteSession(Session* _session)
{
	int sectorX = _session->_player->_x / SECTOR_RATIO;
	int sectorY = _session->_player->_y / SECTOR_RATIO;
	MsgSectorBroadCasting(SendDeleteMessage, (char*)_session, sectorX, sectorY, true);
	_session->_delete = true;
	DeleteArr.push_back(_session); // 지연 삭제를 위해 DeleteArr에 보관


	return true;
}




bool Player::Move() {
	if (_move == false) return false;

	if (_x >= dfRANGE_MOVE_RIGHT || _x < dfRANGE_MOVE_LEFT || _y >= dfRANGE_MOVE_BOTTOM || _y < dfRANGE_MOVE_TOP) return false;


	int oldX = _x;
	int oldY = _y;

	
	switch (_direction) {
	case dfPACKET_MOVE_DIR_LL:
	{
		if (_x - dfSPEED_PLAYER_X < dfRANGE_MOVE_LEFT) return false;
		_x -= dfSPEED_PLAYER_X;;
	}
	break;

	case dfPACKET_MOVE_DIR_LU:
	{
		if (_x - dfSPEED_PLAYER_X < dfRANGE_MOVE_LEFT || _y - dfSPEED_PLAYER_Y < dfRANGE_MOVE_TOP) return false;
		_x -= dfSPEED_PLAYER_X;
		_y -= dfSPEED_PLAYER_Y;
	}

	break;

	case dfPACKET_MOVE_DIR_UU:
	{
		if (_y - dfSPEED_PLAYER_Y < dfRANGE_MOVE_TOP) return false;
		_y -= dfSPEED_PLAYER_Y;
	}

	break;

	case dfPACKET_MOVE_DIR_RU:
	{
		if (_x + dfSPEED_PLAYER_X >= dfRANGE_MOVE_RIGHT || _y - dfSPEED_PLAYER_Y < dfRANGE_MOVE_TOP) return false;
		_x += dfSPEED_PLAYER_X;
		_y -= dfSPEED_PLAYER_Y;
	}
	break;

	case dfPACKET_MOVE_DIR_RR:
	{
		if (_x + dfSPEED_PLAYER_X >= dfRANGE_MOVE_RIGHT) return false;
		_x += dfSPEED_PLAYER_X;
		
	}
	break;

	case dfPACKET_MOVE_DIR_RD:
	{
		if (_x + dfSPEED_PLAYER_X >= dfRANGE_MOVE_RIGHT || _y + dfSPEED_PLAYER_Y >= dfRANGE_MOVE_BOTTOM) return false;
		_x += dfSPEED_PLAYER_X;
		_y += dfSPEED_PLAYER_Y;
	
	}
	break;

	case dfPACKET_MOVE_DIR_DD:
	{
		if (_y + dfSPEED_PLAYER_Y >= dfRANGE_MOVE_BOTTOM) return false;
		_y += dfSPEED_PLAYER_Y;
	
	}
	break;

	case dfPACKET_MOVE_DIR_LD:
	{
		if (_x - dfSPEED_PLAYER_X < dfRANGE_MOVE_LEFT || _y + dfSPEED_PLAYER_Y >= dfRANGE_MOVE_BOTTOM) return false;
		_x -= dfSPEED_PLAYER_X;
		_y += dfSPEED_PLAYER_Y;
	
	}
	break;

defalut:

	break;
	}

	
	//Todo//이동된 섹터에 따른 캐릭터 생성과 삭제 작업 필요
	
	if (_x / SECTOR_RATIO == oldX / SECTOR_RATIO && _y / SECTOR_RATIO == oldY / SECTOR_RATIO) 
		return true;


	int oldSectorX = oldX / SECTOR_RATIO;
	int oldSectorY = oldY / SECTOR_RATIO;

	int newSectorX = _x / SECTOR_RATIO;
	int newSectorY = _y / SECTOR_RATIO;

	Sector[oldSectorX][oldSectorY].remove(pSession);
	Sector[newSectorX][newSectorY].push_back(pSession);
	
	int paramX;
	int paramY;

	if (newSectorX > oldSectorX) //오른쪽으로 간 상황
	{
		paramX = oldX + dfSPEED_PLAYER_X;
		paramY = oldY;
		MoveSectorR(pSession, paramX, paramY, oldX, oldY);
		oldX = paramX;
		oldY = paramY;
		
	}
	else if (oldSectorX > newSectorX) //왼쪽으로 간 상황
	{
		paramX = oldX - dfSPEED_PLAYER_X;
		paramY = oldY;
		MoveSectorL(pSession, paramX, paramY, oldX, oldY);
		oldX = paramX;
		oldY = paramY;
		
		
	}
	if (newSectorY > oldSectorY) //아래로 간 상황
	{
		paramX = oldX;
		paramY = oldY + dfSPEED_PLAYER_Y;
		MoveSectorD(pSession, paramX, paramY, oldX, oldY);
		oldX = paramX;
		oldY = paramY;
		
	}
	else if (oldSectorY > newSectorY) //위로 간 상황
	{
		paramX = oldX;
		paramY = oldY - dfSPEED_PLAYER_Y;

		MoveSectorU(pSession, paramX, paramY, oldX, oldY);
		oldX = paramX;
		oldY = paramY;

	}

	
	return true;
}



bool Player::MoveStart(BYTE Direction, short X, short Y) {

	int oldSectorX = _x / SECTOR_RATIO;
	int oldSectorY = _y / SECTOR_RATIO;
	int newSectorX = X / SECTOR_RATIO;
	int newSectorY = Y / SECTOR_RATIO;



	_direction = Direction;
	_x = X;
	_y = Y;
	_move = true;

	if (oldSectorX == newSectorX && oldSectorY == newSectorY) return true;

	//섹터 동기화 작업

	//SyncSector(pSession, oldSectorX, oldSectorY, newSectorX, newSectorY);


	

	return true;
}



void Player::MoveStop(int Dir, int x, int y)
{

	
	int sumX = x - _x;
	int sumY = y - _y;

	if (sumX < 0) sumX *= -1;
	if (sumY < 0) sumY *= -1;

	_direction = Dir;

	_move = false;


	if (sumX < dfERROR_RANGE && sumY < dfERROR_RANGE) return;

	//섹터 동기화 작업
	
	
	SyncPos(pSession, _x, _y, x, y);

}

void FlushDeleteArr()
{
	if (DeleteArr.size() > 0)
	{
		int arrSize = DeleteArr.size();
		for (int arrIdex = 0; arrIdex < arrSize; arrIdex++)
		{
			Sector[DeleteArr[arrIdex]->_player->_x / SECTOR_RATIO][DeleteArr[arrIdex]->_player->_y / SECTOR_RATIO].
				remove(DeleteArr[arrIdex]);
			SessionArr.remove(DeleteArr[arrIdex]);

			//SendDeleteMessage 해야함


			closesocket(DeleteArr[arrIdex]->_socket);
			delete DeleteArr[arrIdex];
		}
		DeleteArr.clear();
		playerIdex -= arrSize;
	}
}

void TimeOutCheck()
{
	int deadLine = timeGetTime() - dfNETWORK_PACKET_RECV_TIMEOUT;

	s_ArrIt = SessionArr.begin();
	s_ArrIt++;
	for (; s_ArrIt != SessionArr.end(); s_ArrIt++)
	{
		if ((*s_ArrIt)->_timeout < deadLine)
		{
			DeleteSession(*s_ArrIt);
			timeOutCount++;
		}
	}

}