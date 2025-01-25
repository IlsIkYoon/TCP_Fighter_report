#include "GameProc.h"
#include "Message.h"

extern Session SessionArr[PLAYERMAXCOUNT];

extern DWORD playerIdex;

extern std::list<Session*> Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];
extern Session* pSector;
std::list<Session*>::iterator it;

extern int sectorXRange;
extern int sectorYRange;


bool MoveStart(Session* _session)
{
	CS_MOVE_START MoveStartPacket;
	unsigned int peekResult;
	unsigned int dequeResult;
	unsigned int enqueResult;
	_session->_recvQ.Peek((char*)&MoveStartPacket, sizeof(MoveStartPacket), &peekResult);

	if (peekResult < sizeof(MoveStartPacket)) {
		
		printf("Peek error, Line : %d, front : %d, rear : %d\n", __LINE__, _session->_recvQ.GetFront(), _session->_recvQ.GetRear());
		__debugbreak();


		//패킷 헤더 다시 넣는 과정
		int ExtraBuf = _session->_recvQ.GetSizeUsed();
		char* backUpBuf = (char*)malloc(ExtraBuf);
		PacketHeader pHeader;
		pHeader.byCode = 0x89;
		pHeader.bySize = sizeof(MoveStartPacket);
		pHeader.byType = dfPACKET_CS_MOVE_START;

		_session->_recvQ.Dequeue(backUpBuf, ExtraBuf, &dequeResult);
		_session->_recvQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
		_session->_recvQ.Enqueue(backUpBuf, ExtraBuf, &enqueResult);

		return false;
	}
	
	_session->_recvQ.MoveFront(sizeof(MoveStartPacket));

	_session->_player->MoveStart(MoveStartPacket.Direction, MoveStartPacket.X, MoveStartPacket.Y);


	//섹터 주변에 뿌려주기

	int sectorX = (_session->_player->_x) / SECTOR_RATIO;
	int sectorY = (_session->_player->_y) / SECTOR_RATIO;

	MsgSectorBroadCasting(SendMoveStartMessage, (char*)_session, sectorX, sectorY, false);


	return true;
}


bool MoveStop(Session* _session)
{

	CS_MOVE_STOP MoveStopPacket;
	unsigned int peekResult;
	unsigned int dequeResult;
	unsigned int enqueResult;
	_session->_recvQ.Peek((char*)&MoveStopPacket, sizeof(MoveStopPacket), &peekResult);
	if (peekResult < sizeof(MoveStopPacket)) {

		printf("Peek error, Line : %d, front : %d, rear : %d\n", __LINE__, _session->_recvQ.GetFront(), _session->_recvQ.GetRear());
		__debugbreak();
		//패킷 헤더 다시 넣는 과정
		int ExtraBuf = _session->_recvQ.GetSizeUsed();
		char* backUpBuf = (char*)malloc(ExtraBuf);
		PacketHeader pHeader;
		pHeader.byCode = 0x89;
		pHeader.bySize = sizeof(MoveStopPacket);
		pHeader.byType = dfPACKET_CS_MOVE_STOP;

		_session->_recvQ.Dequeue(backUpBuf, ExtraBuf, &dequeResult);
		_session->_recvQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
		_session->_recvQ.Enqueue(backUpBuf, ExtraBuf, &enqueResult);

		return false;
	}

	_session->_recvQ.MoveFront(sizeof(MoveStopPacket));
	

	_session->_player->MoveStop(MoveStopPacket.Direction, MoveStopPacket.X, MoveStopPacket.Y);
	

	int sectorX = (_session->_player->_x) / SECTOR_RATIO;
	int sectorY = (_session->_player->_y) / SECTOR_RATIO;


	MsgSectorBroadCasting(SendMoveStopMessage, (char*)_session, sectorX, sectorY, false);

	return true;
}


