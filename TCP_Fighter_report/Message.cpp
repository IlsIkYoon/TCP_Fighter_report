#include "Message.h"
#include "Sector.h"

extern std::list<Session*> Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];
extern int sectorXRange;
extern int sectorYRange;
extern DWORD SyncMessageCount;

void SendMoveStartMessage(char* src, char* dest)
{
	unsigned int enqueResult;

	Session* _src = (Session*)src;
	Session* _dest = (Session*)dest;
	PacketHeader pHeader;
	SC_MOVE_START MovePacket;

	pHeader.byCode = 0x89;
	pHeader.bySize = sizeof(MovePacket);
	pHeader.byType = dfPACKET_SC_MOVE_START;

	MovePacket.Direction = _src->_player->_direction;
	MovePacket.ID = _src->_player->_ID;
	MovePacket.X = _src->_player->_x;
	MovePacket.Y = _src->_player->_y;

	//Enque하기
	if (_dest->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	if (_dest->_sendQ.Enqueue((char*)&MovePacket, pHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	return;
}


void SendCreateOtherCharMessage(char* src, char* dest)
{
	unsigned int enqueResult;

	Session* _src = (Session*)src;
	Session* _dest = (Session*)dest;

	//printf("Send CreateOtherMessage || src : %d, dest : %d\n", _src->_player->_ID, _dest->_player->_ID);

	PacketHeader pHeader;
	SC_CREATE_OTHER_CHARACTER CreatePacket;

	pHeader.byCode = 0x89;
	pHeader.bySize = sizeof(CreatePacket);
	pHeader.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;

	CreatePacket.Direction = _src->_player->_direction;
	CreatePacket.HP = _src->_player->_hp;
	CreatePacket.ID = _src->_player->_ID;
	CreatePacket.X = _src->_player->_x;
	CreatePacket.Y = _src->_player->_y;


	if (_dest->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	if (_dest->_sendQ.Enqueue((char*)&CreatePacket, pHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	return;

}


void SendDeleteMessage(char* src, char* dest)
{
	unsigned int enqueResult;

	Session* _src = (Session*)src;
	Session* _dest = (Session*)dest;

	//printf("Send Delete Message || src : %d, dest : %d\n", _src->_player->_ID, _dest->_player->_ID);
	PacketHeader pHeader;
	SC_DELETE_CHARACTER DeletePacket;


	pHeader.byCode = 0x89;
	pHeader.bySize = sizeof(DeletePacket);
	pHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

	DeletePacket.ID = _src->_player->_ID;


	if (_dest->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	if (_dest->_sendQ.Enqueue((char*)&DeletePacket, pHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	return;

}

void SendMoveStopMessage(char* src, char* dest)
{
	unsigned int enqueResult;

	Session* _src = (Session*)src;
	Session* _dest = (Session*)dest;

	PacketHeader pHeader;
	SC_MOVE_STOP MoveStopPacket;


	pHeader.byCode = 0x89;
	pHeader.bySize = sizeof(MoveStopPacket);
	pHeader.byType = dfPACKET_SC_MOVE_STOP;

	MoveStopPacket.Direction = _src->_player->_direction;
	MoveStopPacket.ID = _src->_player->_ID;
	MoveStopPacket.X = _src->_player->_x;
	MoveStopPacket.Y = _src->_player->_y;


	if (_dest->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	if (_dest->_sendQ.Enqueue((char*)&MoveStopPacket, pHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	return;




}


void SendAttack1Message(char* src, char* dest, char* _srcAttackPacket)
{
	unsigned int enqueResult;

	CS_ATTACK1* AttackPacket = (CS_ATTACK1*)_srcAttackPacket;

	Session* _src = (Session*)src;
	Session* _dest = (Session*)dest;

	PacketHeader AttackHeader;
	SC_ATTACK1 SCAttackPacket;

	AttackHeader.byCode = 0x89;
	AttackHeader.bySize = sizeof(SCAttackPacket);
	AttackHeader.byType = dfPACKET_SC_ATTACK1;

	SCAttackPacket.Direction = AttackPacket->Direction;
	SCAttackPacket.X = AttackPacket->X;
	SCAttackPacket.Y = AttackPacket->Y;
	SCAttackPacket.ID = _src->_player->_ID;

	if (_dest->_sendQ.Enqueue((char*)&AttackHeader, sizeof(AttackHeader), &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	if (_dest->_sendQ.Enqueue((char*)&SCAttackPacket, AttackHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	return;

}


void SendAttack2Message(char* src, char* dest, char* _srcAttackPacket)
{
	unsigned int enqueResult;

	CS_ATTACK2* AttackPacket = (CS_ATTACK2*)_srcAttackPacket;

	Session* _src = (Session*)src;
	Session* _dest = (Session*)dest;

	PacketHeader AttackHeader;
	SC_ATTACK2 SCAttackPacket;

	AttackHeader.byCode = 0x89;
	AttackHeader.bySize = sizeof(SCAttackPacket);
	AttackHeader.byType = dfPACKET_SC_ATTACK2;

	SCAttackPacket.Direction = AttackPacket->Direction;
	SCAttackPacket.X = AttackPacket->X;
	SCAttackPacket.Y = AttackPacket->Y;
	SCAttackPacket.ID = _src->_player->_ID;

	if (_dest->_sendQ.Enqueue((char*)&AttackHeader, sizeof(AttackHeader), &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	if (_dest->_sendQ.Enqueue((char*)&SCAttackPacket, AttackHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	return;



}
void SendAttack3Message(char* src, char* dest, char* _srcAttackPacket)
{
	unsigned int enqueResult;

	CS_ATTACK3* AttackPacket = (CS_ATTACK3*)_srcAttackPacket;

	Session* _src = (Session*)src;
	Session* _dest = (Session*)dest;

	PacketHeader AttackHeader;
	SC_ATTACK3 SCAttackPacket;

	AttackHeader.byCode = 0x89;
	AttackHeader.bySize = sizeof(SCAttackPacket);
	AttackHeader.byType = dfPACKET_SC_ATTACK3;

	SCAttackPacket.Direction = AttackPacket->Direction;
	SCAttackPacket.X = AttackPacket->X;
	SCAttackPacket.Y = AttackPacket->Y;
	SCAttackPacket.ID = _src->_player->_ID;

	if (_dest->_sendQ.Enqueue((char*)&AttackHeader, sizeof(AttackHeader), &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	if (_dest->_sendQ.Enqueue((char*)&SCAttackPacket, AttackHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	return;


}
void SendDamageMessage(char* Attack, char* dest, char* Damage)
{
	unsigned int enqueResult;


	Session* _attack = (Session*)Attack;
	Session* _damage = (Session*)Damage;
	Session* _dest = (Session*)dest;
	

	PacketHeader pHeader;
	SC_DAMAGE DamagePacket;

	pHeader.byCode = 0x89;
	pHeader.bySize = sizeof(DamagePacket);
	pHeader.byType = dfPACKET_SC_DAMAGE;

	DamagePacket.AttackID = _attack->_player->_ID;
	DamagePacket.DamageHP = _damage->_player->_hp;
	DamagePacket.DamageID = _damage->_player->_ID;


	if (_dest->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	if (_dest->_sendQ.Enqueue((char*)&DamagePacket, pHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	return;

}



void SendSyncMessage(char* src, char* dest)
{
	unsigned int enqueResult;

	Session* _src = (Session*)src;
	Session* _dest = (Session*)dest;


	PacketHeader pHeader;
	SC_SYNC SC_Sync;

	pHeader.byCode = 0x89;
	pHeader.bySize = sizeof(SC_Sync);
	pHeader.byType = dfPACKET_SC_SYNC;

	SC_Sync.ID = _src->_player->_ID;
	SC_Sync.X = _src->_player->_x;
	SC_Sync.Y = _src->_player->_y;


	if (_dest->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	if (_dest->_sendQ.Enqueue((char*)&SC_Sync, pHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d RingBuffer Size : %d\n", __LINE__, enqueResult, _dest->_sendQ.GetSizeUsed());
	}
	return;
}


void MsgSectorBroadCasting(void (*Func)(char* src, char* dest), char* _src, int SectorX, int SectorY, bool SendMe)
{
	std::list<Session*>::iterator it;
	Session* pSrc = (Session*)_src;
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if (SectorX + i < 0 || SectorX + i >= sectorXRange) continue;
			if (SectorY + j < 0 || SectorY + j >= sectorYRange) continue;

			for (it = Sector[SectorX + i][SectorY + j].begin();
				it != Sector[SectorX + i][SectorY + j].end(); it++)
			{
				//여기에 나를 제외한 곳에 뿌린다는 옵션이 들어가야 하나 ?
				if ((*it)->_player->_ID == pSrc->_player->_ID && SendMe == false)
				{
					continue;
				}
				Func(_src, (char*)(*it));
			}
		}
	}
}



void MsgSectorRSend(void (*Func)(char* src, char* dest), char* _src, int SectorX, int SectorY, bool SendMe)
{
	std::list<Session*>::iterator it;
	Session* pSrc = (Session*)_src;
	for (int i = -1; i < 2; i++)
	{
		if (SectorX + 1 < 0 || SectorX + 1 >= sectorXRange) break;
		if (SectorY + i < 0 || SectorY + i >= sectorYRange) continue;

		for (it = Sector[SectorX + 1][SectorY + i].begin();
			it != Sector[SectorX + 1][SectorY + i].end(); it++)
			{
				if ((*it)->_player->_ID == pSrc->_player->_ID && SendMe == false)
				{
					continue;
				}
				Func(_src, (char*)(*it));
			}
	}
}
void MsgSectorLSend(void (*Func)(char* src, char* dest), char* _src, int SectorX, int SectorY, bool SendMe)
{
	std::list<Session*>::iterator it;
	Session* pSrc = (Session*)_src;
	for (int i = -1; i < 2; i++)
	{
		if (SectorX - 1 < 0 || SectorX - 1 >= sectorXRange) break;
		if (SectorY + i < 0 || SectorY + i >= sectorYRange) continue;

		for (it = Sector[SectorX - 1][SectorY + i].begin();
			it != Sector[SectorX - 1][SectorY + i].end(); it++)
		{
			if ((*it)->_player->_ID == pSrc->_player->_ID && SendMe == false)
			{
				continue;
			}
			Func(_src, (char*)(*it));
		}
	}

}
void MsgSectorUSend(void (*Func)(char* src, char* dest), char* _src, int SectorX, int SectorY, bool SendMe)
{
	std::list<Session*>::iterator it;
	Session* pSrc = (Session*)_src;
	for (int i = -1; i < 2; i++)
	{
		if (SectorX + i < 0 || SectorX + i >= sectorXRange) continue;
		if (SectorY - 1 < 0 || SectorY - 1 >= sectorYRange) break;

		for (it = Sector[SectorX + i][SectorY - 1].begin();
			it != Sector[SectorX + i][SectorY - 1].end(); it++)
		{
			if ((*it)->_player->_ID == pSrc->_player->_ID && SendMe == false)
			{
				continue;
			}
			Func(_src, (char*)(*it));
		}
	}
}
void MsgSectorDSend(void (*Func)(char* src, char* dest), char* _src, int SectorX, int SectorY, bool SendMe)
{
	std::list<Session*>::iterator it;
	Session* pSrc = (Session*)_src;
	for (int i = -1; i < 2; i++)
	{
		if (SectorX + i < 0 || SectorX + i >= sectorXRange) break;
		if (SectorY + 1 < 0 || SectorY + 1 >= sectorYRange) continue;

		for (it = Sector[SectorX + i][SectorY + 1].begin();
			it != Sector[SectorX + i][SectorY + 1].end(); it++)
		{
			if ((*it)->_player->_ID == pSrc->_player->_ID && SendMe == false)
			{
				continue;
			}
			Func(_src, (char*)(*it));
		}
	}
}


void MsgSectorBroadCasting(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe)
{
	std::list<Session*>::iterator it;
	Session* pSrc = (Session*)_src;
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if (SectorX + i < 0 || SectorX + i >= sectorXRange) continue;
			if (SectorY + j < 0 || SectorY + j >= sectorYRange) continue;

			for (it = Sector[SectorX + i][SectorY + j].begin();
				it != Sector[SectorX + i][SectorY + j].end(); it++)
			{
				if ((*it)->_player->_ID == pSrc->_player->_ID && SendMe == false)
				{
					continue;
				}
				Func(_src, (char*)(*it), AttackPacket);
			}
		}
	}

}
void MsgSectorRSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe)
{
	std::list<Session*>::iterator it;
	Session* pSrc = (Session*)_src;
	for (int i = -1; i < 2; i++)
	{
		if (SectorX + 1 < 0 || SectorX + 1 >= sectorXRange) break;
		if (SectorY + i < 0 || SectorY + i >= sectorYRange) continue;

		for (it = Sector[SectorX + 1][SectorY + i].begin();
			it != Sector[SectorX + 1][SectorY + i].end(); it++)
		{
			if ((*it)->_player->_ID == pSrc->_player->_ID && SendMe == false)
			{
				continue;
			}
			Func(_src, (char*)(*it), AttackPacket);
		}
	}

}
void MsgSectorLSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe)
{
	std::list<Session*>::iterator it;
	Session* pSrc = (Session*)_src;
	for (int i = -1; i < 2; i++)
	{
		if (SectorX - 1 < 0 || SectorX - 1 >= sectorXRange) break;
		if (SectorY + i < 0 || SectorY + i >= sectorYRange) continue;

		for (it = Sector[SectorX - 1][SectorY + i].begin();
			it != Sector[SectorX - 1][SectorY + i].end(); it++)
		{
			if ((*it)->_player->_ID == pSrc->_player->_ID && SendMe == false)
			{
				continue;
			}
			Func(_src, (char*)(*it), AttackPacket);
		}
	}

}
void MsgSectorUSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe)
{
	std::list<Session*>::iterator it;
	Session* pSrc = (Session*)_src;
	for (int i = -1; i < 2; i++)
	{
		if (SectorX + i < 0 || SectorX + i >= sectorXRange) continue;
		if (SectorY - 1 < 0 || SectorY - 1 >= sectorYRange) break;

		for (it = Sector[SectorX + i][SectorY - 1].begin();
			it != Sector[SectorX + i][SectorY - 1].end(); it++)
		{
			if ((*it)->_player->_ID == pSrc->_player->_ID && SendMe == false)
			{
				continue;
			}
			Func(_src, (char*)(*it), AttackPacket);
		}
	}

}
void MsgSectorDSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe)
{
	std::list<Session*>::iterator it;
	Session* pSrc = (Session*)_src;
	for (int i = -1; i < 2; i++)
	{
		if (SectorX + i < 0 || SectorX + i >= sectorXRange) break;
		if (SectorY + 1 < 0 || SectorY + 1 >= sectorYRange) continue;

		for (it = Sector[SectorX + i][SectorY + 1].begin();
			it != Sector[SectorX + i][SectorY + 1].end(); it++)
		{
			if ((*it)->_player->_ID == pSrc->_player->_ID && SendMe == false)
			{
				continue;
			}
			Func(_src, (char*)(*it), AttackPacket);
		}
	}


}


void RestorePacket(Session* _session, int packetSize, int packetType)
{
	unsigned int dequeResult;
	unsigned int enqueResult;
	PacketHeader pHeader;
	pHeader.byCode = 0x89;
	pHeader.bySize = packetSize;
	pHeader.byType = packetType;
	int size = _session->_recvQ.GetSizeUsed();
	char* localBuf = (char*)malloc(size);

	_session->_recvQ.Dequeue(localBuf, size, &dequeResult);
	_session->_recvQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult);
	_session->_recvQ.Enqueue(localBuf, size, &enqueResult);

	return;
}



void SyncPos(Session* pSession, int sX, int sY, int cX, int cY)
{


	int SVSectorX = sX / SECTOR_RATIO;
	int SVSectorY = sY / SECTOR_RATIO;
	SendSyncMessage((char*)pSession, (char*)pSession);

	//MsgSectorBroadCasting(SendSyncMessage, (char*)pSession, SVSectorX, SVSectorY, true);


	SyncMessageCount++;


	//SyncSector(pSession, CLSectorX, CLSectorY, SVSectorX, SVSectorY);

	

}