#include "Sector.h"


std::list<Session*> Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];
extern std::list<Session*> SessionArr;
static std::list<Session*>::iterator stat_ArrIt;
//Todo // 대각선 방향에 대한 중복 예외 처리를 해줘야 함 


bool MoveSectorL(Session* _session, int x, int y, int oldX, int oldY)
{
	unsigned int enqueResult;
	bool sendResult;
	int oldSectorX;
	int oldSectorY;

	int newSectorX;
	int newSectorY;

	int sectorRightMax = dfRANGE_MOVE_RIGHT / SECTOR_RATIO;
	int sectorBottomMax = dfRANGE_MOVE_BOTTOM / SECTOR_RATIO;

	// 이전 섹터에 내 세션 딜리트 메세지 보내주기//
	for (int i = -1; i < 2; i++)
	{
		oldSectorX = oldX / SECTOR_RATIO + 1;
		oldSectorY = oldY / SECTOR_RATIO + i;

		if (oldSectorX < 0 || oldSectorX > sectorRightMax) break;
		if (oldSectorY < 0 || oldSectorX > sectorBottomMax) continue;


		for (stat_ArrIt = Sector[oldSectorX][oldSectorY].begin();
			stat_ArrIt != Sector[oldSectorX][oldSectorY].end(); stat_ArrIt++)
		{
			if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;


			PacketHeader pHeader;
			SC_DELETE_CHARACTER deleteMsg;
			pHeader.byCode = 0x89;
			pHeader.bySize = sizeof(deleteMsg);
			pHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

			deleteMsg.ID = _session->_player->_ID;

			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&deleteMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}

			//나에게 이전 섹터 딜리트 메세지 보내기
			PacketHeader pMyHeader;
			SC_DELETE_CHARACTER myDeleteMsg;
			pMyHeader.byCode = 0x89;
			pMyHeader.bySize = sizeof(myDeleteMsg);
			pMyHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

			myDeleteMsg.ID = (*stat_ArrIt)->_player->_ID;

			sendResult = _session->_sendQ.Enqueue((char*)&pMyHeader, sizeof(pMyHeader), &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			sendResult = _session->_sendQ.Enqueue((char*)&myDeleteMsg, pMyHeader.bySize, &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			printf("Send Delete Message || dest : %d, target : %d\n", _session->_player->_ID, myDeleteMsg.ID);

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
		newSectorX = x / SECTOR_RATIO - 1;
		newSectorY = y / SECTOR_RATIO + i;


		if (newSectorX< 0 || newSectorX> sectorRightMax) break;
		if (newSectorY< 0 || newSectorY> sectorBottomMax) continue;

		for (stat_ArrIt = Sector[newSectorX][newSectorY].begin();
			stat_ArrIt != Sector[newSectorX][newSectorY].end(); stat_ArrIt++)
		{
			if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;

			createOtherMsg.Direction = (*stat_ArrIt)->_player->_direction;
			createOtherMsg.HP = (*stat_ArrIt)->_player->_hp;
			createOtherMsg.ID = (*stat_ArrIt)->_player->_ID;
			createOtherMsg.X = (*stat_ArrIt)->_player->_x;
			createOtherMsg.Y = (*stat_ArrIt)->_player->_y;

			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if(sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&createMyMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = _session->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			sendResult = _session->_sendQ.Enqueue((char*)&createOtherMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);



			//MoveMessage 보내야 함 
			if (_session->_player->_move == true)
			{
				//다른 사람한테 내가 움직인다는 메세지를 보내야 함
				PacketHeader MoveHeader;
				SC_MOVE_START MovePacket;
				MoveHeader.byCode = 0x89;
				MoveHeader.bySize = sizeof(MovePacket);
				MoveHeader.byType = dfPACKET_SC_MOVE_START;

				MovePacket.Direction = _session->_player->_direction;
				MovePacket.ID = _session->_player->_ID;
				MovePacket.X = _session->_player->_x;
				MovePacket.Y = _session->_player->_y;

				sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&MoveHeader, sizeof(MoveHeader), &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

				sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&MovePacket, MoveHeader.bySize, &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			}
			if ((*stat_ArrIt)->_player->_move == true)
			{
				//나에게 다른 사람이 움직인다는 메세지를 보내야 함 
				PacketHeader MoveHeader;
				SC_MOVE_START MovePacket;
				MoveHeader.byCode = 0x89;
				MoveHeader.bySize = sizeof(MovePacket);
				MoveHeader.byType = dfPACKET_SC_MOVE_START;

				MovePacket.Direction = (*stat_ArrIt)->_player->_direction;
				MovePacket.ID = (*stat_ArrIt)->_player->_ID;
				MovePacket.X = (*stat_ArrIt)->_player->_x;
				MovePacket.Y = (*stat_ArrIt)->_player->_y;

				sendResult = _session->_sendQ.Enqueue((char*)&MoveHeader, sizeof(MoveHeader), &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
				sendResult = _session->_sendQ.Enqueue((char*)&MovePacket, MoveHeader.bySize, &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}

		}
	}
	

	return true;
}
bool MoveSectorR(Session* _session, int x, int y, int oldX, int oldY)
{
	unsigned int enqueResult;
	bool sendResult;

	int oldSectorX;
	int oldSectorY;

	int newSectorX;
	int newSectorY;

	int sectorRightMax = dfRANGE_MOVE_RIGHT / SECTOR_RATIO;
	int sectorBottomMax = dfRANGE_MOVE_BOTTOM / SECTOR_RATIO;

	// 이전 섹터에 내 세션 딜리트 메세지 보내주기//
	for (int i = -1; i < 2; i++)
	{
		oldSectorX = oldX / SECTOR_RATIO - 1;
		oldSectorY = oldY / SECTOR_RATIO + i;

		if (oldSectorX< 0 || oldSectorX> sectorRightMax) break;
		if (oldSectorY< 0 || oldSectorY> sectorBottomMax) continue;


		for (stat_ArrIt = Sector[oldSectorX][oldSectorY].begin();
			stat_ArrIt != Sector[oldSectorX][oldSectorY].end(); stat_ArrIt++)
		{
			if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;
			PacketHeader pHeader;
			SC_DELETE_CHARACTER deleteMsg;
			pHeader.byCode = 0x89;
			pHeader.bySize = sizeof(deleteMsg);
			pHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

			deleteMsg.ID = _session->_player->_ID;

			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&deleteMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}

			//나에게 이전 섹터 딜리트 메세지 보내기
			PacketHeader pMyHeader;
			SC_DELETE_CHARACTER myDeleteMsg;
			pMyHeader.byCode = 0x89;
			pMyHeader.bySize = sizeof(myDeleteMsg);
			pMyHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

			myDeleteMsg.ID = (*stat_ArrIt)->_player->_ID;

			sendResult = _session->_sendQ.Enqueue((char*)&pMyHeader, sizeof(pMyHeader), &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			sendResult = _session->_sendQ.Enqueue((char*)&myDeleteMsg, pMyHeader.bySize, &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			printf("Send Delete Message || dest : %d, target : %d\n", _session->_player->_ID, myDeleteMsg.ID);

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
		newSectorX = x / SECTOR_RATIO + 1;
		newSectorY = y / SECTOR_RATIO + i;
		if (newSectorX< 0 || newSectorX> sectorRightMax) break;
		if (newSectorY< 0 || newSectorY> sectorBottomMax) continue;

		for (stat_ArrIt = Sector[newSectorX][newSectorY].begin();
			stat_ArrIt != Sector[newSectorX][newSectorY].end(); stat_ArrIt++)
		{
			if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;
			createOtherMsg.Direction = (*stat_ArrIt)->_player->_direction;
			createOtherMsg.HP = (*stat_ArrIt)->_player->_hp;
			createOtherMsg.ID = (*stat_ArrIt)->_player->_ID;
			createOtherMsg.X = (*stat_ArrIt)->_player->_x;
			createOtherMsg.Y = (*stat_ArrIt)->_player->_y;

			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&createMyMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = _session->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			sendResult = _session->_sendQ.Enqueue((char*)&createOtherMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);


			//MoveMessage 보내야 함 
			if (_session->_player->_move == true)
			{
				//다른 사람한테 내가 움직인다는 메세지를 보내야 함
				PacketHeader MoveHeader;
				SC_MOVE_START MovePacket;
				MoveHeader.byCode = 0x89;
				MoveHeader.bySize = sizeof(MovePacket);
				MoveHeader.byType = dfPACKET_SC_MOVE_START;

				MovePacket.Direction = _session->_player->_direction;
				MovePacket.ID = _session->_player->_ID;
				MovePacket.X = _session->_player->_x;
				MovePacket.Y = _session->_player->_y;

				sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&MoveHeader, sizeof(MoveHeader), &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

				sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&MovePacket, MoveHeader.bySize, &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			}
			if ((*stat_ArrIt)->_player->_move == true)
			{
				//나에게 다른 사람이 움직인다는 메세지를 보내야 함 
				PacketHeader MoveHeader;
				SC_MOVE_START MovePacket;
				MoveHeader.byCode = 0x89;
				MoveHeader.bySize = sizeof(MovePacket);
				MoveHeader.byType = dfPACKET_SC_MOVE_START;

				MovePacket.Direction = (*stat_ArrIt)->_player->_direction;
				MovePacket.ID = (*stat_ArrIt)->_player->_ID;
				MovePacket.X = (*stat_ArrIt)->_player->_x;
				MovePacket.Y = (*stat_ArrIt)->_player->_y;

				sendResult = _session->_sendQ.Enqueue((char*)&MoveHeader, sizeof(MoveHeader), &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
				sendResult = _session->_sendQ.Enqueue((char*)&MovePacket, MoveHeader.bySize, &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}





		}
	}


	return true;
}
bool MoveSectorU(Session* _session, int x, int y, int oldX, int oldY)
{
	unsigned int enqueResult;
	bool sendResult;

	int oldSectorX;
	int oldSectorY;

	int newSectorX;
	int newSectorY;

	int sectorRightMax = dfRANGE_MOVE_RIGHT / SECTOR_RATIO;
	int sectorBottomMax = dfRANGE_MOVE_BOTTOM / SECTOR_RATIO;

	// 이전 섹터에 내 세션 딜리트 메세지 보내주기//
	for (int i = -1; i < 2; i++)
	{
		oldSectorX = oldX / SECTOR_RATIO + i;
		oldSectorY = oldY / SECTOR_RATIO + 1;


		if (oldSectorY< 0 || oldSectorY> sectorBottomMax) break;
		if (oldSectorX< 0 || oldSectorX> sectorRightMax) continue;


		for (stat_ArrIt = Sector[oldSectorX][oldSectorY].begin();
			stat_ArrIt != Sector[oldSectorX][oldSectorY].end(); stat_ArrIt++)
		{
			if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;
			PacketHeader pHeader;
			SC_DELETE_CHARACTER deleteMsg;
			pHeader.byCode = 0x89;
			pHeader.bySize = sizeof(deleteMsg);
			pHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

			deleteMsg.ID = _session->_player->_ID;

			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&deleteMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}

			//나에게 이전 섹터 딜리트 메세지 보내기
			PacketHeader pMyHeader;
			SC_DELETE_CHARACTER myDeleteMsg;
			pMyHeader.byCode = 0x89;
			pMyHeader.bySize = sizeof(myDeleteMsg);
			pMyHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

			myDeleteMsg.ID = (*stat_ArrIt)->_player->_ID;

			sendResult = _session->_sendQ.Enqueue((char*)&pMyHeader, sizeof(pMyHeader), &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			sendResult = _session->_sendQ.Enqueue((char*)&myDeleteMsg, pMyHeader.bySize, &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			printf("Send Delete Message || dest : %d, target : %d\n", _session->_player->_ID, myDeleteMsg.ID);

		}

		//나에게도 보내야 함 

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
		newSectorX = x / SECTOR_RATIO + i;
		newSectorY = y / SECTOR_RATIO - 1;

		if (newSectorY|| newSectorY> sectorBottomMax) break;
		if (newSectorX< 0 || newSectorX> sectorRightMax) continue;

		for (stat_ArrIt = Sector[newSectorX][newSectorY].begin();
			stat_ArrIt != Sector[newSectorX][newSectorY].end(); stat_ArrIt++)
		{
			if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;
			createOtherMsg.Direction = (*stat_ArrIt)->_player->_direction;
			createOtherMsg.HP = (*stat_ArrIt)->_player->_hp;
			createOtherMsg.ID = (*stat_ArrIt)->_player->_ID;
			createOtherMsg.X = (*stat_ArrIt)->_player->_x;
			createOtherMsg.Y = (*stat_ArrIt)->_player->_y;

			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&createMyMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = _session->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			sendResult = _session->_sendQ.Enqueue((char*)&createOtherMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);



			//MoveMessage 보내야 함 
			if (_session->_player->_move == true)
			{
				//다른 사람한테 내가 움직인다는 메세지를 보내야 함
				PacketHeader MoveHeader;
				SC_MOVE_START MovePacket;
				MoveHeader.byCode = 0x89;
				MoveHeader.bySize = sizeof(MovePacket);
				MoveHeader.byType = dfPACKET_SC_MOVE_START;

				MovePacket.Direction = _session->_player->_direction;
				MovePacket.ID = _session->_player->_ID;
				MovePacket.X = _session->_player->_x;
				MovePacket.Y = _session->_player->_y;

				sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&MoveHeader, sizeof(MoveHeader), &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

				sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&MovePacket, MoveHeader.bySize, &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			}
			if ((*stat_ArrIt)->_player->_move == true)
			{
				//나에게 다른 사람이 움직인다는 메세지를 보내야 함 
				PacketHeader MoveHeader;
				SC_MOVE_START MovePacket;
				MoveHeader.byCode = 0x89;
				MoveHeader.bySize = sizeof(MovePacket);
				MoveHeader.byType = dfPACKET_SC_MOVE_START;

				MovePacket.Direction = (*stat_ArrIt)->_player->_direction;
				MovePacket.ID = (*stat_ArrIt)->_player->_ID;
				MovePacket.X = (*stat_ArrIt)->_player->_x;
				MovePacket.Y = (*stat_ArrIt)->_player->_y;

				sendResult = _session->_sendQ.Enqueue((char*)&MoveHeader, sizeof(MoveHeader), &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
				sendResult = _session->_sendQ.Enqueue((char*)&MovePacket, MoveHeader.bySize, &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}


		}
	}


	return true;
}
bool MoveSectorD(Session* _session, int x, int y, int oldX, int oldY)
{
	unsigned int enqueResult;
	bool sendResult;

	int oldSectorX;
	int oldSectorY;

	int newSectorX;
	int newSectorY;

	int sectorRightMax = dfRANGE_MOVE_RIGHT / SECTOR_RATIO;
	int sectorBottomMax = dfRANGE_MOVE_BOTTOM / SECTOR_RATIO;

	// 이전 섹터에 내 세션 딜리트 메세지 보내주기//
	for (int i = -1; i < 2; i++)
	{
		oldSectorX = oldX / SECTOR_RATIO + i;
		oldSectorY = oldY / SECTOR_RATIO - 1;

		if (oldSectorY< 0 ||oldSectorY> sectorBottomMax) break;
		if (oldSectorX< 0 ||oldSectorX> sectorRightMax) continue;


		for (stat_ArrIt = Sector[oldSectorX][oldSectorY].begin();
			stat_ArrIt != Sector[oldSectorX][oldSectorY].end(); stat_ArrIt++)
		{
			if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;
			PacketHeader pHeader;
			SC_DELETE_CHARACTER deleteMsg;
			pHeader.byCode = 0x89;
			pHeader.bySize = sizeof(deleteMsg);
			pHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

			deleteMsg.ID = _session->_player->_ID;

			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&deleteMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}

			//나에게 이전 섹터 딜리트 메세지 보내기
			PacketHeader pMyHeader;
			SC_DELETE_CHARACTER myDeleteMsg;
			pMyHeader.byCode = 0x89;
			pMyHeader.bySize = sizeof(myDeleteMsg);
			pMyHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

			myDeleteMsg.ID = (*stat_ArrIt)->_player->_ID;

			sendResult = _session->_sendQ.Enqueue((char*)&pMyHeader, sizeof(pMyHeader), &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			sendResult = _session->_sendQ.Enqueue((char*)&myDeleteMsg, pMyHeader.bySize, &enqueResult);
			if (sendResult == false)
			{
				printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}
			printf("Send Delete Message || dest : %d, target : %d\n", _session->_player->_ID, myDeleteMsg.ID);

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
		newSectorX = x / SECTOR_RATIO + i;
		newSectorY = y / SECTOR_RATIO + 1;


		if (newSectorY< 0 || newSectorY> sectorBottomMax) break;
		if (newSectorX< 0 || newSectorX> sectorRightMax) continue;

		for (stat_ArrIt = Sector[newSectorX][newSectorY].begin();
			stat_ArrIt != Sector[newSectorY][newSectorY].end(); stat_ArrIt++)
		{
			if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;
			createOtherMsg.Direction = (*stat_ArrIt)->_player->_direction;
			createOtherMsg.HP = (*stat_ArrIt)->_player->_hp;
			createOtherMsg.ID = (*stat_ArrIt)->_player->_ID;
			createOtherMsg.X = (*stat_ArrIt)->_player->_x;
			createOtherMsg.Y = (*stat_ArrIt)->_player->_y;

			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&createMyMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			sendResult = _session->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			sendResult = _session->_sendQ.Enqueue((char*)&createOtherMsg, pHeader.bySize, &enqueResult);
			if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);



			//MoveMessage 보내야 함 
			if (_session->_player->_move == true)
			{
				//다른 사람한테 내가 움직인다는 메세지를 보내야 함
				PacketHeader MoveHeader;
				SC_MOVE_START MovePacket;
				MoveHeader.byCode = 0x89;
				MoveHeader.bySize = sizeof(MovePacket);
				MoveHeader.byType = dfPACKET_SC_MOVE_START;

				MovePacket.Direction = _session->_player->_direction;
				MovePacket.ID = _session->_player->_ID;
				MovePacket.X = _session->_player->_x;
				MovePacket.Y = _session->_player->_y;

				sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&MoveHeader, sizeof(MoveHeader), &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

				sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&MovePacket, MoveHeader.bySize, &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

			}
			if ((*stat_ArrIt)->_player->_move == true)
			{
				//나에게 다른 사람이 움직인다는 메세지를 보내야 함 
				PacketHeader MoveHeader;
				SC_MOVE_START MovePacket;
				MoveHeader.byCode = 0x89;
				MoveHeader.bySize = sizeof(MovePacket);
				MoveHeader.byType = dfPACKET_SC_MOVE_START;

				MovePacket.Direction = (*stat_ArrIt)->_player->_direction;
				MovePacket.ID = (*stat_ArrIt)->_player->_ID;
				MovePacket.X = (*stat_ArrIt)->_player->_x;
				MovePacket.Y = (*stat_ArrIt)->_player->_y;

				sendResult = _session->_sendQ.Enqueue((char*)&MoveHeader, sizeof(MoveHeader), &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
				sendResult = _session->_sendQ.Enqueue((char*)&MovePacket, MoveHeader.bySize, &enqueResult);
				if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
			}


		}
	}


	return true;
}


bool SyncSector(Session* _session, int oldSecX, int oldSecY,  int newSecX, int newSecY)
{
	unsigned int enqueResult;
	int sendResult;


	//이전 섹터 주변에 내 세션 삭제 메세지 보내기
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{

			if (oldSecX + i < 0 || oldSecX + i > dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
			if (oldSecY + j < 0 || oldSecY + j > dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;

			for (stat_ArrIt = Sector[oldSecX + i][oldSecY + j].begin();
				stat_ArrIt != Sector[oldSecX + i][oldSecY + j].end(); stat_ArrIt++)
			{
				if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;
				
				PacketHeader pHeader;
				SC_DELETE_CHARACTER deletePacket;

				pHeader.byCode = 0x89;
				pHeader.bySize = sizeof(deletePacket);
				pHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

				deletePacket.ID = _session->_player->_ID;

				sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
				if (sendResult == SOCKET_ERROR && GetLastError() != WSAEWOULDBLOCK)
				{
					printf("send Error || sendResult : %d, enqueResult : %d, LINE : %d , Errorcode: %d",
						sendResult, enqueResult, __LINE__, GetLastError());
				}
				sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&deletePacket, pHeader.bySize, &enqueResult);
				if (sendResult == SOCKET_ERROR && GetLastError() != WSAEWOULDBLOCK)
				{
					printf("send Error || sendResult : %d, enqueResult : %d, LINE : %d , Errorcode: %d",
						sendResult, enqueResult, __LINE__, GetLastError());
				}


			}
		}
	}

	//이번 섹터 주변에 내 섹터 생성 메세지 전송

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{

			if (newSecX + i < 0 || newSecX + i > dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
			if (newSecY + j < 0 || newSecY + j > dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;

			for (stat_ArrIt = Sector[newSecX + i][newSecY + j].begin();
				stat_ArrIt != Sector[newSecX + i][newSecY + j].end(); stat_ArrIt++)
			{
				//내 세션 삭제 메세지 보내기
				if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;

				PacketHeader pHeader;
				SC_CREATE_OTHER_CHARACTER createPacket;

				pHeader.byCode = 0x89;
				pHeader.bySize = sizeof(createPacket);
				pHeader.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;

				createPacket.ID = _session->_player->_ID;
				createPacket.Direction = _session->_player->_direction;
				createPacket.HP = _session->_player->_hp;
				createPacket.X = _session->_player->_x;
				createPacket.Y = _session->_player->_y;

				sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
				if (sendResult == SOCKET_ERROR && GetLastError() != WSAEWOULDBLOCK)
				{
					printf("send Error || sendResult : %d, enqueResult : %d, LINE : %d , Errorcode: %d",
						sendResult, enqueResult, __LINE__, GetLastError());
				}
				sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&createPacket, pHeader.bySize, &enqueResult);
				if (sendResult == SOCKET_ERROR && GetLastError() != WSAEWOULDBLOCK)
				{
					printf("send Error || sendResult : %d, enqueResult : %d, LINE : %d , Errorcode: %d",
						sendResult, enqueResult, __LINE__, GetLastError());
				}



				//MoveMessage 보내야 함 
				if (_session->_player->_move == true)
				{
					//다른 사람한테 내가 움직인다는 메세지를 보내야 함
					PacketHeader MoveHeader;
					SC_MOVE_START MovePacket;
					MoveHeader.byCode = 0x89;
					MoveHeader.bySize = sizeof(MovePacket);
					MoveHeader.byType = dfPACKET_SC_MOVE_START;

					MovePacket.Direction = _session->_player->_direction;
					MovePacket.ID = _session->_player->_ID;
					MovePacket.X = _session->_player->_x;
					MovePacket.Y = _session->_player->_y;

					sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&MoveHeader, sizeof(MoveHeader), &enqueResult);
					if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

					sendResult = (*stat_ArrIt)->_sendQ.Enqueue((char*)&MovePacket, MoveHeader.bySize, &enqueResult);
					if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);

				}
				if ((*stat_ArrIt)->_player->_move == true)
				{
					//나에게 다른 사람이 움직인다는 메세지를 보내야 함 
					PacketHeader MoveHeader;
					SC_MOVE_START MovePacket;
					MoveHeader.byCode = 0x89;
					MoveHeader.bySize = sizeof(MovePacket);
					MoveHeader.byType = dfPACKET_SC_MOVE_START;

					MovePacket.Direction = (*stat_ArrIt)->_player->_direction;
					MovePacket.ID = (*stat_ArrIt)->_player->_ID;
					MovePacket.X = (*stat_ArrIt)->_player->_x;
					MovePacket.Y = (*stat_ArrIt)->_player->_y;

					sendResult = _session->_sendQ.Enqueue((char*)&MoveHeader, sizeof(MoveHeader), &enqueResult);
					if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
					sendResult = _session->_sendQ.Enqueue((char*)&MovePacket, MoveHeader.bySize, &enqueResult);
					if (sendResult == false) printf("sendResult Error :%d, enqueResult = %d, Line : %d\n", sendResult, enqueResult, __LINE__);
				}



			}
		}
	}

	//실제 섹터 이동
	Sector[newSecX][newSecY].push_back(_session);
	Sector[oldSecX][oldSecY].remove(_session);


	return true;
}
