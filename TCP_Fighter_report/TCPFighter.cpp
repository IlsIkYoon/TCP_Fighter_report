#include "TCPFighter.h"
#include "PacketDefine.h"
#include "Session.h"

SOCKET listen_socket;

DWORD playerCount = 0 ; //63명까지 접속을 받는 상황으로 가정

extern Session SessionArr[PLAYERMAXCOUNT];


bool TCPFighter() {
	int i;


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
	fd_set readset;
	fd_set writeset;
	TIMEVAL timeout = { 0, 0 };
	
	FD_ZERO(&readset);
	FD_ZERO(&writeset);

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

	sock_retval = bind(listen_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (sock_retval != 0) {
		printf("%d bind error : %d\n", __LINE__, GetLastError());
		return false;
	}

	ioctlsocket(listen_socket, FIONBIO, &non_block);

	//메인 루프 //-----------------------------------------

	while (1) {

		//네트워크//----------------------------
		
		FD_SET(listen_socket, &readset);
		//나머지 accept된 소켓들은 아래에서 넣을 예정

		select_retval = select(NULL, &readset, &writeset, NULL, &timeout);
		if (select_retval == SOCKET_ERROR) {
			printf("%d select error : %d\n", __LINE__, GetLastError());
		}

		if (FD_ISSET(listen_socket, &readset)){
			//세션의 할당 안 된 곳에 accept해 줄 예정//---------------------
			// 세션을 먼저 구성하고 나서 
			// -----------------------------------------------------------
			if (playerCount >= PLAYERMAXCOUNT) {
				printf("playercount overflow !! , PlayerCount : %d\n", playerCount);
			}
			else {
				SessionArr[playerCount]._socket = accept(listen_socket, (SOCKADDR*)&clientAddr, &addrSize);
				if (SessionArr[playerCount]._socket == INVALID_SOCKET) {
					printf("Line : %d, playcount : %d,  accept error : %d\n", __LINE__, playerCount, GetLastError());
				}
				SessionArr[playerCount]._ip = clientAddr.sin_addr.s_addr;
				SessionArr[playerCount]._port = clientAddr.sin_port;
				SessionArr[playerCount]._player = new Player;
				playerCount++;
			}
		}

		for (i = 0; i < playerCount; i++) {
			if (FD_ISSET(SessionArr[i]._socket, &readset)) {
				//recv 작업 일단 다이렉트로 넣는 방법 구현
				sock_retval = recv(SessionArr[i]._socket, SessionArr[i]._recvQ.GetRearBufferPtr(),
					SessionArr[i]._recvQ.DirectEnqueueSize(), 0);
				if (sock_retval == SOCKET_ERROR) {
					if (GetLastError() != WSAEWOULDBLOCK) {
						printf("%d recv error : %d\n", __LINE__, GetLastError());
						return false;
					}
				}

				if (SessionArr[i]._recvQ.MoveRear(sock_retval)) {
					printf("%d recvQ MoveRear error %d\n", __LINE__, GetLastError());
					return false;
				}// sock_retval 만큼 moverear;
			}


				if (FD_ISSET(SessionArr[i]._socket, &writeset)) {
					//send 작업
					sock_retval = send(SessionArr[i]._socket, SessionArr[i]._sendQ.GetFrontBufferPtr(),
						SessionArr[i]._sendQ.DirectDequeueSize(), 0);
					if (sock_retval == SOCKET_ERROR) {
						if (GetLastError() != WSAEWOULDBLOCK) {
							printf("%d send error : %d\n", __LINE__, GetLastError());
							return false;
						}
						//send의 wouldblock의 경우에 대한 예외처리 필요
					}
						if (SessionArr[i]._sendQ.MoveFront(sock_retval)) {
							printf("%d sendQ MoveFront error : %d\n", __LINE__, GetLastError());
							return false;
						}
					}
				}
			}
		}

		FD_ZERO(&readset);
		FD_ZERO(&writeset);

		for (i = 0; i < playerCount; i++) {
			FD_SET(SessionArr[i]._socket, &readset);
			FD_SET(SessionArr[i]._socket, &writeset);
		}

		//로직//---------------------------------











	}






	WSACleanup();




	return true;
}