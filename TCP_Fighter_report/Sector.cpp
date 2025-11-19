#include "Sector.h"
#include "Message.h"


std::list<Session*> Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];
int sectorXRange = dfRANGE_MOVE_RIGHT / SECTOR_RATIO;
int sectorYRange = dfRANGE_MOVE_BOTTOM / SECTOR_RATIO;

extern std::list<Session*> SessionArr;
static std::list<Session*>::iterator stat_ArrIt;

extern CRITICAL_SECTION g_lock;


bool MoveSectorL(Session* _session, int x, int y, int oldX, int oldY)
{	
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

		if (oldSectorX < 0 || oldSectorX >= sectorRightMax) break;
		if (oldSectorY < 0 || oldSectorY >= sectorBottomMax) continue;


		for (stat_ArrIt = Sector[oldSectorX][oldSectorY].begin();
			stat_ArrIt != Sector[oldSectorX][oldSectorY].end(); stat_ArrIt++)
		{
			if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;
			if ((*stat_ArrIt)->_delete == true) continue;

			SendDeleteMessage((char*)_session, (char*)(*stat_ArrIt));
			SendDeleteMessage((char*)(*stat_ArrIt), (char*)_session);
			

		}
	}
	//새로운 섹터에 내 캐릭터 생성 메세지 + 새로운 섹터 캐릭터 생성 메세지를 나에게 보내기

	for (int i = -1; i < 2; i++)
	{
		newSectorX = x / SECTOR_RATIO - 1;
		newSectorY = y / SECTOR_RATIO + i;


		if (newSectorX< 0 || newSectorX>= sectorRightMax) break;
		if (newSectorY< 0 || newSectorY>= sectorBottomMax) continue;

		for (stat_ArrIt = Sector[newSectorX][newSectorY].begin();
			stat_ArrIt != Sector[newSectorX][newSectorY].end(); stat_ArrIt++)
		{

			if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;
			if ((*stat_ArrIt)->_delete == true) continue;

			SendCreateOtherCharMessage((char*)_session, (char*)(*stat_ArrIt));
			SendCreateOtherCharMessage((char*)(*stat_ArrIt), (char*)_session);

			//MoveMessage 보내야 함 
			if (_session->_player->_move == true)
			{
				SendMoveStartMessage((char*)_session, (char*)(*stat_ArrIt));

			}
			if ((*stat_ArrIt)->_player->_move == true)
			{
				SendMoveStartMessage((char*)(*stat_ArrIt), (char*)_session);
			}

		}
	}
	

	return true;
}
bool MoveSectorR(Session* _session, int x, int y, int oldX, int oldY)
{

	int oldSectorX;
	int oldSectorY;

	int newSectorX;
	int newSectorY;

	int sectorRightMax = dfRANGE_MOVE_RIGHT / SECTOR_RATIO;
	int sectorBottomMax = dfRANGE_MOVE_BOTTOM / SECTOR_RATIO;

	// 이전 섹터에 내 세션 딜리트 메세지 보내주기//
	
	for (int i = -1; i < 2; i++)
	{
		oldSectorX = (oldX / SECTOR_RATIO) - 1;
		oldSectorY = (oldY / SECTOR_RATIO) + i;

		if (oldSectorX< 0 || oldSectorX>= sectorRightMax) break;
		if (oldSectorY< 0 || oldSectorY>= sectorBottomMax) continue;


		for (auto dest : Sector[oldSectorX][oldSectorY])
		{
			if (dest->_player->_ID == _session->_player->_ID) continue;
			if (dest->_delete == true) continue;

			SendDeleteMessage((char*)_session, (char*)dest);
			SendDeleteMessage((char*)dest, (char*)_session);
		}
	}

	//새로운 섹터에 내 캐릭터 생성 메세지 + 새로운 섹터 캐릭터 생성 메세지를 나에게 보내기

	for (int i = -1; i < 2; i++)
	{
		newSectorX = x / SECTOR_RATIO + 1;
		newSectorY = y / SECTOR_RATIO + i;
		if (newSectorX< 0 || newSectorX>= sectorRightMax) break;
		if (newSectorY< 0 || newSectorY>= sectorBottomMax) continue;

		for (auto dest : Sector[newSectorX][newSectorY])
		{
			if (dest->_player->_ID == _session->_player->_ID) continue;
			if (dest->_delete == true) continue;

			SendCreateOtherCharMessage((char*)_session, (char*)dest);
			SendCreateOtherCharMessage((char*)dest, (char*)_session);


			//MoveMessage 보내야 함 
			if (_session->_player->_move == true)
			{
				SendMoveStartMessage((char*)_session, (char*)dest);

			}
			if (dest->_player->_move == true)
			{
				SendMoveStartMessage((char*)dest, (char*)_session);
			}




		}
	}


	return true;
}
bool MoveSectorU(Session* _session, int x, int y, int oldX, int oldY)
{

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


		if (oldSectorY< 0 || oldSectorY>= sectorBottomMax) break;
		if (oldSectorX< 0 || oldSectorX>= sectorRightMax) continue;


		for (auto dest : Sector[oldSectorX][oldSectorY])
		{
			if (dest->_player->_ID == _session->_player->_ID) continue;
			if (dest->_delete == true) continue;

			SendDeleteMessage((char*)_session, (char*)dest);
			SendDeleteMessage((char*)dest, (char*)_session);

		}
	}
	//새로운 섹터에 내 캐릭터 생성 메세지 + 새로운 섹터 캐릭터 생성 메세지를 나에게 보내기

	for (int i = -1; i < 2; i++)
	{
		newSectorX = x / SECTOR_RATIO + i;
		newSectorY = y / SECTOR_RATIO - 1;

		if (newSectorY < 0|| newSectorY>= sectorBottomMax) break;
		if (newSectorX< 0 || newSectorX>= sectorRightMax) continue;

		for (auto dest : Sector[newSectorX][newSectorY])
		{
			if (dest->_player->_ID == _session->_player->_ID) continue;
			if (dest->_delete == true) continue;

			SendCreateOtherCharMessage((char*)_session, (char*)dest);
			SendCreateOtherCharMessage((char*)dest, (char*)_session);


			//MoveMessage 보내야 함 
			if (_session->_player->_move == true)
			{
				SendMoveStartMessage((char*)_session, (char*)dest);

			}
			if (dest->_player->_move == true)
			{
				SendMoveStartMessage((char*)dest, (char*)_session);
			}


		}
	}


	return true;
}
bool MoveSectorD(Session* _session, int x, int y, int oldX, int oldY)
{
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

		if (oldSectorY< 0 ||oldSectorY>= sectorBottomMax) break;
		if (oldSectorX< 0 ||oldSectorX>= sectorRightMax) continue;


		for (stat_ArrIt = Sector[oldSectorX][oldSectorY].begin();
			stat_ArrIt != Sector[oldSectorX][oldSectorY].end(); stat_ArrIt++)
		{
			if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;
			if ((*stat_ArrIt)->_delete == true) continue;

			SendDeleteMessage((char*)_session, (char*)(*stat_ArrIt));
			SendDeleteMessage((char*)(*stat_ArrIt), (char*)_session);

		}
	}
	//새로운 섹터에 내 캐릭터 생성 메세지 + 새로운 섹터 캐릭터 생성 메세지를 나에게 보내기

	for (int i = -1; i < 2; i++)
	{
		newSectorX = x / SECTOR_RATIO + i;
		newSectorY = y / SECTOR_RATIO + 1;


		if (newSectorY< 0 || newSectorY>= sectorBottomMax) break;
		if (newSectorX< 0 || newSectorX>= sectorRightMax) continue;

		for (stat_ArrIt = Sector[newSectorX][newSectorY].begin();
			stat_ArrIt != Sector[newSectorX][newSectorY].end(); stat_ArrIt++)
		{
			if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;
			if ((*stat_ArrIt)->_delete == true) continue;

			SendCreateOtherCharMessage((char*)_session, (char*)(*stat_ArrIt));
			SendCreateOtherCharMessage((char*)(*stat_ArrIt), (char*)_session);

			//MoveMessage 보내야 함 
			if (_session->_player->_move == true)
			{
				SendMoveStartMessage((char*)_session, (char*)(*stat_ArrIt));

			}
			if ((*stat_ArrIt)->_player->_move == true)
			{
				SendMoveStartMessage((char*)(*stat_ArrIt), (char*)_session);
			}

		}
	}


	return true;
}


