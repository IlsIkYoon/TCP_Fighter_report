#include "TCPFighter.h"
#include "Log.h"


SOCKET listen_socket;

DWORD frame = 0;
DWORD sec;



extern DWORD playerIdex; 

extern std::list<Session*> SessionArr;
extern std::list<Session*>::iterator s_ArrIt;

extern std::vector<Session*> DeleteArr; //지연삭제를 위한 자료구조

extern std::list<Session*> Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];

Session* pSector = (Session*)Sector;

fd_set readset[SELECTCOUNT];
fd_set writeset[SELECTCOUNT];


//프레임이 떨어지면 로직을 더 돌리는 방식으로 

bool TCPFighter() {
	int i;
	int iDex;
	srand(50);

	

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
	Session* listen_session;
	listen_session = new Session;
	ZeroMemory(listen_session, sizeof(Session));
	listen_session->_socket = listen_socket;
	
	SessionArr.push_back(listen_session);
	playerIdex++;

	DWORD dwUpdateTick = timeGetTime() - 40;
	sec = timeGetTime() / 1000;
	
	//메인 루프 //-----------------------------------------

	while (1) 
	{
		dwUpdateTick += 40; // 25프레임 맞추기 위해 
		//네트워크//----------------------------

		for (int i = 0; i < SELECTCOUNT; i++)
		{
			FD_ZERO(&readset[i]);
			FD_ZERO(&writeset[i]);
		}


		//FD_SET(listen_socket, &readset[0]);
	
		Session* newacptSession;
		
		for (int acptIdex = 0; acptIdex < 100; acptIdex++)
		{
			newacptSession = nullptr;
			newacptSession = new Session;
			ZeroMemory(newacptSession, sizeof(newacptSession));
			newacptSession->_socket = accept(listen_socket, (SOCKADDR*)&clientAddr, &addrSize);

			if (newacptSession->_socket == INVALID_SOCKET)
			{
				delete newacptSession;
				break;
			}

			if (newacptSession->_socket == SOCKET_ERROR && GetLastError() == WSAEWOULDBLOCK)
			{
				delete newacptSession;
				break;
			}

			newacptSession->_ip = clientAddr.sin_addr.s_addr;
			newacptSession->_port = clientAddr.sin_port;
			newacptSession->_player = new Player(newacptSession);
			newacptSession->_delete = false; //delete에 false;
			Sector[newacptSession->_player->_x / SECTOR_RATIO][newacptSession->_player->_y / SECTOR_RATIO].push_back(newacptSession);

			CreateNewCharacter(newacptSession);

			SessionArr.push_back(newacptSession);
			playerIdex++;

		}


		/*
		for (auto session : SessionArr)
		{
			FD_SET(session->_socket, 
		}
		
		*/
		
		iDex = 0;
		for (s_ArrIt = SessionArr.begin(); s_ArrIt != SessionArr.end(); s_ArrIt++)
		{
			FD_SET((*s_ArrIt)->_socket, &readset[iDex / 64]);
			FD_SET((*s_ArrIt)->_socket, &writeset[iDex / 64]);
			iDex++;
		}


		for (i = 0; i < ((playerIdex - 1) / 64 + 1); i++)
		{
			select_retval = select(NULL, &readset[i], &writeset[i], NULL, &timeout);
			if (select_retval == SOCKET_ERROR)
			{
				printf("Idex : %d, %d select error : %d\n", i, __LINE__, GetLastError());
			}
		}

		//받은 데이터나 보낼 데이터가 있다면 수신 링버퍼에 넣어주는 과정

		
		s_ArrIt = SessionArr.begin();
		s_ArrIt++; //listenSocket은 안 건드림
		iDex = 1;
		for (; s_ArrIt != SessionArr.end(); s_ArrIt++)
		{
			if (FD_ISSET((*s_ArrIt)->_socket, &readset[iDex / 64]))
			{
				(*s_ArrIt)->_timeout = timeGetTime();

				recv_retval = recv((*s_ArrIt)->_socket, (*s_ArrIt)->_recvQ.GetRear(),
					(*s_ArrIt)->_recvQ.GetDirectEnqueSize(), NULL);

				
				

				if (recv_retval == 0)
				{
					
					//printf("delete Session : %d\n", (*s_ArrIt)->_player->_ID);
					DeleteSession(*s_ArrIt);
				}

				else if (recv_retval == SOCKET_ERROR && GetLastError() != WSAEWOULDBLOCK)
				{
					
					if (GetLastError() == 10054 || GetLastError() == 10053)
					{

						//printf("delete Session : %d\n", (*s_ArrIt)->_player->_ID);
						DeleteSession(*s_ArrIt); 
					}
					else {
						printf("recv error : %d , Line : %d\n", GetLastError(), __LINE__);
					}
				}

				else (*s_ArrIt)->_recvQ.MoveRear(recv_retval);

			}

			if (FD_ISSET((*s_ArrIt)->_socket, &writeset[iDex / 64]))
			{
				
				send_retval = send((*s_ArrIt)->_socket, (*s_ArrIt)->_sendQ.GetFront(),
					(*s_ArrIt)->_sendQ.GetDirectDequeSize(), NULL);

				

				//Todo// Send값이 적을 경우에 대한 예외처리 필요
				if (send_retval == SOCKET_ERROR && GetLastError() != WSAEWOULDBLOCK)
				{
					if(GetLastError() != 10054 && GetLastError() != 10053)
					printf("send Error : %d, Line : %d\n", GetLastError(), __LINE__);
		
				}

				else (*s_ArrIt)->_sendQ.MoveFront(send_retval);

			}

			iDex++;
		}




		if (FD_ISSET(listen_socket, &readset[0])) //캐릭터 초기 생성
		{
			if (playerIdex >= PLAYERMAXCOUNT) 
			{
				printf("playercount overflow !! , PlayerCount : %d\n", playerIdex);
			}
			else 
			{
				Session* newSession = new Session;

				

				
				newSession->_socket= accept(listen_socket, (SOCKADDR*)&clientAddr, &addrSize);
				if (newSession->_socket == INVALID_SOCKET)
				{
					printf("Line : %d, playcount : %d,  accept error : %d\n", __LINE__, playerIdex, GetLastError());
					delete newSession;
				}
				else {
					newSession->_ip = clientAddr.sin_addr.s_addr;
					newSession->_port = clientAddr.sin_port;
					newSession->_player = new Player(newSession);
					newSession->_delete = false; //delete에 false;
					Sector[newSession->_player->_x / SECTOR_RATIO][newSession->_player->_y / SECTOR_RATIO].push_back(newSession);
					
					//초기 생성에 대한 작업들
					CreateNewCharacter(newSession);

					SessionArr.push_back(newSession);
					playerIdex++;
				}

			}
		}

	//로직//--------------------------------
	

		//recvq에 있는 데이터를 뜯어보고 sendq에 데이터를 넣어 줌 
		if (playerIdex > 1) {


			s_ArrIt = SessionArr.begin();
			s_ArrIt++;
			for (; s_ArrIt != SessionArr.end(); s_ArrIt++)
			{
				if ((*s_ArrIt)->_recvQ.IsEmpty() == false) {
					
					DecodeMessages(*s_ArrIt);

				}
			}
		}


		if (playerIdex > 1) {
			s_ArrIt = SessionArr.begin();
			s_ArrIt++; //listenSocket 넘기기

			//listensocket 다음거를 바라보는 상황

			for (; s_ArrIt != SessionArr.end(); s_ArrIt++)
			{
				
				(*s_ArrIt)->_player->Move();

				
			}
		}

		
		TimeOutCheck(); //timeOut로직을 그대로 둬도 컨텐츠 오류가 나지 않는지 체크 // 현재 TimeOut이랑 CloseSocket이랑 겹침
		FlushDeleteArr();


		DWORD dwTick2 = timeGetTime() - dwUpdateTick;
		if (dwTick2 < 40)
		{

			if (sec != timeGetTime() / 1000)
			{
				//1초에 한번 할 일 
				PrintLog();
				sec = timeGetTime() / 1000;
				frame = 0;

			}

			
			dwTick2 +=  timeGetTime() - (dwTick2 + dwUpdateTick); //렌더링 된 시간만큼 더해줌 
			

			if (40 - dwTick2 < 0) __debugbreak();
			Sleep(40 - dwTick2); //??? 슬립이 길게 걸렸다는 의미 같은데 
			
		}


		frame++;




	}



	WSACleanup();




	return true;
}