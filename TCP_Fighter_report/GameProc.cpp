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

	printf("Player ID : %d, Direction : %d, X: %d, Y : %d MoveStart \n", _session->_player->_ID, _session->_player->_direction
	, _session->_player->_x, _session->_player->_y);

	//다른 캐릭터들 send q 에 move start enqueue

	
	PacketHeader MoveHeader;
	SC_MOVE_START SC_MoveStart_Packet;
	
	MoveHeader.byCode = 0x89;
	MoveHeader.bySize = sizeof(SC_MoveStart_Packet);
	MoveHeader.byType = dfPACKET_SC_MOVE_START;



	
	
	SC_MoveStart_Packet.Direction = MoveStartPacket.Direction;
	SC_MoveStart_Packet.X = MoveStartPacket.X;
	SC_MoveStart_Packet.Y = MoveStartPacket.Y;
	SC_MoveStart_Packet.ID = _session->_player->_ID;




	for (int i = 0; i < playerCount; i++)
	{
		if (SessionArr[i]._player->_ID != _session->_player->_ID) {

			SessionArr[i]._sendQ.Enqueue((char*)&MoveHeader, sizeof(MoveHeader));
			SessionArr[i]._sendQ.Enqueue((char*)&SC_MoveStart_Packet, sizeof(SC_MoveStart_Packet)); //enqueue가 실패할 경우에 대한 예외처리 필요
		}
	}




	return true;
}


void MoveStop(Session* _session)
{

	CS_MOVE_STOP MoveStopPacket;
	if (_session->_recvQ.Peek((char*)&MoveStopPacket, sizeof(MoveStopPacket)) < sizeof(MoveStopPacket)) {
		printf("Line : %d, recvQ Peek Error", __LINE__); 
		return;
	}

	_session->_recvQ.MoveFront(sizeof(MoveStopPacket));

	_session->_player->MoveStop();
	
	_session->_player->_direction = MoveStopPacket.Direction;
	_session->_player->_x = MoveStopPacket.X;
	_session->_player->_y = MoveStopPacket.Y;

	//다른 캐릭터들 send q에 move stop을 넣어줘야 함

	PacketHeader MoveHeader;
	SC_MOVE_STOP SC_MoveStopPacket;

	MoveHeader.byCode = 0x89;
	MoveHeader.bySize = sizeof(SC_MoveStopPacket);
	MoveHeader.byType = dfPACKET_SC_MOVE_STOP;


	SC_MoveStopPacket.Direction = MoveStopPacket.Direction;
	SC_MoveStopPacket.X = MoveStopPacket.X;
	SC_MoveStopPacket.Y = MoveStopPacket.Y;
	SC_MoveStopPacket.ID = _session->_player->_ID;

	for (int i = 0; i < playerCount; i++)
	{
		if (SessionArr[i]._player->_ID != _session->_player->_ID)
		{
			SessionArr[i]._sendQ.Enqueue((char*)&MoveHeader, sizeof(MoveHeader));
			SessionArr[i]._sendQ.Enqueue((char*)&SC_MoveStopPacket, sizeof(SC_MoveStopPacket)); //enqueue실패에 대한 예외처리도 필요
		}
	}





	printf("Player ID : %d, Direction : %d, MoveStop  \n", _session->_player->_ID, _session->_player->_direction);

}


