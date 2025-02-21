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

	std::list<Session*>::iterator s_ArrIt;


		for (auto session : SessionArr)
		{
			if (session->_recvQ.IsEmpty() == false) {

				DecodeMessages(session);

			}

		}

		for (auto session : SessionArr)
		{

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




	int sumX = MoveStartPacket.X - _session->_player->_x;
	int sumY = MoveStartPacket.Y - _session->_player->_y;

	if (sumX < 0) sumX *= -1;
	if (sumY < 0) sumY *= -1;
	
	if (sumX >= dfERROR_RANGE || sumY >= dfERROR_RANGE)
	{
		
		std::string logEntry = std::format("Send Sync Message ||old X : {} old Y : {} newX : {} newY : {}  FILE : {}, Func : {} , Line : {} error : {}\n",
			_session->_player->_x, _session->_player->_y, MoveStartPacket.X, MoveStartPacket.Y, getFileName(__FILE__), __func__, __LINE__, GetLastError());
		EnterCriticalSection(&g_lock);
		LogQ.push_back(logEntry);
		LeaveCriticalSection(&g_lock);

		SyncPos(_session, _session->_player->_x, _session->_player->_y, MoveStartPacket.X, MoveStartPacket.Y);
	}

	/*
	else 
	{
		int oldSectorX = _session->_player->_x / SECTOR_RATIO;
		int oldSectorY = _session->_player->_y / SECTOR_RATIO;

		int newSectorX = MoveStartPacket.X / SECTOR_RATIO;
		int newSectorY = MoveStartPacket.Y / SECTOR_RATIO;;

		_session->_player->_x = MoveStartPacket.X;
		_session->_player->_y = MoveStartPacket.Y;
		_session->_player->_direction = MoveStartPacket.Direction;

		if (oldSectorX != newSectorX || oldSectorY != newSectorY)
		{

			Sector[oldSectorX][oldSectorY].remove(_session);
			Sector[newSectorX][newSectorY].push_back(_session);
			SyncSector(_session, oldSectorX, oldSectorY, newSectorX, newSectorY);
		}

	}

	//*/



	_session->_player->MoveStart(MoveStartPacket.Direction, MoveStartPacket.X, MoveStartPacket.Y);
	//명시적으로 인자를 direction만 받는게 나아보임, 안 받던가


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
	

	int sumX = MoveStopPacket.X - _session->_player->_x;
	int sumY = MoveStopPacket.Y - _session->_player->_y;

	if (sumX < 0) sumX *= -1;
	if (sumY < 0) sumY *= -1;

	if (sumX >= dfERROR_RANGE || sumY >= dfERROR_RANGE)
	{
		std::string logEntry = std::format("Send Sync Message ||old X : {} old Y : {} newX : {} newY : {}  FILE : {}, Func : {} , Line : {} error : {}\n",
			_session->_player->_x, _session->_player->_y, MoveStopPacket.X, MoveStopPacket.Y, getFileName(__FILE__), __func__, __LINE__, GetLastError());
		EnterCriticalSection(&g_lock);
		LogQ.push_back(logEntry);
		LeaveCriticalSection(&g_lock);

		SyncPos(_session, _session->_player->_x, _session->_player->_y, MoveStopPacket.X, MoveStopPacket.Y);
	}
	/*
	else
	{
		int oldSectorX = _session->_player->_x / SECTOR_RATIO;
		int oldSectorY = _session->_player->_y / SECTOR_RATIO;

		int newSectorX = MoveStopPacket.X / SECTOR_RATIO;
		int newSectorY = MoveStopPacket.Y / SECTOR_RATIO;;

		_session->_player->_x = MoveStopPacket.X;
		_session->_player->_y = MoveStopPacket.Y;
		_session->_player->_direction = MoveStopPacket.Direction;

		if (oldSectorX != newSectorX || oldSectorY != newSectorY)
		{

			Sector[oldSectorX][oldSectorY].remove(_session);
			Sector[newSectorX][newSectorY].push_back(_session);
			SyncSector(_session, oldSectorX, oldSectorY, newSectorX, newSectorY);
		}
	}

	//*/

	_session->_player->MoveStop(MoveStopPacket.Direction, MoveStopPacket.X, MoveStopPacket.Y);


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
	int sumX = AttackPacket.X - _session->_player->_x;
	int sumY = AttackPacket.Y - _session->_player->_y;

	if (sumX < 0) sumX *= -1;
	if (sumY < 0) sumY *= -1;

	if (sumX >= dfERROR_RANGE || sumY >= dfERROR_RANGE)
	{
		std::string logEntry = std::format("Send Sync Message ||old X : {} old Y : {} newX : {} newY : {}  FILE : {}, Func : {} , Line : {} error : {}\n",
			_session->_player->_x, _session->_player->_y, AttackPacket.X, AttackPacket.Y, getFileName(__FILE__), __func__, __LINE__, GetLastError());
		EnterCriticalSection(&g_lock);
		LogQ.push_back(logEntry);
		LeaveCriticalSection(&g_lock);


		SyncPos(_session, _session->_player->_x, _session->_player->_y, AttackPacket.X, AttackPacket.Y);
	}
	/*
	else
	{
		int oldSectorX = _session->_player->_x / SECTOR_RATIO;
		int oldSectorY = _session->_player->_y / SECTOR_RATIO;

		int newSectorX = AttackPacket.X / SECTOR_RATIO;
		int newSectorY = AttackPacket.Y / SECTOR_RATIO;;

		_session->_player->_x = AttackPacket.X;
		_session->_player->_y = AttackPacket.Y;
		_session->_player->_direction = AttackPacket.Direction;

		if (oldSectorX != newSectorX || oldSectorY != newSectorY)
		{

			Sector[oldSectorX][oldSectorY].remove(_session);
			Sector[newSectorX][newSectorY].push_back(_session);
			SyncSector(_session, oldSectorX, oldSectorY, newSectorX, newSectorY);
		}

	}

	//*/

	_session->_player->_direction = AttackPacket.Direction;

	//먼저 어택 메세지를 섹터에 뿌려줌

	int sectorX = (_session->_player->_x) / SECTOR_RATIO;
	int sectorY = (_session->_player->_y) / SECTOR_RATIO;

	MsgSectorBroadCasting(SendAttack1Message, (char*)_session, (char*)&AttackPacket, sectorX, sectorY, true);


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


					int resultX = AttackPacket.X - (*it)->_player->_x;
					int resultY = AttackPacket.Y - (*it)->_player->_y;

					if (resultX < 0) continue;
					if (resultY < 0) resultY *= -1;

					if (resultX > dfATTACK1_RANGE_X || resultY > dfATTACK1_RANGE_Y) continue;
					//타격 성공 
					(*it)->_player->_hp -= dfAttack1Damage;


					//데미지 메세지 보내기

					std::list<Session*>::iterator FuncIt;

					MsgSectorBroadCasting(SendDamageMessage, (char*)_session, (char*)(*it), sectorX, sectorY, true);

					//사망 판정 //

					if ((*it)->_player->_hp <= 0)
					{
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


					int resultX = AttackPacket.X - (*it)->_player->_x;
					int resultY = AttackPacket.Y - (*it)->_player->_y;

					if (resultX > 0) continue;
					if (resultY < 0) resultY *= -1;
					resultX *= -1;

					if (resultX > dfATTACK1_RANGE_X || resultY > dfATTACK1_RANGE_Y) continue;

					//타격 성공 
					(*it)->_player->_hp -= dfAttack1Damage;


					//데미지 메세지 보내기

					std::list<Session*>::iterator FuncIt;

					MsgSectorBroadCasting(SendDamageMessage, (char*)_session,(char*)(*it), sectorX, sectorY, true);

					//사망 판정 //

					if ((*it)->_player->_hp <= 0)
					{
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

	_session->_recvQ.Peek((char*)&AttackPacket, sizeof(AttackPacket), &peekResult);

	if (peekResult < sizeof(AttackPacket))
	{
		RestorePacket(_session, sizeof(AttackPacket), dfPACKET_CS_ATTACK2);
		return false;
	}

	_session->_recvQ.MoveFront(sizeof(AttackPacket));

	//싱크 작업 먼저 
	int sumX = AttackPacket.X - _session->_player->_x;
	int sumY = AttackPacket.Y - _session->_player->_y;

	if (sumX < 0) sumX *= -1;
	if (sumY < 0) sumY *= -1;

	if (sumX >= dfERROR_RANGE || sumY >= dfERROR_RANGE)
	{
		std::string logEntry = std::format("Send Sync Message ||old X : {} old Y : {} newX : {} newY : {}  FILE : {}, Func : {} , Line : {} error : {}\n",
			_session->_player->_x, _session->_player->_y, AttackPacket.X, AttackPacket.Y, getFileName(__FILE__), __func__, __LINE__, GetLastError());
		EnterCriticalSection(&g_lock);
		LogQ.push_back(logEntry);
		LeaveCriticalSection(&g_lock);

		SyncPos(_session, _session->_player->_x, _session->_player->_y, AttackPacket.X, AttackPacket.Y);
	}
	/*
	else
	{
		int oldSectorX = _session->_player->_x / SECTOR_RATIO;
		int oldSectorY = _session->_player->_y / SECTOR_RATIO;

		int newSectorX = AttackPacket.X / SECTOR_RATIO;
		int newSectorY = AttackPacket.Y / SECTOR_RATIO;;

		_session->_player->_x = AttackPacket.X;
		_session->_player->_y = AttackPacket.Y;
		_session->_player->_direction = AttackPacket.Direction;

		if (oldSectorX != newSectorX || oldSectorY != newSectorY)
		{

			Sector[oldSectorX][oldSectorY].remove(_session);
			Sector[newSectorX][newSectorY].push_back(_session);
			SyncSector(_session, oldSectorX, oldSectorY, newSectorX, newSectorY);
		}

	}
	//*/

	_session->_player->_direction = AttackPacket.Direction;


	int sectorX = (_session->_player->_x) / SECTOR_RATIO;
	int sectorY = (_session->_player->_y) / SECTOR_RATIO;


	//먼저 어택 메세지를 섹터에 뿌려줌
	MsgSectorBroadCasting(SendAttack2Message, (char*)_session, (char*)&AttackPacket, sectorX, sectorY, true);

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


					int resultX = AttackPacket.X - (*it)->_player->_x;
					int resultY = AttackPacket.Y - (*it)->_player->_y;

					if (resultX < 0) continue;
					if (resultY < 0) resultY *= -1;

					if (resultX > dfATTACK2_RANGE_X || resultY > dfATTACK2_RANGE_Y) continue;

					//타격 성공 
					(*it)->_player->_hp -= dfAttack2Damage;

					MsgSectorBroadCasting(SendDamageMessage, (char*)_session, (char*)(*it), sectorX, sectorY, true);

					if ((*it)->_player->_hp <= 0)
					{
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


					int resultX = AttackPacket.X - (*it)->_player->_x;
					int resultY = AttackPacket.Y - (*it)->_player->_y;

					if (resultX > 0) continue;
					if (resultY < 0) resultY *= -1;
					resultX *= -1;

					if (resultX > dfATTACK2_RANGE_X || resultY > dfATTACK2_RANGE_Y) continue;

					//타격 성공
					(*it)->_player->_hp -= dfAttack2Damage;

					MsgSectorBroadCasting(SendDamageMessage, (char*)_session, (char*)(*it), sectorX, sectorY, true);

					if ((*it)->_player->_hp <= 0)
					{
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

	_session->_recvQ.Peek((char*)&AttackPacket, sizeof(AttackPacket), &peekResult);

	if (peekResult < sizeof(AttackPacket))
	{

		RestorePacket(_session, sizeof(AttackPacket), dfPACKET_CS_ATTACK3);
		return false;
	}

	_session->_recvQ.MoveFront(sizeof(AttackPacket));


	//싱크 작업 먼저 
	int sumX = AttackPacket.X - _session->_player->_x;
	int sumY = AttackPacket.Y - _session->_player->_y;

	if (sumX < 0) sumX *= -1;
	if (sumY < 0) sumY *= -1;

	if (sumX >= dfERROR_RANGE || sumY >= dfERROR_RANGE)
	{

		std::string logEntry = std::format("Send Sync Message ||old X : {} old Y : {} newX : {} newY : {}  FILE : {}, Func : {} , Line : {} error : {}\n",
			_session->_player->_x, _session->_player->_y, AttackPacket.X, AttackPacket.Y, getFileName(__FILE__), __func__, __LINE__, GetLastError());
		EnterCriticalSection(&g_lock);
		LogQ.push_back(logEntry);
		LeaveCriticalSection(&g_lock);



		SyncPos(_session, _session->_player->_x, _session->_player->_y, AttackPacket.X, AttackPacket.Y);
	}
	/*
	else
	{
		int oldSectorX = _session->_player->_x / SECTOR_RATIO;
		int oldSectorY = _session->_player->_y / SECTOR_RATIO;

		int newSectorX = AttackPacket.X / SECTOR_RATIO;
		int newSectorY = AttackPacket.Y / SECTOR_RATIO;;

		_session->_player->_x = AttackPacket.X;
		_session->_player->_y = AttackPacket.Y;
		_session->_player->_direction = AttackPacket.Direction;

		if (oldSectorX != newSectorX || oldSectorY != newSectorY)
		{

			Sector[oldSectorX][oldSectorY].remove(_session);
			Sector[newSectorX][newSectorY].push_back(_session);
			SyncSector(_session, oldSectorX, oldSectorY, newSectorX, newSectorY);
		}

	}
	//*/

	_session->_player->_direction = AttackPacket.Direction;

	int sectorX = (_session->_player->_x) / SECTOR_RATIO;
	int sectorY = (_session->_player->_y) / SECTOR_RATIO;


	//먼저 어택 메세지를 섹터에 뿌려줌
	MsgSectorBroadCasting(SendAttack3Message, (char*)_session, (char*)&AttackPacket, sectorX, sectorY, true);

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


					int resultX = AttackPacket.X - (*it)->_player->_x;
					int resultY = AttackPacket.Y - (*it)->_player->_y;

					if (resultX < 0) continue;
					if (resultY < 0) resultY *= -1;

					if (resultX > dfATTACK3_RANGE_X || resultY > dfATTACK3_RANGE_Y) continue;
					//타격 성공 

					(*it)->_player->_hp -= dfAttack3Damage;


					//데미지 메세지 보내기
					MsgSectorBroadCasting(SendDamageMessage, (char*)_session, (char*)(*it), sectorX, sectorY, true);
					if ((*it)->_player->_hp <= 0)
					{
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


					int resultX = AttackPacket.X - (*it)->_player->_x;
					int resultY = AttackPacket.Y - (*it)->_player->_y;


					if (resultX > 0) continue;
					if (resultY < 0) resultY *= -1;
					resultX *= -1;

					if (resultX > dfATTACK3_RANGE_X || resultY > dfATTACK3_RANGE_Y) continue;


					//타격 성공 
					(*it)->_player->_hp -= dfAttack3Damage;


					//데미지 메세지 보내기
					MsgSectorBroadCasting(SendDamageMessage, (char*)_session, (char*)(*it), sectorX, sectorY, true);
					if ((*it)->_player->_hp <= 0)
					{
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