bool Attack1(Session* _session)
{
	CS_ATTACK1 AttackPacket;
	unsigned int peekResult;
	unsigned int dequeResult;
	unsigned int enqueResult;

	_session->_recvQ.Peek((char*)&AttackPacket, sizeof(AttackPacket), &peekResult);

	if (peekResult < sizeof(AttackPacket))
	{

		printf("Peek error, Line : %d, front : %d, rear : %d\n", __LINE__, _session->_recvQ.GetFront(), _session->_recvQ.GetRear());
		__debugbreak();
		//패킷 헤더 다시 넣는 과정
		int ExtraBuf = _session->_recvQ.GetSizeUsed();
		char* backUpBuf = (char*)malloc(ExtraBuf);
		PacketHeader pHeader;
		pHeader.byCode = 0x89;
		pHeader.bySize = sizeof(AttackPacket);
		pHeader.byType = dfPACKET_CS_ATTACK1;

		_session->_recvQ.Dequeue(backUpBuf, ExtraBuf, &dequeResult);
		_session->_recvQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
		_session->_recvQ.Enqueue(backUpBuf, ExtraBuf, &enqueResult);

		return false;
	}

	_session->_recvQ.MoveFront(sizeof(AttackPacket));

	_session->_player->_direction = AttackPacket.Direction;

	//먼저 어택 메세지를 섹터에 뿌려줌

	int sectorX = (_session->_player->_x) / SECTOR_RATIO;
	int sectorY = (_session->_player->_y) / SECTOR_RATIO;

	MsgSectorBroadCasting(SendAttack1Message, (char*)_session, (char*)&AttackPacket, sectorX, sectorY, false);


	if ((AttackPacket.Direction) == LL) //left
	{
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (sectorX + i < 0 || sectorX + i >= sectorXRange) continue;
				if (sectorY + j < 0 || sectorY + j >= sectorYRange) continue;

				it = Sector[sectorX + i][sectorY + j].begin();

				for (; it != Sector[sectorX + i][sectorY + j].end(); it++)
				{
					if ((*it)->_player->_ID == _session->_player->_ID) continue;

					if ((AttackPacket.X - (*it)->_player->_x) < 0 ||
						(AttackPacket.X - (*it)->_player->_x) > dfATTACK1_RANGE_X) continue; //공격 범위 벗어남
					
					if ((AttackPacket.Y - (*it)->_player->_y >= 0 && AttackPacket.Y - (*it)->_player->_y < dfATTACK1_RANGE_Y) ||
						((*it)->_player->_y - AttackPacket.Y >= 0 && (*it)->_player->_y - AttackPacket.Y < dfATTACK1_RANGE_Y) == false) 
						continue;

					//타격 성공 
					(*it)->_player->_hp -= dfAttack1Damage;


					//데미지 메세지 보내기

					std::list<Session*>::iterator FuncIt;

					MsgSectorBroadCasting(SendDamageMessage, (char*)_session, (char*)(*it), sectorX, sectorY, true);

					//사망 판정 //

					if ((*it)->_player->_hp <= 0)
					{

						MsgSectorBroadCasting(SendDeleteMessage, (char*)(*it), sectorX, sectorY, true);


						DeleteSession(*it);
					}
						return true; //한명 타격 성공하면 바로 리턴 

				}
			}
		}

	}

	else
	{
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (sectorX  + i < 0 || sectorX + i >= sectorXRange) continue;
				if (sectorY + j < 0 || sectorY + j >= sectorYRange) continue;

				it = Sector[sectorX + i][sectorY + j].begin();

				for (; it != Sector[sectorX + i][sectorY + j].end(); it++)
				{
					if ((*it)->_player->_ID == _session->_player->_ID) continue;

					if (((*it)->_player->_x) - AttackPacket.X < 0 ||
						((*it)->_player->_x) - AttackPacket.X > dfATTACK1_RANGE_X) continue; //공격 범위 벗어남

					if ((AttackPacket.Y - (*it)->_player->_y >= 0 && AttackPacket.Y - (*it)->_player->_y < dfATTACK1_RANGE_Y) ||
						((*it)->_player->_y - AttackPacket.Y >= 0 && (*it)->_player->_y - AttackPacket.Y < dfATTACK1_RANGE_Y) == false)
						continue;

					//타격 성공 
					(*it)->_player->_hp -= dfAttack1Damage;


					//데미지 메세지 보내기

					std::list<Session*>::iterator FuncIt;

					MsgSectorBroadCasting(SendDamageMessage, (char*)_session,(char*)(*it), sectorX, sectorY, true);

					

					//사망 판정 //

					if ((*it)->_player->_hp <= 0)
					{
						MsgSectorBroadCasting(SendDeleteMessage, (char*)(*it), sectorX, sectorY, true);

						DeleteSession(*it);
					}

					return true; //한명 타격 성공하면 바로 리턴 
				}
			}
		}

	}


	return false;
}

