#include "Session.h"
#include "GameProc.h"

//전역변수
Session SessionArr[PLAYERMAXCOUNT];

DWORD playerCount = 0; //63명까지 접속을 받는 상황으로 가정


bool CreateNewCharacter(Session* _session) {

	PacketHeader pHeader;
	SC_CREATE_MY_CHARACTER CreatePacket;
	SC_CREATE_OTHER_CHARACTER other_myCharacter;
	SC_CREATE_OTHER_CHARACTER otherCharacter;


	int ringbuffer_retval;


	//내 캐릭터 생성
	pHeader.byCode = 0x89;
	pHeader.bySize = sizeof(SC_CREATE_MY_CHARACTER); //12
	pHeader.byType = dfPACKET_SC_CREATE_MY_CHARACTER;
	
	CreatePacket.Direction = _session->_player->_direction;
	CreatePacket.HP = _session->_player->_hp;
	CreatePacket.X = _session->_player->_x;
	CreatePacket.Y = _session->_player->_y;
	CreatePacket.ID = playerCount;

	if (_session->_sendQ.GetBufferFree() <= pHeader.bySize + sizeof(pHeader)) 
	{
		printf("Line : %d, Send buffer Full\n", __LINE__);
		return false;
	}
		


	if (_session->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader)) == false) 
	{
		printf("Line : %d, ringbuffer send error : %d\n", __LINE__, GetLastError());
		return false;
	}
		
	

	if (_session->_sendQ.Enqueue((char*)&CreatePacket, pHeader.bySize) == false)
	{
		printf("Line : %d, ringbuffer send error : %d\n", __LINE__, GetLastError());
		return false;
	}


	//----------------------------------------------//

	//다른 캐릭터에게 내 캐릭터 보내기//
	other_myCharacter.Direction = _session->_player->_direction;
	other_myCharacter.HP = _session->_player->_hp;
	other_myCharacter.ID = playerCount;
	other_myCharacter.X = _session->_player->_x;
	other_myCharacter.Y = _session->_player->_y;




	//다른 캐릭터 생성
	for (int i = 0; i < playerCount; i++) 
	{

		pHeader.byCode = 0x89;
		pHeader.bySize = sizeof(other_myCharacter);
		pHeader.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;
		
		if (SessionArr[i]._sendQ.Enqueue((char*)&pHeader, sizeof(pHeader)) == false)
		{
			printf("Line : %d, Enqueue error : %d\n", __LINE__, GetLastError());
			return false;
		}
		
		if (SessionArr[i]._sendQ.Enqueue((char*)&other_myCharacter, sizeof(other_myCharacter)) == false)
		{
			printf("Line : %d, Enqueue error : %d\n", __LINE__, GetLastError());
			return false;
		}





		pHeader.byCode = 0x89;
		pHeader.bySize = sizeof(otherCharacter);
		pHeader.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;

		if (_session->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader)) == false)
		{
			printf("Line : %d, ringbuffer send error : %d\n", __LINE__, GetLastError());
			return false;
		}


		otherCharacter.Direction = SessionArr[i]._player->_direction;
		otherCharacter.HP = SessionArr[i]._player->_hp;
		otherCharacter.X = SessionArr[i]._player->_x;
		otherCharacter.Y = SessionArr[i]._player->_y;
		otherCharacter.ID = i;
		
		if (_session->_sendQ.Enqueue((char*)&otherCharacter, sizeof(otherCharacter)) == false)
		{
			printf("Line : %d, ringbuffer send error : %d\n", __LINE__, GetLastError());
			return false;
		}
	}











	return true;
}


bool DecodePacket(Session* _session) 
{

	//peek이 잘 작동하고 있는지도 확인 필요 
	PacketHeader pHeader;
	if (_session->_recvQ.Peek((char*) & pHeader, sizeof(pHeader)) < sizeof(pHeader))
	{
		printf("Line : %d, header error\n", __LINE__); //헤더보다 작은 양의 데이터가 남아 있는 경우
		return false;
	}


	if (pHeader.byCode != 0x89)
	{
		printf("protocol code error : %d\n", pHeader.byCode);
		closesocket(_session->_socket); //맞는 코드가 아니라면 소켓 연결 종료
		return false;
	}
	
	_session->_recvQ.MoveFront(sizeof(pHeader));

	switch (pHeader.byType)
	{
		/*
	case dfPACKET_SC_CREATE_MY_CHARACTER:



		break;

	case dfPACKET_SC_CREATE_OTHER_CHARACTER:

		break;
		*/


	case dfPACKET_CS_MOVE_START:

		MoveStart(_session);


		break;

	case dfPACKET_CS_MOVE_STOP:
		MoveStop(_session);

		break;

	case dfPACKET_SC_MOVE_STOP:

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

	case dfPACKET_SC_ATTACK3:

		break;

	case dfPACKET_SC_DAMAGE:

		break;


	case dfPACKET_CS_SYNC:
		
		break;

	case dfPACKET_SC_SYNC:

		break;

	default:

		break;
	}










}