bool Attack1(Session* _session)
{
	CS_ATTACK1 AttackPacket;
	if (_session->_recvQ.Peek((char*)&AttackPacket, sizeof(AttackPacket)) < sizeof(AttackPacket))
	{
		printf("Line : %d, Peek error", __LINE__);
		return false;
	}

	_session->_recvQ.MoveFront(sizeof(AttackPacket));

	_session->_player->_direction = AttackPacket.Direction;

	PacketHeader AttackHeader;
	SC_ATTACK1 SCAttackPacket;

	AttackHeader.byCode = 0x89;
	AttackHeader.bySize = sizeof(SCAttackPacket);
	AttackHeader.byType = dfPACKET_SC_ATTACK1;

	SCAttackPacket.Direction = AttackPacket.Direction;
	SCAttackPacket.X = AttackPacket.X;
	SCAttackPacket.Y = AttackPacket.Y;
	SCAttackPacket.ID = _session->_player->_ID;

	for (int i = 0; i < playerCount; i++)
	{
		if (SessionArr[i]._player->_ID != _session->_player->_ID)
		{
			SessionArr[i]._sendQ.Enqueue((char*)&AttackHeader, sizeof(AttackHeader));
			SessionArr[i]._sendQ.Enqueue((char*)&SCAttackPacket, sizeof(SCAttackPacket));
		}

	}




	if ((AttackPacket.Direction) == LL) //left
	{
		printf("attack Left ! \n");
		for (int i = 0; i < playerCount; i++)
		{
			
			//타격 판정

			if ((AttackPacket.X - SessionArr[i]._player->_x) >= 0 && (AttackPacket.X - SessionArr[i]._player->_x) <= dfATTACK1_RANGE_X && SessionArr[i]._player->_ID != _session->_player->_ID)
			{
				if ((AttackPacket.Y - SessionArr[i]._player->_y >= 0 && AttackPacket.Y - SessionArr[i]._player->_y < dfATTACK1_RANGE_Y) || 
					(SessionArr[i]._player->_y - AttackPacket.Y >= 0 && SessionArr[i]._player->_y - AttackPacket.Y < dfATTACK1_RANGE_Y))
				{
					//타격 성공 


					SessionArr[i]._player->_hp -= dfAttack1Damage;

					for (int j = 0; j < playerCount; j++)
					{
						//데미지 메세지 보내기

						PacketHeader DamageHeader;
						SC_DAMAGE DamagePacket;

						DamageHeader.byCode = 0x89;
						DamageHeader.bySize = sizeof(DamagePacket);
						DamageHeader.byType = dfPACKET_SC_DAMAGE;

						DamagePacket.AttackID = _session->_player->_ID;
						DamagePacket.DamageID = SessionArr[i]._player->_ID;
						DamagePacket.DamageHP = SessionArr[i]._player->_hp;
							 
						SessionArr[j]._sendQ.Enqueue((char*)&DamageHeader, sizeof(DamageHeader));
						SessionArr[j]._sendQ.Enqueue((char*)&DamagePacket, sizeof(DamagePacket));
					}

					if (SessionArr[i]._player->_hp <= 0)
					{
						PacketHeader DeleteHeader;
						SC_DELETE_CHARACTER DeletePacket;

						DeleteHeader.byCode = 0x89;
						DeleteHeader.bySize = sizeof(DeletePacket);
						DeleteHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

						DeletePacket.ID = SessionArr[i]._player->_ID;

						for (int j = 0; j < playerCount; j++)
						{
							SessionArr[j]._sendQ.Enqueue((char*)&DeleteHeader, sizeof(DeleteHeader));
							SessionArr[j]._sendQ.Enqueue((char*)&DeletePacket, sizeof(DeletePacket));
						}

						SessionArr[i]._delete = true;
						//소켓 연결도 끊어줘야 함..
					}


				}
			}

		}

	}

	else
	{
		printf("attack Right ! \n");
		for (int i = 0; i < playerCount; i++)
		{

			//타격 판정

			if ((SessionArr[i]._player->_x - AttackPacket.X) >= 0 && (SessionArr[i]._player->_x - AttackPacket.X) <= dfATTACK1_RANGE_X && SessionArr[i]._player->_ID != _session->_player->_ID)
			{
				if ((AttackPacket.Y - SessionArr[i]._player->_y >= 0 && AttackPacket.Y - SessionArr[i]._player->_y < dfATTACK1_RANGE_Y) ||
					(SessionArr[i]._player->_y - AttackPacket.Y >= 0 && SessionArr[i]._player->_y - AttackPacket.Y < dfATTACK1_RANGE_Y))
				{
					//타격 성공 
					SessionArr[i]._player->_hp -= dfAttack1Damage;

					for (int j = 0; j < playerCount; j++)
					{
						//데미지 메세지 보내기

						PacketHeader DamageHeader;
						SC_DAMAGE DamagePacket;

						DamageHeader.byCode = 0x89;
						DamageHeader.bySize = sizeof(DamagePacket);
						DamageHeader.byType = dfPACKET_SC_DAMAGE;

						DamagePacket.AttackID = _session->_player->_ID;
						DamagePacket.DamageID = SessionArr[i]._player->_ID;
						DamagePacket.DamageHP = SessionArr[i]._player->_hp;

						SessionArr[j]._sendQ.Enqueue((char*)&DamageHeader, sizeof(DamageHeader));
						SessionArr[j]._sendQ.Enqueue((char*)&DamagePacket, sizeof(DamagePacket));
					}

					if (SessionArr[i]._player->_hp <= 0)
					{
						PacketHeader DeleteHeader;
						SC_DELETE_CHARACTER DeletePacket;

						DeleteHeader.byCode = 0x89;
						DeleteHeader.bySize = sizeof(DeletePacket);
						DeleteHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

						DeletePacket.ID = SessionArr[i]._player->_ID;

						for (int j = 0; j < playerCount; j++)
						{
							SessionArr[j]._sendQ.Enqueue((char*)&DeleteHeader, sizeof(DeleteHeader));
							SessionArr[j]._sendQ.Enqueue((char*)&DeletePacket, sizeof(DeletePacket));
						}


						//소켓 연결도 끊어줘야 함..
						SessionArr[i]._delete = true;
					}


				}
			}

		}

	}


	return true;
}