bool Attack2(Session* _session)
{
	CS_ATTACK2 AttackPacket;
	unsigned int peekResult;
	unsigned int dequeResult;
	unsigned int enqueResult;

	_session->_recvQ.Peek((char*)&AttackPacket, sizeof(AttackPacket), &peekResult);

	if (peekResult < sizeof(AttackPacket))
	{

		printf("Peek error, Line : %d, front : %d, rear : %d\n", __LINE__, _session->_recvQ.GetFront(), _session->_recvQ.GetRear());
		__debugbreak();
		//패킷 헤더 다시 넣는 과정
		int ExtraBuf = _session->_recvQ.GetSizeUsed();
		char* backUpBuf = (char*)malloc(ExtraBuf);
		PacketHeader pHeader;
		pHeader.byCode = 0x89;
		pHeader.bySize = sizeof(AttackPacket);
		pHeader.byType = dfPACKET_CS_ATTACK2;

		_session->_recvQ.Dequeue(backUpBuf, ExtraBuf, &dequeResult);
		_session->_recvQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
		_session->_recvQ.Enqueue(backUpBuf, ExtraBuf, &enqueResult);

		return false;
	}

	_session->_recvQ.MoveFront(sizeof(AttackPacket));

	_session->_player->_direction = AttackPacket.Direction;


	int sectorX = (_session->_player->_x) / SECTOR_RATIO;
	int sectorY = (_session->_player->_y) / SECTOR_RATIO;


	//먼저 어택 메세지를 섹터에 뿌려줌
	MsgSectorBroadCasting(SendAttack2Message, (char*)_session, (char*)&AttackPacket, sectorX, sectorY, false);

	if ((AttackPacket.Direction) == LL) //left
	{
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (sectorX + i < 0 || sectorX + i >= sectorXRange) continue;
				if (sectorX + j < 0 || sectorY + j >= sectorYRange) continue;

				it = Sector[sectorX + i][sectorY + j].begin();

				for (; it != Sector[sectorX + i][sectorY + j].end(); it++)
				{
					if ((*it)->_player->_ID == _session->_player->_ID) continue;

					if ((AttackPacket.X - (*it)->_player->_x) < 0 ||
						(AttackPacket.X - (*it)->_player->_x) > dfATTACK2_RANGE_X) continue; //공격 범위 벗어남

					if ((AttackPacket.Y - (*it)->_player->_y >= 0 && AttackPacket.Y - (*it)->_player->_y < dfATTACK2_RANGE_Y) ||
						((*it)->_player->_y - AttackPacket.Y >= 0 && (*it)->_player->_y - AttackPacket.Y < dfATTACK2_RANGE_Y) == false)
						continue;

					//타격 성공 
					(*it)->_player->_hp -= dfAttack2Damage;

					MsgSectorBroadCasting(SendDamageMessage, (char*)_session, (char*)(*it), sectorX, sectorY, true);

					if ((*it)->_player->_hp <= 0)
					{
						MsgSectorBroadCasting(SendDeleteMessage, (char*)(*it), sectorX, sectorY, true);

						DeleteSession(*it);
					}

					return true; //한명 타격 성공하면 바로 리턴 
				}
			}
		}

	}

	else
	{
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (sectorX + i < 0 || sectorX + i >= sectorXRange) continue;
				if (sectorY + j < 0 || sectorY + j >= sectorYRange) continue;

				it = Sector[sectorX + i][sectorY + j].begin();

				for (; it != Sector[sectorX + i][sectorY + j].end(); it++)
				{
					if ((*it)->_player->_ID == _session->_player->_ID) continue;

					if (((*it)->_player->_x) - AttackPacket.X < 0 ||
						((*it)->_player->_x) - AttackPacket.X > dfATTACK2_RANGE_X) continue; //공격 범위 벗어남

					if ((AttackPacket.Y - (*it)->_player->_y >= 0 && AttackPacket.Y - (*it)->_player->_y < dfATTACK2_RANGE_Y) ||
						((*it)->_player->_y - AttackPacket.Y >= 0 && (*it)->_player->_y - AttackPacket.Y < dfATTACK2_RANGE_Y) == false)
						continue;

					//타격 성공
					(*it)->_player->_hp -= dfAttack2Damage;

					MsgSectorBroadCasting(SendDamageMessage, (char*)_session, (char*)(*it), sectorX, sectorY, true);

					if ((*it)->_player->_hp <= 0)
					{
						MsgSectorBroadCasting(SendDeleteMessage, (char*)(*it), sectorX, sectorY, true);
						DeleteSession(*it);
					}
						return true; //한명 타격 성공하면 바로 리턴 
				}
			}
		}

	}


	return false;
}
bool Attack3(Session* _session)
{
	CS_ATTACK3 AttackPacket;
	unsigned int peekResult;
	unsigned int dequeResult;
	unsigned int enqueResult;

	_session->_recvQ.Peek((char*)&AttackPacket, sizeof(AttackPacket), &peekResult);

	if (peekResult < sizeof(AttackPacket))
	{

		printf("Peek error, Line : %d, front : %d, rear : %d\n", __LINE__, _session->_recvQ.GetFront(), _session->_recvQ.GetRear());

		__debugbreak();
		//패킷 헤더 다시 넣는 과정
		int ExtraBuf = _session->_recvQ.GetSizeUsed();
		char* backUpBuf = (char*)malloc(ExtraBuf);
		PacketHeader pHeader;
		pHeader.byCode = 0x89;
		pHeader.bySize = sizeof(AttackPacket);
		pHeader.byType = dfPACKET_CS_ATTACK3;

		_session->_recvQ.Dequeue(backUpBuf, ExtraBuf, &dequeResult);
		_session->_recvQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
		_session->_recvQ.Enqueue(backUpBuf, ExtraBuf, &enqueResult);

		return false;
	}

	_session->_recvQ.MoveFront(sizeof(AttackPacket));

	_session->_player->_direction = AttackPacket.Direction;

	int sectorX = (_session->_player->_x) / SECTOR_RATIO;
	int sectorY = (_session->_player->_y) / SECTOR_RATIO;


	//먼저 어택 메세지를 섹터에 뿌려줌
	MsgSectorBroadCasting(SendAttack3Message, (char*)_session, (char*)&AttackPacket, sectorX, sectorY, false);

	if ((AttackPacket.Direction) == LL) //left
	{
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (sectorX + i < 0 || sectorX + i >= sectorXRange) continue;
				if (sectorY + j < 0 || sectorY + j >= sectorYRange) continue;

				it = Sector[sectorX + i][sectorY + j].begin();

				for (; it != Sector[sectorX + i][sectorY + j].end(); it++)
				{
					if ((*it)->_player->_ID == _session->_player->_ID) continue;

					if ((AttackPacket.X - (*it)->_player->_x) < 0 ||
						(AttackPacket.X - (*it)->_player->_x) > dfATTACK3_RANGE_X) continue; //공격 범위 벗어남

					if ((AttackPacket.Y - (*it)->_player->_y >= 0 && AttackPacket.Y - (*it)->_player->_y < dfATTACK3_RANGE_Y) ||
						((*it)->_player->_y - AttackPacket.Y >= 0 && (*it)->_player->_y - AttackPacket.Y < dfATTACK3_RANGE_Y) == false)
						continue;

					//타격 성공 
					(*it)->_player->_hp -= dfAttack3Damage;


					//데미지 메세지 보내기
					MsgSectorBroadCasting(SendDamageMessage, (char*)_session, (char*)(*it), sectorX, sectorY, true);
					if ((*it)->_player->_hp <= 0)
					{
						MsgSectorBroadCasting(SendDeleteMessage, (char*)(*it), sectorX, sectorY, true);
						DeleteSession(*it);
					}
						return true; //한명 타격 성공하면 바로 리턴 
				}
			}
		}

	}

	else
	{
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (sectorX + i < 0 || sectorX + i >= sectorXRange) continue;
				if (sectorY + j < 0 || sectorY + j >= sectorYRange) continue;

				it = Sector[sectorX + i][sectorY + j].begin();

				for (; it != Sector[sectorX + i][sectorY + j].end(); it++)
				{
					if ((*it)->_player->_ID == _session->_player->_ID) continue;

					if (((*it)->_player->_x) - AttackPacket.X < 0 ||
						((*it)->_player->_x) - AttackPacket.X > dfATTACK3_RANGE_X) continue; //공격 범위 벗어남

					if ((AttackPacket.Y - (*it)->_player->_y >= 0 && AttackPacket.Y - (*it)->_player->_y < dfATTACK3_RANGE_Y) ||
						((*it)->_player->_y - AttackPacket.Y >= 0 && (*it)->_player->_y - AttackPacket.Y < dfATTACK3_RANGE_Y) == false)
						continue;

					//타격 성공 
					(*it)->_player->_hp -= dfAttack3Damage;


					//데미지 메세지 보내기
					MsgSectorBroadCasting(SendDamageMessage, (char*)_session, (char*)(*it), sectorX, sectorY, true);
					if ((*it)->_player->_hp <= 0)
					{
						MsgSectorBroadCasting(SendDeleteMessage, (char*)(*it), sectorX, sectorY, true);
						DeleteSession(*it);
					}
						return true; //한명 타격 성공하면 바로 리턴 

				}
			}
		}

	}


	return false;
}

