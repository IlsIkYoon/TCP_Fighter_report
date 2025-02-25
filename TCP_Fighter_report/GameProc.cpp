#include "GameProc.h"
#include "Message.h"
#include <format>
#include "Log.h"
#include "Sector.h"

extern std::list<Session*> SessionArr;

extern DWORD playerCount;

extern std::list<Session*> Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];
extern Session* pSector;
std::list<Session*>::iterator it;

extern int sectorXRange;
extern int sectorYRange;

extern std::list<std::string> LogQ;
extern CRITICAL_SECTION g_lock;

void UpdateGameLogic(DWORD deltaTime)
{

		for (auto session : SessionArr)
		{
			if (session->_delete == true) continue;

			if (session->_recvQ.IsEmpty() == false) {

				DecodeMessages(session);

			}

		}

		for (auto session : SessionArr)
		{

			if (session->_delete == true) continue;

			session->_player->Move(deltaTime);
		}
	TimeOutCheck(); 
	FlushDeleteArr();

}






bool MoveStart(Session* _session)
{
	CS_MOVE_START MoveStartPacket;
	unsigned int peekResult;

	_session->_recvQ.Peek((char*)&MoveStartPacket, sizeof(MoveStartPacket), &peekResult);

	if (peekResult < sizeof(MoveStartPacket)) {
		
		RestorePacket(_session, sizeof(MoveStartPacket), dfPACKET_CS_MOVE_START);
		return false;
	}
	

	_session->_recvQ.MoveFront(sizeof(MoveStartPacket));

	//헤더 추출 완료

	SyncCheck(_session, MoveStartPacket.X, MoveStartPacket.Y, __FILE__, __func__, __LINE__, GetLastError());


	_session->_player->MoveStart(MoveStartPacket.Direction);

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

	_session->_recvQ.Peek((char*)&MoveStopPacket, sizeof(MoveStopPacket), &peekResult);

	if (peekResult < sizeof(MoveStopPacket)) {
		RestorePacket(_session, sizeof(MoveStopPacket), dfPACKET_CS_MOVE_STOP);
		return false;
	}

	_session->_recvQ.MoveFront(sizeof(MoveStopPacket));
	

	SyncCheck(_session, MoveStopPacket.X, MoveStopPacket.Y, __FILE__, __func__, __LINE__, GetLastError());

	_session->_player->MoveStop(MoveStopPacket.Direction);


	int sectorX = MoveStopPacket.X / SECTOR_RATIO;
	int sectorY = MoveStopPacket.Y / SECTOR_RATIO;


	MsgSectorBroadCasting(SendMoveStopMessage, (char*)_session, sectorX, sectorY, false);

	

	return true;
}


bool Attack1(Session* _session)
{
	CS_ATTACK1 AttackPacket;
	unsigned int peekResult;

	_session->_recvQ.Peek((char*)&AttackPacket, sizeof(AttackPacket), &peekResult);

	if (peekResult < sizeof(AttackPacket))
	{
		RestorePacket(_session, sizeof(AttackPacket), dfPACKET_CS_ATTACK1);
		return false;
	}

	_session->_recvQ.MoveFront(sizeof(AttackPacket));

	//싱크 작업 먼저 
	SyncCheck(_session, AttackPacket.X, AttackPacket.Y, __FILE__, __func__, __LINE__, GetLastError());


	_session->_player->_direction = AttackPacket.Direction;

	//먼저 어택 메세지를 섹터에 뿌려줌

	int sectorX = (_session->_player->_x) / SECTOR_RATIO;
	int sectorY = (_session->_player->_y) / SECTOR_RATIO;

	//타격 및 데미지 로직//

	MsgSectorBroadCasting(SendAttack1Message, (char*)_session, (char*)&AttackPacket, sectorX, sectorY, true);

	Session* hitTarget = CheckHit(_session, dfATTACK1_RANGE_X, dfATTACK1_RANGE_Y);
	if (hitTarget == HIT_FAILED) return false;

	hitTarget->_player->_hp -= dfAttack1Damage;

	MsgSectorBroadCasting(SendDamageMessage, (char*)_session, (char*)hitTarget, sectorX, sectorY, true);

	if (hitTarget->_player->_hp <= 0)
	{
		DeleteSession(hitTarget);
	}

	return true;
}

