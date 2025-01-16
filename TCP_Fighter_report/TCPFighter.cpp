#include "TCPFighter.h"


SOCKET listen_socket;

extern DWORD playerIdex; //63명까지 접속을 받는 상황으로 가정
extern Session SessionArr[SELECTCOUNT][SELECTDEFINE];
extern Session* pSessionArr;

fd_set readset[SELECTCOUNT];
fd_set writeset[SELECTCOUNT];

bool TCPFighter() {
	int i;
	srand(50);


	timeBeginPeriod(1);

	//return value //-----------------------------
	int wsa_retval;
	int sock_retval;
	int select_retval;
	int ringbuffer_retval;

	//socket inteligense//----------------
	WSADATA wsa;
	SOCKADDR_IN serverAddr;
	SOCKADDR_IN clientAddr;

	ZeroMemory(&serverAddr, sizeof(serverAddr));
	ZeroMemory(&clientAddr, sizeof(serverAddr));

	int addrSize = sizeof(clientAddr);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORTNUM);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	u_long non_block = 1;


	//select inteligense//----------------------------
	TIMEVAL timeout = { 0, 0 };

	for (int i = 0; i < SELECTCOUNT; i++)
	{
		FD_ZERO(&readset[i]);
		FD_ZERO(&writeset[i]);
	}
	//NetWork//---------------------------

	wsa_retval = WSAStartup(WINSOCK_VERSION, &wsa);
	if (wsa_retval != 0) {
		printf("wsa startup error : %d\n", GetLastError());
		return false;
	}


	listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET) {
		printf("%d socket error : %d\n", __LINE__, GetLastError());
		return false;
	}
	printf("socket\n");
	sock_retval = bind(listen_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (sock_retval != 0) {
		printf("%d bind error : %d\n", __LINE__, GetLastError());
		return false;
	}
	printf("bind\n");

	sock_retval = listen(listen_socket, SOMAXCONN);
	if (sock_retval != 0) {
		printf("%d listen error : %d\n", __LINE__, GetLastError());
		return false;
	}
	printf("listen \n");
	ioctlsocket(listen_socket, FIONBIO, &non_block);

	//메인 루프 //-----------------------------------------



	
	DWORD dwUpdateTick = timeGetTime() - 20;

	while (1) 
	{
		dwUpdateTick += 20; // 50프레임 맞추기 위해 

		//네트워크//----------------------------

		FD_SET(listen_socket, &readset[0]);
		//나머지 accept된 소켓들은 아래에서 넣을 예정

		for (int i = 0; i < SELECTCOUNT; i++)
		{
			select_retval = select(NULL, &readset[i], &writeset[i], NULL, &timeout);
			if (select_retval == SOCKET_ERROR)
			{
				printf("Idex : %d, %d select error : %d\n", i, __LINE__, GetLastError());
			}
		}




		if (FD_ISSET(listen_socket, &readset[0])) //캐릭터 초기 생성
		{
			//세션의 할당 안 된 곳에 accept해 줄 예정//---------------------
			// 세션을 먼저 구성하고 나서 
			// -----------------------------------------------------------
			if (playerIdex >= PLAYERMAXCOUNT) 
			{
				printf("playercount overflow !! , PlayerCount : %d\n", playerIdex);
			}
			else 
			{
				pSessionArr[playerIdex]._socket = accept(listen_socket, (SOCKADDR*)&clientAddr, &addrSize);
				if (pSessionArr[playerIdex]._socket == INVALID_SOCKET) 
				{
					printf("Line : %d, playcount : %d,  accept error : %d\n", __LINE__, playerIdex, GetLastError());
				}
				pSessionArr[playerIdex]._ip = clientAddr.sin_addr.s_addr;
				pSessionArr[playerIdex]._port = clientAddr.sin_port;
				pSessionArr[playerIdex]._player = new Player;
				pSessionArr[playerIdex]._delete = false; //delete에 false;
				//초기 생성에 대한 작업들
				CreateNewCharacter(&pSessionArr[playerIdex]);

				playerIdex++;

				printf("create new character ! %d \n", playerIdex);


			}
		}

		//여기서부터 네트워크 구조 수정 필요 selectcount 

		for (i = 0; i < playerIdex; i++) 
		{
			if (FD_ISSET(SessionArr[i]._socket, &readset)) 
			{
				//recv 작업 
				//direct enqueue를 안하는 상황

				char* buf;
				buf = (char*)malloc(SessionArr[i]._recvQ.GetSizeFree());

				sock_retval = recv(SessionArr[i]._socket, buf, SessionArr[i]._recvQ.GetSizeFree(), 0);
				/*
				sock_retval = recv(SessionArr[i]._socket, SessionArr[i]._recvQ.GetRearBufferPtr(),
					SessionArr[i]._recvQ.DirectEnqueueSize(), 0);
				*/
				
				
				
				if (sock_retval == SOCKET_ERROR) 
				{
					if (GetLastError() == 10053 || GetLastError () == 10054)
					{
						if (SessionArr[i]._delete == false)
						{
							PacketHeader DeleteHeader;
							SC_DELETE_CHARACTER DeletePacket;

							DeleteHeader.byCode = 0x89;
							DeleteHeader.bySize = sizeof(DeletePacket);
							DeleteHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

							DeletePacket.ID = SessionArr[i]._player->_ID;

							for (int j = 0; j < playerIdex; j++)
							{
								SessionArr[j]._sendQ.Enqueue((char*)&DeleteHeader, sizeof(DeleteHeader));
								SessionArr[j]._sendQ.Enqueue((char*)&DeletePacket, sizeof(DeletePacket));
							}

							SessionArr[i]._delete = true;
							//현재는 딜리트 메세지로 캐릭터들이 사라지고 있음 (소켓 연결 끊어야하나 ?)
						}

					}


					else if (GetLastError() != WSAEWOULDBLOCK && GetLastError() != 10053) //RST 종료에 대한 예외 처리 
					{
						printf("%d recv error : %d\n", __LINE__, GetLastError());
						return false;
					}
				}

				
				if (SessionArr[i]._recvQ.Enqueue(buf, sock_retval) == false)
				{
					//printf("Line : %d, Enqueue Error \n", __LINE__);
					//return false;
				}

				/*
				if (SessionArr[i]._recvQ.MoveRear(sock_retval) == 0) 
				{
					printf("%d recvQ MoveRear error %d\n", __LINE__, GetLastError());
					return false;
				}// sock_retval 만큼 moverear;
				*/

				free(buf);

			}


			if (FD_ISSET(SessionArr[i]._socket, &writeset)) 
			{	
				if (SessionArr[i]._sendQ.IsEmpty() == false) {
					//send 작업
					//direct dequeue는 사용하지 않는 상황으로 일단

					char* sendBuf;
					int sendVal;
					sendBuf = (char*)malloc(SessionArr[i]._sendQ.GetSizeUsed());
					PacketHeader* pHeader;
					
					sendVal = SessionArr[i]._sendQ.GetSizeUsed();

					pHeader = (PacketHeader*)SessionArr[i]._sendQ.GetFront();

					//puts("");
					if (pHeader->byCode != 0x89)

					{
						printf("Send RingBuf / byCode : %d, bySize : %d, byType : %d\n", pHeader->byCode, pHeader->bySize, pHeader->byType);
					}
					if (SessionArr[i]._sendQ.Dequeue(sendBuf, SessionArr[i]._sendQ.GetSizeUsed()) == false) {
						printf("Line : %d, Dequeue error \n", __LINE__);
						//return false;
					}

					pHeader = (PacketHeader*)&sendBuf;
					if (pHeader->byCode != 0x89)
					{
						//puts("");
						//printf("sendBuf / byCode : %d, bySize : %d, byType: %d \n", pHeader->byCode, pHeader->bySize, pHeader->byType);
					}



					sock_retval = send(SessionArr[i]._socket, sendBuf, sendVal, 0);



					/*
					sock_retval = send(SessionArr[i]._socket, SessionArr[i]._sendQ.GetFrontBufferPtr(),
						SessionArr[i]._sendQ.DirectDequeueSize(), 0);
					printf("send Retval : %d, direct dequeueSize : %d\n", sock_retval, SessionArr[i]._sendQ.DirectDequeueSize());
					*/


					if (sock_retval == SOCKET_ERROR)
					{
						if (GetLastError() == 10053 || GetLastError() == 10054)
						{
							if (SessionArr[i]._delete <= false)
							{
								PacketHeader DeleteHeader;
								SC_DELETE_CHARACTER DeletePacket;

								DeleteHeader.byCode = 0x89;
								DeleteHeader.bySize = sizeof(DeletePacket);
								DeleteHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

								DeletePacket.ID = SessionArr[i]._player->_ID;

								for (int j = 0; j < playerIdex; j++)
								{
									SessionArr[j]._sendQ.Enqueue((char*)&DeleteHeader, sizeof(DeleteHeader));
									SessionArr[j]._sendQ.Enqueue((char*)&DeletePacket, sizeof(DeletePacket));
								}

								SessionArr[i]._delete = true;
								//현재는 딜리트 메세지로 캐릭터들이 사라지고 있음 (소켓 연결 끊어야하나 ?)
							}

						}


						else if (GetLastError() != WSAEWOULDBLOCK && GetLastError() != 10053)
						{
							printf("%d send error : %d\n", __LINE__, GetLastError());
							return false;
						}
						//send의 wouldblock의 경우에 대한 예외처리 필요
					}
					/*
					if (SessionArr[i]._sendQ.MoveFront(sock_retval) == 0)
					{
						printf("%d sendQ MoveFront error : %d\n", __LINE__, GetLastError());
						return false;
					}

					*/
				}

			}
		}
		//Sleep(10000);
	
		FD_ZERO(&readset);
		FD_ZERO(&writeset);

		for (i = 0; i < playerIdex; i++) 
		{
			FD_SET(SessionArr[i]._socket, &readset);
			FD_SET(SessionArr[i]._socket, &writeset);
		}

	//로직//--------------------------------
	

		//recvq에 있는 데이터를 뜯어보고 sendq에 데이터를 넣어 줌 

		for (int i = 0; i < playerIdex; i++) 
		{
			if (SessionArr[i]._recvQ.IsEmpty() != true) {

				DecodePacket(&SessionArr[i]);


			}
		}


		//게임 로직//
		for (int i = 0; i < playerIdex; i++)
		{
			SessionArr[i]._player->Move();
			//printf("Move ! /////// ID: %d, x: %d, y : %d\n", SessionArr[i]._player->_ID, SessionArr[i]._player->_x, SessionArr[i]._player->_y);
		}







		DWORD dwTick2 = timeGetTime() - dwUpdateTick;
		if (dwTick2 < 20)
		{
			Sleep(20 - dwTick2);
		}


	}



	WSACleanup();




	return true;
}