bool Sync(Session* _session)
{
	//Sync메세지 안에 있는 좌표를 서버 좌표에 동기화 후 다른 섹터에 뿌려주는 함수

	CS_SYNC CS_Sync;
	unsigned int peekResult;
	unsigned int dequeResult;
	unsigned int enqueResult;

	_session->_recvQ.Peek((char*)&CS_Sync, sizeof(CS_Sync), &peekResult);
	if (peekResult < sizeof(CS_Sync))
	{
		printf("Peek error, Line : %d, front : %d, rear : %d\n", __LINE__, _session->_recvQ.GetFront(), _session->_recvQ.GetRear());
		__debugbreak();
		//패킷 헤더 다시 넣는 과정
		int ExtraBuf = _session->_recvQ.GetSizeUsed();
		char* backUpBuf = (char*)malloc(ExtraBuf);
		PacketHeader pHeader;
		pHeader.byCode = 0x89;
		pHeader.bySize = sizeof(CS_Sync);
		pHeader.byType = dfPACKET_CS_SYNC;

		_session->_recvQ.Dequeue(backUpBuf, ExtraBuf, &dequeResult);
		_session->_recvQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
		_session->_recvQ.Enqueue(backUpBuf, ExtraBuf, &enqueResult);

		return false;

	}

	_session->_player->_x = CS_Sync.X;
	_session->_player->_y = CS_Sync.Y;

	int sectorX = (_session->_player->_x) / SECTOR_RATIO;
	int sectorY = (_session->_player->_y) / SECTOR_RATIO;


	MsgSectorBroadCasting(SendSyncMessage, (char*)_session, sectorX, sectorY, false);

	return true;
}