bool Attack2(Session* _session)
{
	CS_ATTACK2 AttackPacket;
	if (_session->_recvQ.Peek((char*)&AttackPacket, sizeof(AttackPacket)) < sizeof(AttackPacket))
	{
		printf("Line : %d, Peek error", __LINE__);
		return false;
	}

	_session->_recvQ.MoveFront(sizeof(AttackPacket));

	_session->_player->_direction = AttackPacket.Direction;

	PacketHeader AttackHeader;
	SC_ATTACK2 SCAttackPacket;

	AttackHeader.byCode = 0x89;
	AttackHeader.bySize = sizeof(SCAttackPacket);
	AttackHeader.byType = dfPACKET_SC_ATTACK2;

	SCAttackPacket.Direction = AttackPacket.Direction;
	SCAttackPacket.X = AttackPacket.X;
	SCAttackPacket.Y = AttackPacket.Y;
	SCAttackPacket.ID = _session->_player->_ID;

	for (int i = 0; i < playerCount; i++)
	{
		if (SessionArr[i]._player->_ID != _session->_player->_ID)
		{
			SessionArr[i]._sendQ.Enqueue((char*)&AttackHeader, sizeof(AttackHeader));
			SessionArr[i]._sendQ.Enqueue((char*)&SCAttackPacket, sizeof(SCAttackPacket));
		}

	}




	if ((AttackPacket.Direction) == LL) //left
	{
		printf("attack Left ! \n");
		for (int i = 0; i < playerCount; i++)
		{

			//타격 판정

			if ((AttackPacket.X - SessionArr[i]._player->_x) >= 0 && (AttackPacket.X - SessionArr[i]._player->_x) <= dfATTACK2_RANGE_X && SessionArr[i]._player->_ID != _session->_player->_ID)
			{
				if ((AttackPacket.Y - SessionArr[i]._player->_y >= 0 && AttackPacket.Y - SessionArr[i]._player->_y < dfATTACK2_RANGE_Y) ||
					(SessionArr[i]._player->_y - AttackPacket.Y >= 0 && SessionArr[i]._player->_y - AttackPacket.Y < dfATTACK2_RANGE_Y))
				{
					//타격 성공 


					SessionArr[i]._player->_hp -= dfAttack2Damage;

					for (int j = 0; j < playerCount; j++)
					{
						//데미지 메세지 보내기

						PacketHeader DamageHeader;
						SC_DAMAGE DamagePacket;

						DamageHeader.byCode = 0x89;
						DamageHeader.bySize = sizeof(DamagePacket);
						DamageHeader.byType = dfPACKET_SC_DAMAGE;

						DamagePacket.AttackID = _session->_player->_ID;
						DamagePacket.DamageID = SessionArr[i]._player->_ID;
						DamagePacket.DamageHP = SessionArr[i]._player->_hp;

						SessionArr[j]._sendQ.Enqueue((char*)&DamageHeader, sizeof(DamageHeader));
						SessionArr[j]._sendQ.Enqueue((char*)&DamagePacket, sizeof(DamagePacket));
					}

					printf("ID : %d, HP : %d ", SessionArr[i]._player->_ID, SessionArr[i]._player->_hp);

					if (SessionArr[i]._player->_hp <= 0)
					{
						PacketHeader DeleteHeader;
						SC_DELETE_CHARACTER DeletePacket;

						DeleteHeader.byCode = 0x89;
						DeleteHeader.bySize = sizeof(DeletePacket);
						DeleteHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

						DeletePacket.ID = SessionArr[i]._player->_ID;

						for (int j = 0; j < playerCount; j++)
						{
							SessionArr[j]._sendQ.Enqueue((char*)&DeleteHeader, sizeof(DeleteHeader));
							SessionArr[j]._sendQ.Enqueue((char*)&DeletePacket, sizeof(DeletePacket));
						}

						SessionArr[i]._delete = true;
						//소켓 연결도 끊어줘야 함..

						printf(" Send Delete Message !\n");
					}


				}
			}

		}

	}

	else
	{
		printf("attack Right ! \n");
		for (int i = 0; i < playerCount; i++)
		{

			//타격 판정

			if ((SessionArr[i]._player->_x - AttackPacket.X) >= 0 && (SessionArr[i]._player->_x - AttackPacket.X) <= dfATTACK2_RANGE_X && SessionArr[i]._player->_ID != _session->_player->_ID)
			{
				if ((AttackPacket.Y - SessionArr[i]._player->_y >= 0 && AttackPacket.Y - SessionArr[i]._player->_y < dfATTACK2_RANGE_Y) ||
					(SessionArr[i]._player->_y - AttackPacket.Y >= 0 && SessionArr[i]._player->_y - AttackPacket.Y < dfATTACK2_RANGE_Y))
				{
					//타격 성공 
					SessionArr[i]._player->_hp -= dfAttack2Damage;

					for (int j = 0; j < playerCount; j++)
					{
						//데미지 메세지 보내기

						PacketHeader DamageHeader;
						SC_DAMAGE DamagePacket;

						DamageHeader.byCode = 0x89;
						DamageHeader.bySize = sizeof(DamagePacket);
						DamageHeader.byType = dfPACKET_SC_DAMAGE;

						DamagePacket.AttackID = _session->_player->_ID;
						DamagePacket.DamageID = SessionArr[i]._player->_ID;
						DamagePacket.DamageHP = SessionArr[i]._player->_hp;

						SessionArr[j]._sendQ.Enqueue((char*)&DamageHeader, sizeof(DamageHeader));
						SessionArr[j]._sendQ.Enqueue((char*)&DamagePacket, sizeof(DamagePacket));
					}

					if (SessionArr[i]._player->_hp <= 0)
					{
						PacketHeader DeleteHeader;
						SC_DELETE_CHARACTER DeletePacket;

						DeleteHeader.byCode = 0x89;
						DeleteHeader.bySize = sizeof(DeletePacket);
						DeleteHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

						DeletePacket.ID = SessionArr[i]._player->_ID;

						for (int j = 0; j < playerCount; j++)
						{
							SessionArr[j]._sendQ.Enqueue((char*)&DeleteHeader, sizeof(DeleteHeader));
							SessionArr[j]._sendQ.Enqueue((char*)&DeletePacket, sizeof(DeletePacket));
						}

						SessionArr[i]._delete = true;
					}


				}
			}

		}

	}


	return true;
}

