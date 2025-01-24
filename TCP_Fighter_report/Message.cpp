#include "Message.h"




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

	//EnqueÇÏ±â
	if (_dest->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
	}
	if (_dest->_sendQ.Enqueue((char*)&MovePacket, pHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
	}
	return;
}


void SendCreateOtherCharMessage(char* src, char* dest)
{
	unsigned int enqueResult;

	Session* _src = (Session*)src;
	Session* _dest = (Session*)dest;

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
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
	}
	if (_dest->_sendQ.Enqueue((char*)&CreatePacket, pHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
	}
	return;

}


void SendDeleteMessage(char* src, char* dest)
{
	unsigned int enqueResult;

	Session* _src = (Session*)src;
	Session* _dest = (Session*)dest;

	PacketHeader pHeader;
	SC_DELETE_CHARACTER DeletePacket;

	pHeader.byCode = 0x89;
	pHeader.bySize = sizeof(DeletePacket);
	pHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

	DeletePacket.ID = _src->_player->_ID;


	if (_dest->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
	}
	if (_dest->_sendQ.Enqueue((char*)&DeletePacket, pHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
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
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
	}
	if (_dest->_sendQ.Enqueue((char*)&MoveStopPacket, pHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
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
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
	}
	if (_dest->_sendQ.Enqueue((char*)&SCAttackPacket, AttackHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
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
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
	}
	if (_dest->_sendQ.Enqueue((char*)&SCAttackPacket, AttackHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
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
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
	}
	if (_dest->_sendQ.Enqueue((char*)&SCAttackPacket, AttackHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
	}
	return;


}
void SendDamageMessage(char* Attack, char* Damage, char* dest)
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
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
	}
	if (_dest->_sendQ.Enqueue((char*)&DamagePacket, pHeader.bySize, &enqueResult) == false)
	{
		printf("Send Error ||Line : %d, enqueResult : %d\n", __LINE__, enqueResult);
	}
	return;

}
