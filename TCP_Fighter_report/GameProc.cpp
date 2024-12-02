#include "GameProc.h"

extern Session SessionArr[PLAYERMAXCOUNT];

extern DWORD playerCount;

bool MoveStart(Session* _session)
{
	CS_MOVE_START MoveStartPacket;

	if (_session->_recvQ.Peek((char*)&MoveStartPacket, sizeof(MoveStartPacket)) < sizeof(MoveStartPacket)) {
		
		printf("Peek error, Line : %d, front : %d, rear : %d\n", __LINE__, _session->_recvQ._front, _session->_recvQ._rear);
		return false;
	}
	
	_session->_recvQ.MoveFront(sizeof(MoveStartPacket));

	_session->_player->MoveStart(MoveStartPacket.Direction, MoveStartPacket.X, MoveStartPacket.Y);

	printf("Player ID : %d, Direction : %d, MoveStart \n", _session->_player->_ID, _session->_player->_direction);

	return true;
}


void MoveStop(Session* _session)
{
	_session->_player->MoveStop();
	printf("Player ID : %d, Direction : %d, MoveStop  \n", _session->_player->_ID, _session->_player->_direction);

}