bool SyncSector(Session* _session, int oldSecX, int oldSecY,  int newSecX, int newSecY)
{

	//이전 섹터 주변에 내 세션 삭제 메세지 보내기
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{

			if (oldSecX + i < 0 || oldSecX + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
			if (oldSecY + j < 0 || oldSecY + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;

			for (stat_ArrIt = Sector[oldSecX + i][oldSecY + j].begin();
				stat_ArrIt != Sector[oldSecX + i][oldSecY + j].end(); stat_ArrIt++)
			{
				if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;
				if ((*stat_ArrIt)->_delete == true) continue;
				
				SendDeleteMessage((char*)_session, (char*)(*stat_ArrIt));
				SendDeleteMessage((char*)(*stat_ArrIt), (char*)_session);


			}
		}
	}

	//이번 섹터 주변에 내 섹터 생성 메세지 전송

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{

			if (newSecX + i < 0 || newSecX + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
			if (newSecY + j < 0 || newSecY + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;

			for (stat_ArrIt = Sector[newSecX + i][newSecY + j].begin();
				stat_ArrIt != Sector[newSecX + i][newSecY + j].end(); stat_ArrIt++)
			{
				//섹터 생성 메세지 
				if ((*stat_ArrIt)->_player->_ID == _session->_player->_ID) continue;
				if ((*stat_ArrIt)->_delete == true) continue;

				SendCreateOtherCharMessage((char*)_session, (char*)(*stat_ArrIt));
				SendCreateOtherCharMessage((char*)(*stat_ArrIt), (char*)_session);
				
				
				//*

				//MoveMessage
				if (_session->_player->_move == true)
				{
					SendMoveStartMessage((char*)_session, (char*)(*stat_ArrIt));

				}
				if ((*stat_ArrIt)->_player->_move == true)
				{
					SendMoveStartMessage((char*)(*stat_ArrIt), (char*)_session);
				}

				//*/


			}
		}
	}
	
	


	return true;
}


Session* CheckHit(Session* _session, int rangeX, int rangeY)
{
	int sectorX = _session->_player->_x / SECTOR_RATIO;
	int sectorY = _session->_player->_y / SECTOR_RATIO;


	if ((_session->_player->_direction) == LL) //left
	{
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (sectorX + i < 0 || sectorX + i >= sectorXRange) continue;
				if (sectorY + j < 0 || sectorY + j >= sectorYRange) continue;

				for (auto target : Sector[sectorX + i][sectorY + j])
				{
					if (target->_player->_ID == _session->_player->_ID) continue;
					if (target->_delete == true) continue;

					int resultX = _session->_player->_x - target->_player->_x;
					int resultY = _session->_player->_y - target->_player->_y;

					if (resultX < 0) continue;
					if (resultY < 0) resultY *= -1;
					
					if (resultX < rangeX && resultY < rangeY) return target;

				}
			}
		}

	}
	else //방향이 RR이라면
	{
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (sectorX + i < 0 || sectorX + i >= sectorXRange) continue;
				if (sectorY + j < 0 || sectorY + j >= sectorYRange) continue;


				for (auto target : Sector[sectorX + i][sectorY + j])
				{
					if (target->_player->_ID == _session->_player->_ID) continue;
					if (target->_delete == true) continue;

					int resultX = _session->_player->_x - target->_player->_x;
					int resultY = _session->_player->_y - target->_player->_y;

					if (resultX > 0) continue;
					if (resultY < 0) resultY *= -1;
					resultX *= -1;

					if (resultX < rangeX && resultY < rangeY) return target;

				}
			}
		}

	}

	return HIT_FAILED;
}

