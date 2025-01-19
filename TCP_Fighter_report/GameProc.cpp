#include "GameProc.h"

extern Session SessionArr[PLAYERMAXCOUNT];

extern DWORD playerIdex;

extern std::list<Session*> Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];
extern Session* pSector;
std::list<Session*>::iterator it;



bool MoveStart(Session* _session)
{
	CS_MOVE_START MoveStartPacket;
	unsigned int peekResult;
	unsigned int dequeResult;
	unsigned int enqueResult;
	_session->_recvQ.Peek((char*)&MoveStartPacket, sizeof(MoveStartPacket), &peekResult);

	if (peekResult < sizeof(MoveStartPacket)) {
		
		printf("Peek error, Line : %d, front : %d, rear : %d\n", __LINE__, _session->_recvQ.GetFront(), _session->_recvQ.GetRear());
		
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
	PacketHeader MoveHeader;
	SC_MOVE_START SC_MoveStart_Packet;
	
	MoveHeader.byCode = 0x89;
	MoveHeader.bySize = sizeof(SC_MoveStart_Packet);
	MoveHeader.byType = dfPACKET_SC_MOVE_START;

	
	SC_MoveStart_Packet.Direction = MoveStartPacket.Direction;
	SC_MoveStart_Packet.X = MoveStartPacket.X;
	SC_MoveStart_Packet.Y = MoveStartPacket.Y;
	SC_MoveStart_Packet.ID = _session->_player->_ID;

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if ((_session->_player->_x + i) < 0 || _session->_player->_x + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
			if ((_session->_player->_y + j) < 0 || _session->_player->_y + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


			it = Sector[_session->_player->_x + i][_session->_player->_y + j].begin();

			for (; it != Sector[_session->_player->_x + i][_session->_player->_y + j].end(); it++)
			{

				if ((*it)->_player->_ID == _session->_player->_ID) continue;

				if ((*it)->_sendQ.GetSizeFree() < sizeof(MoveHeader) + MoveHeader.bySize)
				{
					printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
					//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
					return false;
				}

				if ((*it)->_sendQ.Enqueue((char*)&MoveHeader, sizeof(MoveHeader), &enqueResult) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
					return false;
				}

				if ((*it)->_sendQ.Enqueue((char*)&SC_MoveStart_Packet, MoveHeader.bySize, &enqueResult) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
					return false;
				}

			}
		}

	}

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

	_session->_player->_direction = MoveStopPacket.Direction;
	_session->_player->_x = MoveStopPacket.X;
	_session->_player->_y = MoveStopPacket.Y;


	_session->_player->MoveStop();
	
	//섹터 주변에 뿌려주기
	PacketHeader MoveHeader;
	SC_MOVE_STOP SC_MoveStopPacket;

	MoveHeader.byCode = 0x89;
	MoveHeader.bySize = sizeof(SC_MoveStopPacket);
	MoveHeader.byType = dfPACKET_SC_MOVE_STOP;

	SC_MoveStopPacket.Direction = MoveStopPacket.Direction;
	SC_MoveStopPacket.X = MoveStopPacket.X;
	SC_MoveStopPacket.Y = MoveStopPacket.Y;
	SC_MoveStopPacket.ID = _session->_player->_ID;

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if ((_session->_player->_x + i) < 0 || _session->_player->_x + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
			if ((_session->_player->_y + j) < 0 || _session->_player->_y + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


			it = Sector[_session->_player->_x + i][_session->_player->_y + j].begin();

			for (; it != Sector[_session->_player->_x + i][_session->_player->_y + j].end(); it++)
			{

				if ((*it)->_player->_ID == _session->_player->_ID) continue;

				if ((*it)->_sendQ.GetSizeFree() < sizeof(MoveHeader) + MoveHeader.bySize)
				{
					printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
					//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
					return false;
				}

				if ((*it)->_sendQ.Enqueue((char*)&MoveHeader, sizeof(MoveHeader), &enqueResult) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
					return false;
				}

				if ((*it)->_sendQ.Enqueue((char*)&SC_MoveStopPacket, MoveHeader.bySize, &enqueResult) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
					return false;
				}

			}
		}
	}

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


	PacketHeader AttackHeader;
	SC_ATTACK1 SCAttackPacket;

	AttackHeader.byCode = 0x89;
	AttackHeader.bySize = sizeof(SCAttackPacket);
	AttackHeader.byType = dfPACKET_SC_ATTACK1;

	SCAttackPacket.Direction = AttackPacket.Direction;
	SCAttackPacket.X = AttackPacket.X;
	SCAttackPacket.Y = AttackPacket.Y;
	SCAttackPacket.ID = _session->_player->_ID;

	//먼저 어택 메세지를 섹터에 뿌려줌

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if ((_session->_player->_x + i) < 0 || _session->_player->_x + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
			if ((_session->_player->_y + j) < 0 || _session->_player->_y + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


			it = Sector[_session->_player->_x + i][_session->_player->_y + j].begin();

			for (; it != Sector[_session->_player->_x + i][_session->_player->_y + j].end(); it++)
			{

				if ((*it)->_player->_ID == _session->_player->_ID) continue;

				if ((*it)->_sendQ.GetSizeFree() < sizeof(AttackHeader) + AttackHeader.bySize)
				{
					printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
					//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
					return false;
				}

				if ((*it)->_sendQ.Enqueue((char*)&AttackHeader, sizeof(AttackHeader), &enqueResult) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
					return false;
				}

				if ((*it)->_sendQ.Enqueue((char*)&SCAttackPacket, AttackHeader.bySize, &enqueResult) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
					return false;
				}

			}
		}
	}



	if ((AttackPacket.Direction) == LL) //left
	{
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if ((_session->_player->_x + i) < 0 || _session->_player->_x + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
				if ((_session->_player->_y + j) < 0 || _session->_player->_y + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;

				it = Sector[_session->_player->_x + i][_session->_player->_y + j].begin();

				for (; it != Sector[_session->_player->_x + i][_session->_player->_y + j].end(); it++)
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

					PacketHeader DamageHeader;
					SC_DAMAGE DamagePacket;
					std::list<Session*>::iterator FuncIt;

					DamageHeader.byCode = 0x89;
					DamageHeader.bySize = sizeof(DamagePacket);
					DamageHeader.byType = dfPACKET_SC_DAMAGE;

					DamagePacket.AttackID = _session->_player->_ID;
					DamagePacket.DamageID = (*it)->_player->_ID;
					DamagePacket.DamageHP = (*it)->_player->_hp;

					for (int damageIdex = -1; damageIdex < 2; damageIdex++)
					{
						for (int damageJdex = -1; damageJdex < 2; damageJdex++)
						{
							if ((_session->_player->_x + damageIdex) < 0 || _session->_player->_x + damageIdex >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
							if ((_session->_player->_y + damageJdex) < 0 || _session->_player->_y + damageJdex >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


							FuncIt = Sector[_session->_player->_x + damageIdex][_session->_player->_y + damageJdex].begin();

							for (; FuncIt != Sector[_session->_player->_x + damageIdex][_session->_player->_y + damageJdex].end(); FuncIt++)
							{

								

								if ((*FuncIt)->_sendQ.GetSizeFree() < sizeof(DamageHeader) + DamageHeader.bySize)
								{
									printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
									//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
									return false;
								}

								if ((*FuncIt)->_sendQ.Enqueue((char*)&DamageHeader, sizeof(DamageHeader), &enqueResult) == false)
								{
									printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
									return false;
								}

								if ((*FuncIt)->_sendQ.Enqueue((char*)&DamagePacket, DamageHeader.bySize, &enqueResult) == false)
								{
									printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
									return false;
								}

							}
						}
					}

					//사망 판정 //

					if ((*it)->_player->_hp <= 0)
					{
						PacketHeader DeleteHeader;
						SC_DELETE_CHARACTER DeletePacket;

						DeleteHeader.byCode = 0x89;
						DeleteHeader.bySize = sizeof(DeletePacket);
						DeleteHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

						DeletePacket.ID = (*it)->_player->_ID;

						for (int deleteIdex = -1; deleteIdex < 2; deleteIdex++)
						{
							for (int deleteJdex = -1; deleteJdex < 2; deleteJdex++)
							{
								if ((_session->_player->_x + deleteIdex) < 0 || _session->_player->_x + deleteIdex >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
								if ((_session->_player->_y + deleteJdex) < 0 || _session->_player->_y + deleteJdex >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


								FuncIt = Sector[_session->_player->_x + deleteIdex][_session->_player->_y + deleteJdex].begin();

								for (; FuncIt != Sector[_session->_player->_x + deleteIdex][_session->_player->_y + deleteJdex].end(); FuncIt++)
								{



									if ((*FuncIt)->_sendQ.GetSizeFree() < sizeof(DeleteHeader) + DeleteHeader.bySize)
									{
										printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
										//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
										return false;
									}

									if ((*FuncIt)->_sendQ.Enqueue((char*)&DeleteHeader, sizeof(DeleteHeader), &enqueResult) == false)
									{
										printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
										return false;
									}

									if ((*FuncIt)->_sendQ.Enqueue((char*)&DeletePacket, DeleteHeader.bySize, &enqueResult) == false)
									{
										printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
										return false;
									}




								}
							}
						}

						return true; //한명 타격 성공하면 바로 리턴 
						DeleteSession(*it);
						Sector[_session->_player->_x + i][_session->_player->_y + j].erase(it); //Sector 리스트에서 삭제

					}

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
				if ((_session->_player->_x + i) < 0 || _session->_player->_x + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
				if ((_session->_player->_y + j) < 0 || _session->_player->_y + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;

				it = Sector[_session->_player->_x + i][_session->_player->_y + j].begin();

				for (; it != Sector[_session->_player->_x + i][_session->_player->_y + j].end(); it++)
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

					PacketHeader DamageHeader;
					SC_DAMAGE DamagePacket;
					std::list<Session*>::iterator FuncIt;

					DamageHeader.byCode = 0x89;
					DamageHeader.bySize = sizeof(DamagePacket);
					DamageHeader.byType = dfPACKET_SC_DAMAGE;

					DamagePacket.AttackID = _session->_player->_ID;
					DamagePacket.DamageID = (*it)->_player->_ID;
					DamagePacket.DamageHP = (*it)->_player->_hp;

					for (int damageIdex = -1; damageIdex < 2; damageIdex++)
					{
						for (int damageJdex = -1; damageJdex < 2; damageJdex++)
						{
							if ((_session->_player->_x + damageIdex) < 0 || _session->_player->_x + damageIdex >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
							if ((_session->_player->_y + damageJdex) < 0 || _session->_player->_y + damageJdex >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


							FuncIt = Sector[_session->_player->_x + damageIdex][_session->_player->_y + damageJdex].begin();

							for (; FuncIt != Sector[_session->_player->_x + damageIdex][_session->_player->_y + damageJdex].end(); FuncIt++)
							{



								if ((*FuncIt)->_sendQ.GetSizeFree() < sizeof(DamageHeader) + DamageHeader.bySize)
								{
									printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
									//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
									return false;
								}

								if ((*FuncIt)->_sendQ.Enqueue((char*)&DamageHeader, sizeof(DamageHeader), &enqueResult) == false)
								{
									printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
									return false;
								}

								if ((*FuncIt)->_sendQ.Enqueue((char*)&DamagePacket, DamageHeader.bySize, &enqueResult) == false)
								{
									printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
									return false;
								}

							}
						}
					}

					//사망 판정 //

					if ((*it)->_player->_hp <= 0)
					{
						PacketHeader DeleteHeader;
						SC_DELETE_CHARACTER DeletePacket;

						DeleteHeader.byCode = 0x89;
						DeleteHeader.bySize = sizeof(DeletePacket);
						DeleteHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

						DeletePacket.ID = (*it)->_player->_ID;

						for (int deleteIdex = -1; deleteIdex < 2; deleteIdex++)
						{
							for (int deleteJdex = -1; deleteJdex < 2; deleteJdex++)
							{
								if ((_session->_player->_x + deleteIdex) < 0 || _session->_player->_x + deleteIdex >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
								if ((_session->_player->_y + deleteJdex) < 0 || _session->_player->_y + deleteJdex >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


								FuncIt = Sector[_session->_player->_x + deleteIdex][_session->_player->_y + deleteJdex].begin();

								for (; FuncIt != Sector[_session->_player->_x + deleteIdex][_session->_player->_y + deleteJdex].end(); FuncIt++)
								{



									if ((*FuncIt)->_sendQ.GetSizeFree() < sizeof(DeleteHeader) + DeleteHeader.bySize)
									{
										printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
										//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
										return false;
									}

									if ((*FuncIt)->_sendQ.Enqueue((char*)&DeleteHeader, sizeof(DeleteHeader), &enqueResult) == false)
									{
										printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
										return false;
									}

									if ((*FuncIt)->_sendQ.Enqueue((char*)&DeletePacket, DeleteHeader.bySize, &enqueResult) == false)
									{
										printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
										return false;
									}




								}
							}
						}

						return true; //한명 타격 성공하면 바로 리턴 
						DeleteSession(*it);
						Sector[_session->_player->_x + i][_session->_player->_y + j].erase(it); //Sector 리스트에서 삭제

					}

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


	PacketHeader AttackHeader;
	SC_ATTACK2 SCAttackPacket;

	AttackHeader.byCode = 0x89;
	AttackHeader.bySize = sizeof(SCAttackPacket);
	AttackHeader.byType = dfPACKET_SC_ATTACK2;

	SCAttackPacket.Direction = AttackPacket.Direction;
	SCAttackPacket.X = AttackPacket.X;
	SCAttackPacket.Y = AttackPacket.Y;
	SCAttackPacket.ID = _session->_player->_ID;

	//먼저 어택 메세지를 섹터에 뿌려줌

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if ((_session->_player->_x + i) < 0 || _session->_player->_x + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
			if ((_session->_player->_y + j) < 0 || _session->_player->_y + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


			it = Sector[_session->_player->_x + i][_session->_player->_y + j].begin();

			for (; it != Sector[_session->_player->_x + i][_session->_player->_y + j].end(); it++)
			{

				if ((*it)->_player->_ID == _session->_player->_ID) continue;

				if ((*it)->_sendQ.GetSizeFree() < sizeof(AttackHeader) + AttackHeader.bySize)
				{
					printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
					//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
					return false;
				}

				if ((*it)->_sendQ.Enqueue((char*)&AttackHeader, sizeof(AttackHeader), &enqueResult) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
					return false;
				}

				if ((*it)->_sendQ.Enqueue((char*)&SCAttackPacket, AttackHeader.bySize, &enqueResult) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
					return false;
				}

			}
		}
	}



	if ((AttackPacket.Direction) == LL) //left
	{
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if ((_session->_player->_x + i) < 0 || _session->_player->_x + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
				if ((_session->_player->_y + j) < 0 || _session->_player->_y + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;

				it = Sector[_session->_player->_x + i][_session->_player->_y + j].begin();

				for (; it != Sector[_session->_player->_x + i][_session->_player->_y + j].end(); it++)
				{
					if ((*it)->_player->_ID == _session->_player->_ID) continue;

					if ((AttackPacket.X - (*it)->_player->_x) < 0 ||
						(AttackPacket.X - (*it)->_player->_x) > dfATTACK2_RANGE_X) continue; //공격 범위 벗어남

					if ((AttackPacket.Y - (*it)->_player->_y >= 0 && AttackPacket.Y - (*it)->_player->_y < dfATTACK2_RANGE_Y) ||
						((*it)->_player->_y - AttackPacket.Y >= 0 && (*it)->_player->_y - AttackPacket.Y < dfATTACK2_RANGE_Y) == false)
						continue;

					//타격 성공 
					(*it)->_player->_hp -= dfAttack2Damage;


					//데미지 메세지 보내기

					PacketHeader DamageHeader;
					SC_DAMAGE DamagePacket;
					std::list<Session*>::iterator FuncIt;

					DamageHeader.byCode = 0x89;
					DamageHeader.bySize = sizeof(DamagePacket);
					DamageHeader.byType = dfPACKET_SC_DAMAGE;

					DamagePacket.AttackID = _session->_player->_ID;
					DamagePacket.DamageID = (*it)->_player->_ID;
					DamagePacket.DamageHP = (*it)->_player->_hp;

					for (int damageIdex = -1; damageIdex < 2; damageIdex++)
					{
						for (int damageJdex = -1; damageJdex < 2; damageJdex++)
						{
							if ((_session->_player->_x + damageIdex) < 0 || _session->_player->_x + damageIdex >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
							if ((_session->_player->_y + damageJdex) < 0 || _session->_player->_y + damageJdex >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


							FuncIt = Sector[_session->_player->_x + damageIdex][_session->_player->_y + damageJdex].begin();

							for (; FuncIt != Sector[_session->_player->_x + damageIdex][_session->_player->_y + damageJdex].end(); FuncIt++)
							{



								if ((*FuncIt)->_sendQ.GetSizeFree() < sizeof(DamageHeader) + DamageHeader.bySize)
								{
									printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
									//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
									return false;
								}

								if ((*FuncIt)->_sendQ.Enqueue((char*)&DamageHeader, sizeof(DamageHeader), &enqueResult) == false)
								{
									printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
									return false;
								}

								if ((*FuncIt)->_sendQ.Enqueue((char*)&DamagePacket, DamageHeader.bySize, &enqueResult) == false)
								{
									printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
									return false;
								}

							}
						}
					}

					//사망 판정 //

					if ((*it)->_player->_hp <= 0)
					{
						PacketHeader DeleteHeader;
						SC_DELETE_CHARACTER DeletePacket;

						DeleteHeader.byCode = 0x89;
						DeleteHeader.bySize = sizeof(DeletePacket);
						DeleteHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

						DeletePacket.ID = (*it)->_player->_ID;

						for (int deleteIdex = -1; deleteIdex < 2; deleteIdex++)
						{
							for (int deleteJdex = -1; deleteJdex < 2; deleteJdex++)
							{
								if ((_session->_player->_x + deleteIdex) < 0 || _session->_player->_x + deleteIdex >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
								if ((_session->_player->_y + deleteJdex) < 0 || _session->_player->_y + deleteJdex >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


								FuncIt = Sector[_session->_player->_x + deleteIdex][_session->_player->_y + deleteJdex].begin();

								for (; FuncIt != Sector[_session->_player->_x + deleteIdex][_session->_player->_y + deleteJdex].end(); FuncIt++)
								{



									if ((*FuncIt)->_sendQ.GetSizeFree() < sizeof(DeleteHeader) + DeleteHeader.bySize)
									{
										printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
										//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
										return false;
									}

									if ((*FuncIt)->_sendQ.Enqueue((char*)&DeleteHeader, sizeof(DeleteHeader), &enqueResult) == false)
									{
										printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
										return false;
									}

									if ((*FuncIt)->_sendQ.Enqueue((char*)&DeletePacket, DeleteHeader.bySize, &enqueResult) == false)
									{
										printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
										return false;
									}




								}
							}
						}

						return true; //한명 타격 성공하면 바로 리턴 
						DeleteSession(*it);
						Sector[_session->_player->_x + i][_session->_player->_y + j].erase(it); //Sector 리스트에서 삭제

					}

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
				if ((_session->_player->_x + i) < 0 || _session->_player->_x + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
				if ((_session->_player->_y + j) < 0 || _session->_player->_y + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;

				it = Sector[_session->_player->_x + i][_session->_player->_y + j].begin();

				for (; it != Sector[_session->_player->_x + i][_session->_player->_y + j].end(); it++)
				{
					if ((*it)->_player->_ID == _session->_player->_ID) continue;

					if (((*it)->_player->_x) - AttackPacket.X < 0 ||
						((*it)->_player->_x) - AttackPacket.X > dfATTACK2_RANGE_X) continue; //공격 범위 벗어남

					if ((AttackPacket.Y - (*it)->_player->_y >= 0 && AttackPacket.Y - (*it)->_player->_y < dfATTACK2_RANGE_Y) ||
						((*it)->_player->_y - AttackPacket.Y >= 0 && (*it)->_player->_y - AttackPacket.Y < dfATTACK2_RANGE_Y) == false)
						continue;

					//타격 성공 
					(*it)->_player->_hp -= dfAttack2Damage;


					//데미지 메세지 보내기

					PacketHeader DamageHeader;
					SC_DAMAGE DamagePacket;
					std::list<Session*>::iterator FuncIt;

					DamageHeader.byCode = 0x89;
					DamageHeader.bySize = sizeof(DamagePacket);
					DamageHeader.byType = dfPACKET_SC_DAMAGE;

					DamagePacket.AttackID = _session->_player->_ID;
					DamagePacket.DamageID = (*it)->_player->_ID;
					DamagePacket.DamageHP = (*it)->_player->_hp;

					for (int damageIdex = -1; damageIdex < 2; damageIdex++)
					{
						for (int damageJdex = -1; damageJdex < 2; damageJdex++)
						{
							if ((_session->_player->_x + damageIdex) < 0 || _session->_player->_x + damageIdex >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
							if ((_session->_player->_y + damageJdex) < 0 || _session->_player->_y + damageJdex >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


							FuncIt = Sector[_session->_player->_x + damageIdex][_session->_player->_y + damageJdex].begin();

							for (; FuncIt != Sector[_session->_player->_x + damageIdex][_session->_player->_y + damageJdex].end(); FuncIt++)
							{



								if ((*FuncIt)->_sendQ.GetSizeFree() < sizeof(DamageHeader) + DamageHeader.bySize)
								{
									printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
									//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
									return false;
								}

								if ((*FuncIt)->_sendQ.Enqueue((char*)&DamageHeader, sizeof(DamageHeader), &enqueResult) == false)
								{
									printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
									return false;
								}

								if ((*FuncIt)->_sendQ.Enqueue((char*)&DamagePacket, DamageHeader.bySize, &enqueResult) == false)
								{
									printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
									return false;
								}

							}
						}
					}

					//사망 판정 //

					if ((*it)->_player->_hp <= 0)
					{
						PacketHeader DeleteHeader;
						SC_DELETE_CHARACTER DeletePacket;

						DeleteHeader.byCode = 0x89;
						DeleteHeader.bySize = sizeof(DeletePacket);
						DeleteHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

						DeletePacket.ID = (*it)->_player->_ID;

						for (int deleteIdex = -1; deleteIdex < 2; deleteIdex++)
						{
							for (int deleteJdex = -1; deleteJdex < 2; deleteJdex++)
							{
								if ((_session->_player->_x + deleteIdex) < 0 || _session->_player->_x + deleteIdex >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
								if ((_session->_player->_y + deleteJdex) < 0 || _session->_player->_y + deleteJdex >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


								FuncIt = Sector[_session->_player->_x + deleteIdex][_session->_player->_y + deleteJdex].begin();

								for (; FuncIt != Sector[_session->_player->_x + deleteIdex][_session->_player->_y + deleteJdex].end(); FuncIt++)
								{



									if ((*FuncIt)->_sendQ.GetSizeFree() < sizeof(DeleteHeader) + DeleteHeader.bySize)
									{
										printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
										//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
										return false;
									}

									if ((*FuncIt)->_sendQ.Enqueue((char*)&DeleteHeader, sizeof(DeleteHeader), &enqueResult) == false)
									{
										printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
										return false;
									}

									if ((*FuncIt)->_sendQ.Enqueue((char*)&DeletePacket, DeleteHeader.bySize, &enqueResult) == false)
									{
										printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
										return false;
									}




								}
							}
						}

						return true; //한명 타격 성공하면 바로 리턴 
						DeleteSession(*it);
						Sector[_session->_player->_x + i][_session->_player->_y + j].erase(it); //Sector 리스트에서 삭제

					}

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


	PacketHeader AttackHeader;
	SC_ATTACK3 SCAttackPacket;

	AttackHeader.byCode = 0x89;
	AttackHeader.bySize = sizeof(SCAttackPacket);
	AttackHeader.byType = dfPACKET_SC_ATTACK3;

	SCAttackPacket.Direction = AttackPacket.Direction;
	SCAttackPacket.X = AttackPacket.X;
	SCAttackPacket.Y = AttackPacket.Y;
	SCAttackPacket.ID = _session->_player->_ID;

	//먼저 어택 메세지를 섹터에 뿌려줌

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if ((_session->_player->_x + i) < 0 || _session->_player->_x + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
			if ((_session->_player->_y + j) < 0 || _session->_player->_y + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


			it = Sector[_session->_player->_x + i][_session->_player->_y + j].begin();

			for (; it != Sector[_session->_player->_x + i][_session->_player->_y + j].end(); it++)
			{

				if ((*it)->_player->_ID == _session->_player->_ID) continue;

				if ((*it)->_sendQ.GetSizeFree() < sizeof(AttackHeader) + AttackHeader.bySize)
				{
					printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
					//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
					return false;
				}

				if ((*it)->_sendQ.Enqueue((char*)&AttackHeader, sizeof(AttackHeader), &enqueResult) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
					return false;
				}

				if ((*it)->_sendQ.Enqueue((char*)&SCAttackPacket, AttackHeader.bySize, &enqueResult) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
					return false;
				}

			}
		}
	}



	if ((AttackPacket.Direction) == LL) //left
	{
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if ((_session->_player->_x + i) < 0 || _session->_player->_x + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
				if ((_session->_player->_y + j) < 0 || _session->_player->_y + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;

				it = Sector[_session->_player->_x + i][_session->_player->_y + j].begin();

				for (; it != Sector[_session->_player->_x + i][_session->_player->_y + j].end(); it++)
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

					PacketHeader DamageHeader;
					SC_DAMAGE DamagePacket;
					std::list<Session*>::iterator FuncIt;

					DamageHeader.byCode = 0x89;
					DamageHeader.bySize = sizeof(DamagePacket);
					DamageHeader.byType = dfPACKET_SC_DAMAGE;

					DamagePacket.AttackID = _session->_player->_ID;
					DamagePacket.DamageID = (*it)->_player->_ID;
					DamagePacket.DamageHP = (*it)->_player->_hp;

					for (int damageIdex = -1; damageIdex < 2; damageIdex++)
					{
						for (int damageJdex = -1; damageJdex < 2; damageJdex++)
						{
							if ((_session->_player->_x + damageIdex) < 0 || _session->_player->_x + damageIdex >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
							if ((_session->_player->_y + damageJdex) < 0 || _session->_player->_y + damageJdex >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


							FuncIt = Sector[_session->_player->_x + damageIdex][_session->_player->_y + damageJdex].begin();

							for (; FuncIt != Sector[_session->_player->_x + damageIdex][_session->_player->_y + damageJdex].end(); FuncIt++)
							{



								if ((*FuncIt)->_sendQ.GetSizeFree() < sizeof(DamageHeader) + DamageHeader.bySize)
								{
									printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
									//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
									return false;
								}

								if ((*FuncIt)->_sendQ.Enqueue((char*)&DamageHeader, sizeof(DamageHeader), &enqueResult) == false)
								{
									printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
									return false;
								}

								if ((*FuncIt)->_sendQ.Enqueue((char*)&DamagePacket, DamageHeader.bySize, &enqueResult) == false)
								{
									printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
									return false;
								}

							}
						}
					}

					//사망 판정 //

					if ((*it)->_player->_hp <= 0)
					{
						PacketHeader DeleteHeader;
						SC_DELETE_CHARACTER DeletePacket;

						DeleteHeader.byCode = 0x89;
						DeleteHeader.bySize = sizeof(DeletePacket);
						DeleteHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

						DeletePacket.ID = (*it)->_player->_ID;

						for (int deleteIdex = -1; deleteIdex < 2; deleteIdex++)
						{
							for (int deleteJdex = -1; deleteJdex < 2; deleteJdex++)
							{
								if ((_session->_player->_x + deleteIdex) < 0 || _session->_player->_x + deleteIdex >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
								if ((_session->_player->_y + deleteJdex) < 0 || _session->_player->_y + deleteJdex >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


								FuncIt = Sector[_session->_player->_x + deleteIdex][_session->_player->_y + deleteJdex].begin();

								for (; FuncIt != Sector[_session->_player->_x + deleteIdex][_session->_player->_y + deleteJdex].end(); FuncIt++)
								{



									if ((*FuncIt)->_sendQ.GetSizeFree() < sizeof(DeleteHeader) + DeleteHeader.bySize)
									{
										printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
										//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
										return false;
									}

									if ((*FuncIt)->_sendQ.Enqueue((char*)&DeleteHeader, sizeof(DeleteHeader), &enqueResult) == false)
									{
										printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
										return false;
									}

									if ((*FuncIt)->_sendQ.Enqueue((char*)&DeletePacket, DeleteHeader.bySize, &enqueResult) == false)
									{
										printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
										return false;
									}




								}
							}
						}

						return true; //한명 타격 성공하면 바로 리턴 
						DeleteSession(*it);
						Sector[_session->_player->_x + i][_session->_player->_y + j].erase(it); //Sector 리스트에서 삭제

					}

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
				if ((_session->_player->_x + i) < 0 || _session->_player->_x + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
				if ((_session->_player->_y + j) < 0 || _session->_player->_y + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;

				it = Sector[_session->_player->_x + i][_session->_player->_y + j].begin();

				for (; it != Sector[_session->_player->_x + i][_session->_player->_y + j].end(); it++)
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

					PacketHeader DamageHeader;
					SC_DAMAGE DamagePacket;
					std::list<Session*>::iterator FuncIt;

					DamageHeader.byCode = 0x89;
					DamageHeader.bySize = sizeof(DamagePacket);
					DamageHeader.byType = dfPACKET_SC_DAMAGE;

					DamagePacket.AttackID = _session->_player->_ID;
					DamagePacket.DamageID = (*it)->_player->_ID;
					DamagePacket.DamageHP = (*it)->_player->_hp;

					for (int damageIdex = -1; damageIdex < 2; damageIdex++)
					{
						for (int damageJdex = -1; damageJdex < 2; damageJdex++)
						{
							if ((_session->_player->_x + damageIdex) < 0 || _session->_player->_x + damageIdex >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
							if ((_session->_player->_y + damageJdex) < 0 || _session->_player->_y + damageJdex >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


							FuncIt = Sector[_session->_player->_x + damageIdex][_session->_player->_y + damageJdex].begin();

							for (; FuncIt != Sector[_session->_player->_x + damageIdex][_session->_player->_y + damageJdex].end(); FuncIt++)
							{



								if ((*FuncIt)->_sendQ.GetSizeFree() < sizeof(DamageHeader) + DamageHeader.bySize)
								{
									printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
									//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
									return false;
								}

								if ((*FuncIt)->_sendQ.Enqueue((char*)&DamageHeader, sizeof(DamageHeader), &enqueResult) == false)
								{
									printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
									return false;
								}

								if ((*FuncIt)->_sendQ.Enqueue((char*)&DamagePacket, DamageHeader.bySize, &enqueResult) == false)
								{
									printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
									return false;
								}

							}
						}
					}

					//사망 판정 //

					if ((*it)->_player->_hp <= 0)
					{
						PacketHeader DeleteHeader;
						SC_DELETE_CHARACTER DeletePacket;

						DeleteHeader.byCode = 0x89;
						DeleteHeader.bySize = sizeof(DeletePacket);
						DeleteHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

						DeletePacket.ID = (*it)->_player->_ID;

						for (int deleteIdex = -1; deleteIdex < 2; deleteIdex++)
						{
							for (int deleteJdex = -1; deleteJdex < 2; deleteJdex++)
							{
								if ((_session->_player->_x + deleteIdex) < 0 || _session->_player->_x + deleteIdex >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
								if ((_session->_player->_y + deleteJdex) < 0 || _session->_player->_y + deleteJdex >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


								FuncIt = Sector[_session->_player->_x + deleteIdex][_session->_player->_y + deleteJdex].begin();

								for (; FuncIt != Sector[_session->_player->_x + deleteIdex][_session->_player->_y + deleteJdex].end(); FuncIt++)
								{



									if ((*FuncIt)->_sendQ.GetSizeFree() < sizeof(DeleteHeader) + DeleteHeader.bySize)
									{
										printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
										//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
										return false;
									}

									if ((*FuncIt)->_sendQ.Enqueue((char*)&DeleteHeader, sizeof(DeleteHeader), &enqueResult) == false)
									{
										printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
										return false;
									}

									if ((*FuncIt)->_sendQ.Enqueue((char*)&DeletePacket, DeleteHeader.bySize, &enqueResult) == false)
									{
										printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
										return false;
									}




								}
							}
						}

						return true; //한명 타격 성공하면 바로 리턴 
						DeleteSession(*it);
						Sector[_session->_player->_x + i][_session->_player->_y + j].erase(it); //Sector 리스트에서 삭제

					}

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


	PacketHeader pHeader;
	SC_SYNC SC_Sync;
	pHeader.byCode = 0x89;
	pHeader.bySize = sizeof(SC_Sync);
	pHeader.byType = dfPACKET_SC_SYNC;

	SC_Sync.ID = _session->_player->_ID;
	SC_Sync.X = _session->_player->_x;
	SC_Sync.Y = _session->_player->_y;

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			if ((_session->_player->_x + i) < 0 || _session->_player->_x + i >= dfRANGE_MOVE_RIGHT / SECTOR_RATIO) continue;
			if ((_session->_player->_y + j) < 0 || _session->_player->_y + j >= dfRANGE_MOVE_BOTTOM / SECTOR_RATIO) continue;


			it = Sector[_session->_player->_x + i][_session->_player->_y + j].begin();

			for (; it != Sector[_session->_player->_x + i][_session->_player->_y + j].end(); it++)
			{

				if ((*it)->_player->_ID == _session->_player->_ID) continue;

				if ((*it)->_sendQ.GetSizeFree() < sizeof(pHeader) + pHeader.bySize)
				{
					printf("Send Error : RingBuffer SendQue Full, Line : %d\n", __LINE__);
					//Todo//링버퍼 사이즈가 꽉차서 생성메세지를 보내지 못하는 경우에 대한 예외처리
					return false;
				}

				if ((*it)->_sendQ.Enqueue((char*)&pHeader, sizeof(pHeader), &enqueResult) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
					return false;
				}

				if ((*it)->_sendQ.Enqueue((char*)&SC_Sync, pHeader.bySize, &enqueResult) == false)
				{
					printf("Line : %d, ringbuffer sendQ enque error : %d, EnqueOut : %d\n", __LINE__, GetLastError(), enqueResult);
					return false;
				}

			}
		}
	}



	return true;

}