bool Echo(Session* _session)
{
	CS_ECHO CS_Echo;
	unsigned int peekResult;
	unsigned int dequeResult;
	unsigned int enqueResult;

	_session->_recvQ.Peek((char*)&CS_Echo, sizeof(CS_Echo), &peekResult);
	if (peekResult < sizeof(CS_Echo))
	{
		printf("Peek error, Line : %d, front : %d, rear : %d\n", __LINE__, _session->_recvQ.GetFront(), _session->_recvQ.GetRear());
		__debugbreak();
		//패킷 헤더 다시 넣는 과정
		int ExtraBuf = _session->_recvQ.GetSizeUsed();
		char* backUpBuf = (char*)malloc(ExtraBuf);
		PacketHeader pHeader;
		pHeader.byCode = 0x89;
		pHeader.bySize = sizeof(CS_Echo);
		pHeader.byType = dfPACKET_CS_ECHO;

		_session->_recvQ.Dequeue(backUpBuf, ExtraBuf, &dequeResult);
		_session->_recvQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
		_session->_recvQ.Enqueue(backUpBuf, ExtraBuf, &enqueResult);

		return false;

	}

	_session->_recvQ.MoveFront(sizeof(CS_Echo));

	//에코 메세지 다시 그 클라에 쏴주기

	PacketHeader pHeader;
	SC_ECHO SC_Echo;

	pHeader.byCode = 0x89;
	pHeader.bySize = sizeof(SC_Echo);
	pHeader.byType = dfPACKET_SC_ECHO;

	SC_Echo.Time = CS_Echo.Time;

	_session->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
	_session->_sendQ.Enqueue((char*)&SC_Echo, pHeader.bySize, &enqueResult);
	
	return true;
}