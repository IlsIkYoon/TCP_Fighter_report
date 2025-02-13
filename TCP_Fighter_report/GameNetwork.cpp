#include "GameNetwork.h"
#include "Session.h"
#include "Sector.h"


SOCKET listen_socket;

extern std::list<Session*> SessionArr;
extern std::list<Session*> Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];


fd_set readset[SELECTCOUNT];
fd_set writeset[SELECTCOUNT];

SOCKADDR_IN clientAddr;


void GameNetwork()
{
	std::list<Session*>::iterator s_ArrIt;
	int iDex;
	int select_retval;
	int recv_retval;
	int send_retval;
	
	TIMEVAL timeout = { 0, 0 };


	AccpetSession();


	if (playerCount < 1) return;

	for (int i = 0; i < SELECTCOUNT; i++)
	{
		FD_ZERO(&readset[i]);
		FD_ZERO(&writeset[i]);
	}

	iDex = 0;
	for (auto session : SessionArr)
	{
		FD_SET(session->_socket, &readset[iDex / 64]);
		FD_SET(session->_socket, &writeset[iDex / 64]);
		iDex++;
	}


	for (DWORD i = 0; i < playerCount / 64 + 1; i++) 
	{
		select_retval = select(NULL, &readset[i], &writeset[i], NULL, &timeout);
		if (select_retval == SOCKET_ERROR)
		{
			//printf("Idex : %d, %d select error : %d\n", i, __LINE__, GetLastError());
			continue;
		}
	}


	iDex = 0;
	for (auto session : SessionArr)
	{
		if (FD_ISSET(session->_socket, &readset[iDex / 64]))
		{
			session->_timeout = timeGetTime();

			recv_retval = recv(session->_socket, session->_recvQ.GetRear(),
				session->_recvQ.GetDirectEnqueSize(), NULL);




			if (recv_retval == 0)
			{

				//printf("delete Session : %d\n", (*s_ArrIt)->_player->_ID);
				DeleteSession(session);
			}

			else if (recv_retval == SOCKET_ERROR && GetLastError() != WSAEWOULDBLOCK)
			{

				if (GetLastError() == 10054 || GetLastError() == 10053)
				{

					//printf("delete Session : %d\n", (*s_ArrIt)->_player->_ID);
					DeleteSession(session);
				}
				else {
					printf("recv error : %d , Line : %d\n", GetLastError(), __LINE__);
				}
			}

			else if(recv_retval != SOCKET_ERROR)
			{
				session->_recvQ.MoveRear(recv_retval); 
			}
			else {
				if (GetLastError() != WSAEWOULDBLOCK)
				{
					printf("recv error : %d , Line : %d\n", GetLastError(), __LINE__);
				}
			}
		}

		if (FD_ISSET(session->_socket, &writeset[iDex / 64]))
		{

			send_retval = send(session->_socket, session->_sendQ.GetFront(),
				session->_sendQ.GetDirectDequeSize(), NULL);



			//Todo// Send값이 적을 경우에 대한 예외처리 필요
			if (send_retval == SOCKET_ERROR && GetLastError() != WSAEWOULDBLOCK)
			{
				if (GetLastError() != 10054 && GetLastError() != 10053)
					printf("send Error : %d, Line : %d\n", GetLastError(), __LINE__);

			}

			else session->_sendQ.MoveFront(send_retval);

		}

		iDex++;
	}




}

void AccpetSession()
{
	Session* newacptSession;
	int addrSize = sizeof(clientAddr);

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
		playerCount++;

	}



}