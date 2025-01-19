#include "TCPFighter.h"


SOCKET listen_socket;

extern DWORD playerIdex; //63명까지 접속을 받는 상황으로 가정
extern std::vector<Session> SessionArr;
extern Session SessionArr[SELECTCOUNT][SELECTDEFINE];
extern Session* pSessionArr;

std::list<Session*> Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];
Session* pSector = (Session*)Sector;

fd_set readset[SELECTCOUNT];
fd_set writeset[SELECTCOUNT];

bool TCPFighter() {
	int i;
	srand(50);

	SessionArr.reserve(PLAYERMAXCOUNT);

	timeBeginPeriod(1);

	//return value //-----------------------------
	int sock_retval;
	int select_retval;
	int recv_retval;
	int send_retval;

	//socket inteligense//----------------
	SOCKADDR_IN clientAddr;

	ZeroMemory(&clientAddr, sizeof(clientAddr));

	int addrSize = sizeof(clientAddr);



	//select inteligense//----------------------------
	TIMEVAL timeout = { 0, 0 };

	NetworkInit(); //nonblock, listen까지 작업 수행


	//Session[0]은 listen_socket
	SessionArr.push_back(Session()); //여기에 임시객체 만들어서 넣기
	SessionArr.at(0)._socket = listen_socket;
	playerIdex++;

	DWORD dwUpdateTick = timeGetTime() - 20;
	
	
	//메인 루프 //-----------------------------------------

	while (1) 
	{
		dwUpdateTick += 20; // 50프레임 맞추기 위해 
		//네트워크//----------------------------

		for (int i = 0; i < SELECTCOUNT; i++)
		{
			FD_ZERO(&readset[i]);
			FD_ZERO(&writeset[i]);
		}


		FD_SET(listen_socket, &readset[0]);
		


		
		
		for (int i = 0; i < playerIdex; i++)
		{
			FD_SET(SessionArr[i]._socket, &readset[i/64]);
			FD_SET(SessionArr[i]._socket, &writeset[i/64]);
		}

		for (int i = 0; i < ((playerIdex - 1) / 64 + 1); i++)
		{
			select_retval = select(NULL, &readset[i], &writeset[i], NULL, &timeout);
			if (select_retval == SOCKET_ERROR)
			{
				printf("Idex : %d, %d select error : %d\n", i, __LINE__, GetLastError());
			}
		}

		//받은 데이터나 보낼 데이터가 있다면 수신 링버퍼에 넣어주는 과정
		for (int i = 1; i < playerIdex; i++)
		{
			if (FD_ISSET(SessionArr[i]._socket, &readset[i / 64]))
			{
				recv_retval = recv(SessionArr[i]._socket, SessionArr[i]._recvQ.GetRear(),
					SessionArr[i]._recvQ.GetDirectEnqueSize(), NULL);

				if (recv_retval == 0)
				{
					//recv retval 0이면 정상 종료에 대한 로직
					DeleteSession(&SessionArr[i]);
				}

				else if (recv_retval == SOCKET_ERROR && GetLastError() != WSAEWOULDBLOCK)
				{
					
					if (GetLastError() == 10054)
					{
						DeleteSession(&SessionArr[i]); //Todo//DeleteSession에 대한 함수 정의 만들어야함
					}
					
					printf("recv error : %d , Line : %d\n", GetLastError(), __LINE__);

				}

				else SessionArr[i]._recvQ.MoveRear(recv_retval);

			}

			if (FD_ISSET(SessionArr[i]._socket, &writeset[i / 64]))
			{
				send_retval = send(SessionArr[i]._socket, SessionArr[i]._sendQ.GetFront(),
					SessionArr[i]._sendQ.GetDirectDequeSize(), NULL);

				if (send_retval == SOCKET_ERROR && GetLastError() != WSAEWOULDBLOCK)
				{
					printf("send Error : %d, Line : %d\n", GetLastError(), __LINE__);
				}

				else SessionArr[i]._sendQ.MoveFront(send_retval);

			}


		}



		if (FD_ISSET(listen_socket, &readset[0])) //캐릭터 초기 생성
		{
			if (playerIdex >= PLAYERMAXCOUNT) 
			{
				printf("playercount overflow !! , PlayerCount : %d\n", playerIdex);
			}
			else 
			{
				SessionArr.push_back(Session());
				SessionArr[playerIdex]._socket= accept(listen_socket, (SOCKADDR*)&clientAddr, &addrSize);
				if (SessionArr[playerIdex]._socket == INVALID_SOCKET)
				{
					printf("Line : %d, playcount : %d,  accept error : %d\n", __LINE__, playerIdex, GetLastError());
					//Todo//push_back한 자리를 다시 지워줘야함
				}
				else {
					SessionArr[playerIdex]._ip = clientAddr.sin_addr.s_addr;
					SessionArr[playerIdex]._port = clientAddr.sin_port;
					SessionArr[playerIdex]._player = new Player; 
					SessionArr[playerIdex]._delete = false; //delete에 false;
					Sector[SessionArr[playerIdex]._player->_x / SECTOR_RATIO][SessionArr[playerIdex]._player->_y / SECTOR_RATIO].push_back(&SessionArr[playerIdex]);

					
					//Todo//플레이어를 섹터에 넣어줘야함
					//초기 생성에 대한 작업들
					CreateNewCharacter(&SessionArr[playerIdex]);

					playerIdex++;

					printf("create new character ! %d \n", playerIdex);
				}

			}
		}

	//로직//--------------------------------
	

		//recvq에 있는 데이터를 뜯어보고 sendq에 데이터를 넣어 줌 
		
		for (int i = 1; i < playerIdex; i++) 
		{
			if (SessionArr[i]._recvQ.IsEmpty() != true) {

				DecodePacket(&SessionArr[i]);


			}
		}


		//게임 로직//
		//Todo//이 부분들이 좀 수정이 필요함 -> 이동을 프레임별 ++구조가 아닌 시간에 따른 계산 로직으로
		/*
		for (int i = 0; i < playerIdex; i++)
		{
			SessionArr[i]._player->Move();
			//printf("Move ! /////// ID: %d, x: %d, y : %d\n", SessionArr[i]._player->_ID, SessionArr[i]._player->_x, SessionArr[i]._player->_y);
		}


		*/






		DWORD dwTick2 = timeGetTime() - dwUpdateTick;
		if (dwTick2 < 20)
		{
			Sleep(20 - dwTick2);
		}


	}



	WSACleanup();




	return true;
}