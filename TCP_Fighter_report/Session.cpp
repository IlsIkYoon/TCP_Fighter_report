#include "Session.h"
#include "GameProc.h"


//전역변수
//Session SessionArr[SELECTCOUNT][SELECTDEFINE];
std::vector<Session> SessionArr;

DWORD playerIdex = 0; //63명까지 접속을 받는 상황으로 가정

std::vector<Session*> DeleteArr; 


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
	CreatePacket.ID = playerIdex;

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


bool DecodePacket(Session* _session) 
{
	unsigned int peekResult;
	PacketHeader pHeader;
	_session->_recvQ.Peek((char*)&pHeader, sizeof(pHeader), &peekResult);

	if(peekResult < sizeof(pHeader))
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
\
	case dfPACKET_CS_ATTACK3:

		Attack3(_session);

		break;


	case dfPACKET_CS_SYNC:
		//Todo//CS SYnc메세지를 받으면 그 해당 캐릭터 좌표를 서버에서 입력하고 섹터에 캐릭터 좌표를 SC_SYNC로 뿌려야함
		Sync(_session);
		break;

	





	default:

		break;
	}










}


bool DeleteSession(Session* _session)
{
	//지연 삭제를 안하고 있음 일단 // 세션을 아예 벡터에서 보관하는 중 // 메모리 상에서 나열 시키려는 의도임
	_session->_delete = true;
	Sector[_session->_player->_x / SECTOR_RATIO][_session->_player->_y / SECTOR_RATIO].remove(_session);
	//Sector에서 삭제

	std::vector<Session>::iterator it;
	it = std::remove(SessionArr.begin(), SessionArr.end(), *_session);
	SessionArr.erase(it);

	playerIdex--;


	return true;
}