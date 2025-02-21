#include "Session.h"
#include "GameProc.h"
#include "Sector.h"
#include "Message.h"
#include "Log.h"
#include <format>

//전역변수
std::list<Session*> SessionArr;
std::list<Session*>::iterator s_ArrIt;

DWORD timeOutCount = 0;
DWORD playerCount = 0; //63명까지 접속을 받는 상황으로 가정

std::vector<Session*> DeleteArr; 
extern CRITICAL_SECTION g_lock;

int playerID = 1;

extern DWORD SyncMessageCount;

extern std::list<Session*> Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];
extern Session* pSector;

extern std::list<std::string> LogQ;


bool CreateNewCharacter(Session* _session) {

	int sectorX = _session->_player->_x / SECTOR_RATIO;
	int sectorY = _session->_player->_y / SECTOR_RATIO;

	SendCreateMyCharMessage((char*)_session);

	SendCreateSurroundCharMessage((char*)_session);

	MsgSectorBroadCasting(SendCreateOtherCharMessage, (char*)_session, sectorX, sectorY, false);

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
			EnqueLog("Header ", __FILE__, __func__, __LINE__, GetLastError());
			return false;
		}


		if (pHeader.byCode != 0x89)
		{
			EnqueLog("Protocol Code ", __FILE__, __func__, __LINE__, GetLastError());
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
		{
			EnqueLog("SyncMessage ", __FILE__, __func__, __LINE__, GetLastError());
		}
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




bool Player::Move(DWORD fixedDeltaTime) {
	if (_move == false) return false;

	if (_x >= dfRANGE_MOVE_RIGHT || _x < dfRANGE_MOVE_LEFT || _y >= dfRANGE_MOVE_BOTTOM || _y < dfRANGE_MOVE_TOP) return false;


	short deltaX;
	short deltaY;
	int oldX = _x;
	int oldY = _y;

	deltaX = ((short)fixedDeltaTime / FrameSec) * dfSPEED_PLAYER_X;
	deltaY = ((short)fixedDeltaTime / FrameSec) * dfSPEED_PLAYER_Y;


	
	switch (_direction) {
	case dfPACKET_MOVE_DIR_LL:
	{
		if (_x - deltaX < dfRANGE_MOVE_LEFT) return false;
		_x -= deltaX;;
		

	}
	break;

	case dfPACKET_MOVE_DIR_LU:
	{
		if (_x - deltaX < dfRANGE_MOVE_LEFT || _y - deltaY < dfRANGE_MOVE_TOP) return false;
		_x -= deltaX;
		_y -= deltaY;

		

	}

	break;

	case dfPACKET_MOVE_DIR_UU:
	{
		if (_y - deltaY < dfRANGE_MOVE_TOP) return false;
		_y -= deltaY;

		
	}

	break;

	case dfPACKET_MOVE_DIR_RU:
	{
		if (_x + deltaX >= dfRANGE_MOVE_RIGHT || _y - deltaY < dfRANGE_MOVE_TOP) return false;
		_x += deltaX;
		_y -= deltaY;

		
	}
	break;

	case dfPACKET_MOVE_DIR_RR:
	{
		if (_x + deltaX >= dfRANGE_MOVE_RIGHT) return false;
		_x += deltaX;
		
	
	}
	break;

	case dfPACKET_MOVE_DIR_RD:
	{
		if (_x + deltaX >= dfRANGE_MOVE_RIGHT || _y + deltaY >= dfRANGE_MOVE_BOTTOM) return false;
		_x += deltaX;
		_y += deltaY;
	
		
	}
	break;

	case dfPACKET_MOVE_DIR_DD:
	{
		if (_y + deltaY >= dfRANGE_MOVE_BOTTOM) return false;
		_y += deltaY;
	
		
	}
	break;

	case dfPACKET_MOVE_DIR_LD:
	{
		if (_x - deltaX < dfRANGE_MOVE_LEFT || _y + deltaY >= dfRANGE_MOVE_BOTTOM) return false;
		_x -= deltaX;
		_y += deltaY;
	
		
	}
	break;
	default :
	
		std::string logEntry = std::format("Move dir Error || dir : {} FILE : {}, Func : {} , Line : {} error : {}\n", _direction,
			getFileName(__FILE__), __func__, __LINE__, GetLastError());
		EnterCriticalSection(&g_lock);
		LogQ.push_back(logEntry);
		LeaveCriticalSection(&g_lock);
	break;
	}


	
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
		paramX = oldX + deltaX;
		paramY = oldY;
		MoveSectorR(pSession, paramX, paramY, oldX, oldY);
		oldX = paramX;
		oldY = paramY;
		
	}
	else if (oldSectorX > newSectorX) //왼쪽으로 간 상황
	{
		paramX = oldX - deltaX;
		paramY = oldY;
		MoveSectorL(pSession, paramX, paramY, oldX, oldY);
		oldX = paramX;
		oldY = paramY;
		
		
	}
	if (newSectorY > oldSectorY) //아래로 간 상황
	{
		paramX = oldX;
		paramY = oldY + deltaY;
		MoveSectorD(pSession, paramX, paramY, oldX, oldY);
		oldX = paramX;
		oldY = paramY;
		
	}
	else if (oldSectorY > newSectorY) //위로 간 상황
	{
		paramX = oldX;
		paramY = oldY - deltaY;

		MoveSectorU(pSession, paramX, paramY, oldX, oldY);
		oldX = paramX;
		oldY = paramY;

	}

	
	return true;
}



bool Player::MoveStart(BYTE Direction, short X, short Y) {

	_direction = Direction;

	_move = true;
	
	return true;
}



void Player::MoveStop(int Dir, int x, int y)
{

	_direction = Dir;

	_move = false;

}

void FlushDeleteArr()
{
	int sectorX;
	int sectorY;
	if (DeleteArr.size() > 0)
	{
		int arrSize = (int)DeleteArr.size();
		for (int arrIdex = 0; arrIdex < arrSize; arrIdex++)
		{
			sectorX = DeleteArr[arrIdex]->_player->_x / SECTOR_RATIO;
			sectorY = DeleteArr[arrIdex]->_player->_y / SECTOR_RATIO;
			int debugSize = (int)Sector[sectorX][sectorY].size();
			
			Sector[sectorX][sectorY].remove(DeleteArr[arrIdex]);

			if (debugSize == Sector[sectorX][sectorY].size())
			{

				EnqueLog("Sector ", __FILE__, __func__, __LINE__, GetLastError());
			}


			SessionArr.remove(DeleteArr[arrIdex]);


			closesocket(DeleteArr[arrIdex]->_socket);
			delete DeleteArr[arrIdex];
		}
		DeleteArr.clear();
		playerCount -= arrSize;
	}
}

void TimeOutCheck()
{
	DWORD deadLine = timeGetTime() - dfNETWORK_PACKET_RECV_TIMEOUT;


	for (auto session : SessionArr)
	{
		if (session->_timeout < deadLine)
		{
			DeleteSession(session);
			timeOutCount++;
		}
	}
}