bool Attack3(Session* _session)
{
	CS_ATTACK3 AttackPacket;
	if (_session->_recvQ.Peek((char*)&AttackPacket, sizeof(AttackPacket)) < sizeof(AttackPacket))
	{
		printf("Line : %d, Peek error", __LINE__);
		return false;
	}

	_session->_recvQ.MoveFront(sizeof(AttackPacket));

	_session->_player->_direction = AttackPacket.Direction;

	PacketHeader AttackHeader;
	SC_ATTACK3 SCAttackPacket;

	AttackHeader.byCode = 0x89;
	AttackHeader.bySize = sizeof(SCAttackPacket);
	AttackHeader.byType = dfPACKET_SC_ATTACK3;

	SCAttackPacket.Direction = AttackPacket.Direction;
	SCAttackPacket.X = AttackPacket.X;
	SCAttackPacket.Y = AttackPacket.Y;
	SCAttackPacket.ID = _session->_player->_ID;

	for (int i = 0; i < playerCount; i++)
	{
		if (SessionArr[i]._player->_ID != _session->_player->_ID)
		{
			SessionArr[i]._sendQ.Enqueue((char*)&AttackHeader, sizeof(AttackHeader));
			SessionArr[i]._sendQ.Enqueue((char*)&SCAttackPacket, sizeof(SCAttackPacket));
		}

	}




	if ((AttackPacket.Direction) == LL) //left
	{
		
		for (int i = 0; i < playerCount; i++)
		{

			//타격 판정

			if ((AttackPacket.X - SessionArr[i]._player->_x) >= 0 && (AttackPacket.X - SessionArr[i]._player->_x) <= dfATTACK3_RANGE_X && SessionArr[i]._player->_ID != _session->_player->_ID)
			{
				if ((AttackPacket.Y - SessionArr[i]._player->_y >= 0 && AttackPacket.Y - SessionArr[i]._player->_y < dfATTACK3_RANGE_Y) ||
					(SessionArr[i]._player->_y - AttackPacket.Y >= 0 && SessionArr[i]._player->_y - AttackPacket.Y < dfATTACK3_RANGE_Y))
				{
					//타격 성공 


					SessionArr[i]._player->_hp -= dfAttack3Damage;

					for (int j = 0; j < playerCount; j++)
					{
						//데미지 메세지 보내기

						PacketHeader DamageHeader;
						SC_DAMAGE DamagePacket;

						DamageHeader.byCode = 0x89;
						DamageHeader.bySize = sizeof(DamagePacket);
						DamageHeader.byType = dfPACKET_SC_DAMAGE;

						DamagePacket.AttackID = _session->_player->_ID;
						DamagePacket.DamageID = SessionArr[i]._player->_ID;
						DamagePacket.DamageHP = SessionArr[i]._player->_hp;

						SessionArr[j]._sendQ.Enqueue((char*)&DamageHeader, sizeof(DamageHeader));
						SessionArr[j]._sendQ.Enqueue((char*)&DamagePacket, sizeof(DamagePacket));
						//데미지 메세지 전송
					}

					if (SessionArr[i]._player->_hp <= 0)
					{
						PacketHeader DeleteHeader;
						SC_DELETE_CHARACTER DeletePacket;

						DeleteHeader.byCode = 0x89;
						DeleteHeader.bySize = sizeof(DeletePacket);
						DeleteHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

						DeletePacket.ID = SessionArr[i]._player->_ID;

						for (int j = 0; j < playerCount; j++)
						{
							SessionArr[j]._sendQ.Enqueue((char*)&DeleteHeader, sizeof(DeleteHeader));
							SessionArr[j]._sendQ.Enqueue((char*)&DeletePacket, sizeof(DeletePacket));
						}

						SessionArr[i]._delete = true;

					}

				}
			}

		}

	}

	else
	{
		for (int i = 0; i < playerCount; i++)
		{

			//타격 판정

			if ((SessionArr[i]._player->_x - AttackPacket.X) >= 0 && (SessionArr[i]._player->_x - AttackPacket.X) <= dfATTACK3_RANGE_X && SessionArr[i]._player->_ID != _session->_player->_ID)
			{
				if ((AttackPacket.Y - SessionArr[i]._player->_y >= 0 && AttackPacket.Y - SessionArr[i]._player->_y < dfATTACK3_RANGE_Y) ||
					(SessionArr[i]._player->_y - AttackPacket.Y >= 0 && SessionArr[i]._player->_y - AttackPacket.Y < dfATTACK3_RANGE_Y))
				{
					//타격 성공 
					SessionArr[i]._player->_hp -= dfAttack3Damage;

					for (int j = 0; j < playerCount; j++)
					{
						//데미지 메세지 보내기

						PacketHeader DamageHeader;
						SC_DAMAGE DamagePacket;

						DamageHeader.byCode = 0x89;
						DamageHeader.bySize = sizeof(DamagePacket);
						DamageHeader.byType = dfPACKET_SC_DAMAGE;

						DamagePacket.AttackID = _session->_player->_ID;
						DamagePacket.DamageID = SessionArr[i]._player->_ID;
						DamagePacket.DamageHP = SessionArr[i]._player->_hp;

						SessionArr[j]._sendQ.Enqueue((char*)&DamageHeader, sizeof(DamageHeader));
						SessionArr[j]._sendQ.Enqueue((char*)&DamagePacket, sizeof(DamagePacket));
					}

					if (SessionArr[i]._player->_hp <= 0)
					{
						PacketHeader DeleteHeader;
						SC_DELETE_CHARACTER DeletePacket;

						DeleteHeader.byCode = 0x89;
						DeleteHeader.bySize = sizeof(DeletePacket);
						DeleteHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

						DeletePacket.ID = SessionArr[i]._player->_ID;

						for (int j = 0; j < playerCount; j++)
						{
							SessionArr[j]._sendQ.Enqueue((char*)&DeleteHeader, sizeof(DeleteHeader));
							SessionArr[j]._sendQ.Enqueue((char*)&DeletePacket, sizeof(DeletePacket));
						}

						SessionArr[i]._delete = true;
						//현재는 딜리트 메세지로 캐릭터들이 사라지고 있음 (소켓 연결 끊어야하나 ?)
					}


				}
			}

		}

	}


	return true;
}