bool Attack2(Session* _session)
{
	CS_ATTACK2 AttackPacket;
	unsigned int peekResult;

	_session->_recvQ.Peek((char*)&AttackPacket, sizeof(AttackPacket), &peekResult);

	if (peekResult < sizeof(AttackPacket))
	{
		RestorePacket(_session, sizeof(AttackPacket), dfPACKET_CS_ATTACK2);
		return false;
	}

	_session->_recvQ.MoveFront(sizeof(AttackPacket));


	SyncCheck(_session, AttackPacket.X, AttackPacket.Y, __FILE__, __func__, __LINE__, GetLastError());

	_session->_player->_direction = AttackPacket.Direction;

	int sectorX = (_session->_player->_x) / SECTOR_RATIO;
	int sectorY = (_session->_player->_y) / SECTOR_RATIO;

	
	//타격 및 데미지 로직//
	
	MsgSectorBroadCasting(SendAttack2Message, (char*)_session, (char*)&AttackPacket, sectorX, sectorY, true);

	Session* hitTarget = CheckHit(_session, dfATTACK2_RANGE_X, dfATTACK2_RANGE_Y);
	if (hitTarget == HIT_FAILED) return false;

	hitTarget->_player->_hp -= dfAttack2Damage;

	MsgSectorBroadCasting(SendDamageMessage, (char*)_session, (char*)hitTarget, sectorX, sectorY, true);

	if (hitTarget->_player->_hp <= 0)
	{
		DeleteSession(hitTarget);
	}

	return true;

}
bool Attack3(Session* _session)
{
	CS_ATTACK3 AttackPacket;
	unsigned int peekResult;

	_session->_recvQ.Peek((char*)&AttackPacket, sizeof(AttackPacket), &peekResult);

	if (peekResult < sizeof(AttackPacket))
	{

		RestorePacket(_session, sizeof(AttackPacket), dfPACKET_CS_ATTACK3);
		return false;
	}

	_session->_recvQ.MoveFront(sizeof(AttackPacket));


	SyncCheck(_session, AttackPacket.X, AttackPacket.Y, __FILE__, __func__, __LINE__, GetLastError());

	_session->_player->_direction = AttackPacket.Direction;

	int sectorX = (_session->_player->_x) / SECTOR_RATIO;
	int sectorY = (_session->_player->_y) / SECTOR_RATIO;

	//타격 및 데미지 로직//

	MsgSectorBroadCasting(SendAttack3Message, (char*)_session, (char*)&AttackPacket, sectorX, sectorY, true);

	Session* hitTarget = CheckHit(_session, dfATTACK3_RANGE_X, dfATTACK3_RANGE_Y);
	if (hitTarget == HIT_FAILED) return false;

	hitTarget->_player->_hp -= dfAttack3Damage;

	MsgSectorBroadCasting(SendDamageMessage, (char*)_session, (char*)hitTarget, sectorX, sectorY, true);

	if (hitTarget->_player->_hp <= 0)
	{
		DeleteSession(hitTarget);
	}

	return true;
}

bool Sync(Session* _session)
{
	//Sync메세지 안에 있는 좌표를 서버 좌표에 동기화 후 다른 섹터에 뿌려주는 함수

	CS_SYNC CS_Sync;
	unsigned int peekResult;

	_session->_recvQ.Peek((char*)&CS_Sync, sizeof(CS_Sync), &peekResult);
	if (peekResult < sizeof(CS_Sync))
	{
		RestorePacket(_session, sizeof(CS_Sync), dfPACKET_CS_SYNC);
		return false;
	}

	_session->_recvQ.MoveFront(sizeof(CS_Sync));

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
	unsigned int enqueResult;

	_session->_recvQ.Peek((char*)&CS_Echo, sizeof(CS_Echo), &peekResult);
	if (peekResult < sizeof(CS_Echo))
	{
		RestorePacket(_session, sizeof(CS_Echo), dfPACKET_CS_ECHO);
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