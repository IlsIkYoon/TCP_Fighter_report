#include "Sector.h"


std::list<Session*> Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];
extern std::list<Session*> SessionArr;
extern std::list<Session*>::iterator s_ArrIt;


bool MoveSectorL(Session* _session, int x, int y, int oldX, int oldY)
{
	unsigned int enqueResult;
	bool sendResult;
	// 이전 섹터에 내 세션 딜리트 메세지 보내주기//
	for (int i = -1; i < 2; i++)
	{
		if ((oldX + 1) / SECTOR_RATIO < 0 || (oldX + 1) / SECTOR_RATIO > dfRANGE_MOVE_RIGHT / SECTOR_RATIO) break;
		if ((oldY + i) / SECTOR_RATIO < 0 || (oldY + i) / SECTOR_RATIO > dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


		for (s_ArrIt = Sector[oldX + 1 / SECTOR_RATIO][oldY + i / SECTOR_RATIO].begin();
			s_ArrIt != Sector[oldX + 1 / SECTOR_RATIO][oldY + i / SECTOR_RATIO].end(); s_ArrIt++)
		{
			PacketHeader pHeader;
			SC_DELETE_CHARACTER deleteMsg;
			pHeader.byCode = 0x89;
			pHeader.bySize = sizeof(deleteMsg);
			pHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

			deleteMsg.ID = _session->_player->_ID;

			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&deleteMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
		}
	}
	//새로운 섹터에 내 캐릭터 생성 메세지 + 새로운 섹터 캐릭터 생성 메세지를 나에게 보내기
			PacketHeader pHeader;
			SC_CREATE_OTHER_CHARACTER createMyMsg;
			SC_CREATE_OTHER_CHARACTER createOtherMsg;

			pHeader.byCode = 0x89;
			pHeader.bySize = sizeof(createMyMsg);
			pHeader.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;

			createMyMsg.Direction = _session->_player->_direction;
			createMyMsg.HP = _session->_player->_hp;
			createMyMsg.ID = _session->_player->_ID;
			createMyMsg.X = _session->_player->_x;
			createMyMsg.Y = _session->_player->_y;
	for (int i = -1; i < 2; i++)
	{
		if ((x - 1) / SECTOR_RATIO < 0 || (x - 1) / SECTOR_RATIO > dfRANGE_MOVE_RIGHT / SECTOR_RATIO) break;
		if ((oldY + i) / SECTOR_RATIO < 0 || (oldY + i) / SECTOR_RATIO > dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;

		for (s_ArrIt = Sector[x - 1 / SECTOR_RATIO][oldY + i / SECTOR_RATIO].begin();
			s_ArrIt != Sector[x - 1 / SECTOR_RATIO][oldY + i / SECTOR_RATIO].end(); s_ArrIt++)
		{
			createOtherMsg.Direction = (*s_ArrIt)->_player->_direction;
			createOtherMsg.HP = (*s_ArrIt)->_player->_hp;
			createOtherMsg.ID = (*s_ArrIt)->_player->_ID;
			createOtherMsg.X = (*s_ArrIt)->_player->_x;
			createOtherMsg.Y = (*s_ArrIt)->_player->_y;

			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if(sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&createMyMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = _session->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			sendResult = _session->_sendQ.Enqueue((char*)&createOtherMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);


		}
	}
	

	return true;
}
bool MoveSectorR(Session* _session, int x, int y, int oldX, int oldY)
{
	unsigned int enqueResult;
	bool sendResult;
	// 이전 섹터에 내 세션 딜리트 메세지 보내주기//
	for (int i = -1; i < 2; i++)
	{
		if ((oldX - 1) / SECTOR_RATIO < 0 || (oldX - 1) / SECTOR_RATIO > dfRANGE_MOVE_RIGHT / SECTOR_RATIO) break;
		if ((oldY + i) / SECTOR_RATIO < 0 || (oldY + i) / SECTOR_RATIO > dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


		for (s_ArrIt = Sector[oldX - 1 / SECTOR_RATIO][oldY + i / SECTOR_RATIO].begin();
			s_ArrIt != Sector[oldX - 1 / SECTOR_RATIO][oldY + i / SECTOR_RATIO].end(); s_ArrIt++)
		{
			PacketHeader pHeader;
			SC_DELETE_CHARACTER deleteMsg;
			pHeader.byCode = 0x89;
			pHeader.bySize = sizeof(deleteMsg);
			pHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

			deleteMsg.ID = _session->_player->_ID;

			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&deleteMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
		}
	}
	//새로운 섹터에 내 캐릭터 생성 메세지 + 새로운 섹터 캐릭터 생성 메세지를 나에게 보내기
	PacketHeader pHeader;
	SC_CREATE_OTHER_CHARACTER createMyMsg;
	SC_CREATE_OTHER_CHARACTER createOtherMsg;

	pHeader.byCode = 0x89;
	pHeader.bySize = sizeof(createMyMsg);
	pHeader.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;

	createMyMsg.Direction = _session->_player->_direction;
	createMyMsg.HP = _session->_player->_hp;
	createMyMsg.ID = _session->_player->_ID;
	createMyMsg.X = _session->_player->_x;
	createMyMsg.Y = _session->_player->_y;
	for (int i = -1; i < 2; i++)
	{
		if ((x + 1) / SECTOR_RATIO < 0 || (x + 1) / SECTOR_RATIO > dfRANGE_MOVE_RIGHT / SECTOR_RATIO) break;
		if ((oldY + i) / SECTOR_RATIO < 0 || (oldY + i) / SECTOR_RATIO > dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;

		for (s_ArrIt = Sector[x + 1 / SECTOR_RATIO][oldY + i / SECTOR_RATIO].begin();
			s_ArrIt != Sector[x + 1 / SECTOR_RATIO][oldY + i / SECTOR_RATIO].end(); s_ArrIt++)
		{
			createOtherMsg.Direction = (*s_ArrIt)->_player->_direction;
			createOtherMsg.HP = (*s_ArrIt)->_player->_hp;
			createOtherMsg.ID = (*s_ArrIt)->_player->_ID;
			createOtherMsg.X = (*s_ArrIt)->_player->_x;
			createOtherMsg.Y = (*s_ArrIt)->_player->_y;

			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&createMyMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = _session->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			sendResult = _session->_sendQ.Enqueue((char*)&createOtherMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);


		}
	}


	return true;
}
bool MoveSectorU(Session* _session, int x, int y, int oldX, int oldY)
{
	unsigned int enqueResult;
	bool sendResult;
	// 이전 섹터에 내 세션 딜리트 메세지 보내주기//
	for (int i = -1; i < 2; i++)
	{
		if ((oldY + 1) / SECTOR_RATIO < 0 || (oldY + 1) / SECTOR_RATIO > dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) break;
		if ((oldX + i) / SECTOR_RATIO < 0 || (oldX + i) / SECTOR_RATIO > dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;


		for (s_ArrIt = Sector[oldX + i / SECTOR_RATIO][oldY + 1 / SECTOR_RATIO].begin();
			s_ArrIt != Sector[oldX + i / SECTOR_RATIO][oldY + 1 / SECTOR_RATIO].end(); s_ArrIt++)
		{
			PacketHeader pHeader;
			SC_DELETE_CHARACTER deleteMsg;
			pHeader.byCode = 0x89;
			pHeader.bySize = sizeof(deleteMsg);
			pHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

			deleteMsg.ID = _session->_player->_ID;

			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&deleteMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
		}
	}
	//새로운 섹터에 내 캐릭터 생성 메세지 + 새로운 섹터 캐릭터 생성 메세지를 나에게 보내기
	PacketHeader pHeader;
	SC_CREATE_OTHER_CHARACTER createMyMsg;
	SC_CREATE_OTHER_CHARACTER createOtherMsg;

	pHeader.byCode = 0x89;
	pHeader.bySize = sizeof(createMyMsg);
	pHeader.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;

	createMyMsg.Direction = _session->_player->_direction;
	createMyMsg.HP = _session->_player->_hp;
	createMyMsg.ID = _session->_player->_ID;
	createMyMsg.X = _session->_player->_x;
	createMyMsg.Y = _session->_player->_y;
	for (int i = -1; i < 2; i++)
	{
		if ((oldY -1 ) / SECTOR_RATIO < 0 || (oldY - 1) / SECTOR_RATIO > dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) break;
		if ((x + i) / SECTOR_RATIO < 0 || (x + i) / SECTOR_RATIO > dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;

		for (s_ArrIt = Sector[x + i / SECTOR_RATIO][oldY - 1 / SECTOR_RATIO].begin();
			s_ArrIt != Sector[x + i / SECTOR_RATIO][oldY - 1 / SECTOR_RATIO].end(); s_ArrIt++)
		{
			createOtherMsg.Direction = (*s_ArrIt)->_player->_direction;
			createOtherMsg.HP = (*s_ArrIt)->_player->_hp;
			createOtherMsg.ID = (*s_ArrIt)->_player->_ID;
			createOtherMsg.X = (*s_ArrIt)->_player->_x;
			createOtherMsg.Y = (*s_ArrIt)->_player->_y;

			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&createMyMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = _session->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			sendResult = _session->_sendQ.Enqueue((char*)&createOtherMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);


		}
	}


	return true;
}
bool MoveSectorD(Session* _session, int x, int y, int oldX, int oldY)
{
	unsigned int enqueResult;
	bool sendResult;
	// 이전 섹터에 내 세션 딜리트 메세지 보내주기//
	for (int i = -1; i < 2; i++)
	{
		if ((oldY - 1) / SECTOR_RATIO < 0 || (oldY - 1) / SECTOR_RATIO > dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) break;
		if ((oldX + i) / SECTOR_RATIO < 0 || (oldX + i) / SECTOR_RATIO > dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;


		for (s_ArrIt = Sector[oldX + i / SECTOR_RATIO][oldY - 1 / SECTOR_RATIO].begin();
			s_ArrIt != Sector[oldX + i / SECTOR_RATIO][oldY - 1 / SECTOR_RATIO].end(); s_ArrIt++)
		{
			PacketHeader pHeader;
			SC_DELETE_CHARACTER deleteMsg;
			pHeader.byCode = 0x89;
			pHeader.bySize = sizeof(deleteMsg);
			pHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

			deleteMsg.ID = _session->_player->_ID;

			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&deleteMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
		}
	}
	//새로운 섹터에 내 캐릭터 생성 메세지 + 새로운 섹터 캐릭터 생성 메세지를 나에게 보내기
	PacketHeader pHeader;
	SC_CREATE_OTHER_CHARACTER createMyMsg;
	SC_CREATE_OTHER_CHARACTER createOtherMsg;

	pHeader.byCode = 0x89;
	pHeader.bySize = sizeof(createMyMsg);
	pHeader.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;

	createMyMsg.Direction = _session->_player->_direction;
	createMyMsg.HP = _session->_player->_hp;
	createMyMsg.ID = _session->_player->_ID;
	createMyMsg.X = _session->_player->_x;
	createMyMsg.Y = _session->_player->_y;
	for (int i = -1; i < 2; i++)
	{
		if ((oldY + 1) / SECTOR_RATIO < 0 || (oldY + 1) / SECTOR_RATIO > dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) break;
		if ((x + i) / SECTOR_RATIO < 0 || (x + i) / SECTOR_RATIO > dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;

		for (s_ArrIt = Sector[x + i / SECTOR_RATIO][oldY + 1 / SECTOR_RATIO].begin();
			s_ArrIt != Sector[x + i / SECTOR_RATIO][oldY + 1 / SECTOR_RATIO].end(); s_ArrIt++)
		{
			createOtherMsg.Direction = (*s_ArrIt)->_player->_direction;
			createOtherMsg.HP = (*s_ArrIt)->_player->_hp;
			createOtherMsg.ID = (*s_ArrIt)->_player->_ID;
			createOtherMsg.X = (*s_ArrIt)->_player->_x;
			createOtherMsg.Y = (*s_ArrIt)->_player->_y;

			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = (*s_ArrIt)->_sendQ.Enqueue((char*)&createMyMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = _session->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			sendResult = _session->_sendQ.Enqueue((char*)&createOtherMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);


		}
	}


	return true;
}
