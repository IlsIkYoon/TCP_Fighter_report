#include "GameNetwork.h"
#include "Session.h"
#include "Sector.h"
#include "Log.h"


SOCKET listen_socket;

extern std::list<Session*> SessionArr;
extern std::list<Session*> Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];

extern std::list<std::string> LogQ;

fd_set readset[SELECTCOUNT];
fd_set writeset[SELECTCOUNT];

SOCKADDR_IN clientAddr;

extern CRITICAL_SECTION g_lock;

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
		if (select_retval == SOCKET_ERROR && GetLastError() != 10022)
		{
			std::string logEntry = std::format("Select Error || FILE : %s, Func : %s , Line : %d error : %d\n",
				getFileName(__FILE__), __func__, __LINE__, GetLastError());
			EnterCriticalSection(&g_lock);
			LogQ.push_back(logEntry);
			LeaveCriticalSection(&g_lock);
			

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

				
				DeleteSession(session);
			}

			else if (recv_retval == SOCKET_ERROR && GetLastError() != WSAEWOULDBLOCK)
			{

				if (GetLastError() == 10054 || GetLastError() == 10053)
				{

					
					DeleteSession(session);
				}
				else {
					std::string logEntry = std::format("Recv Error || FILE : %s, Func : %s , Line : %d error : %d\n",
						getFileName(__FILE__), __func__, __LINE__, GetLastError());
					EnterCriticalSection(&g_lock);
					LogQ.push_back(logEntry);
					LeaveCriticalSection(&g_lock);

				}
			}

			else if(recv_retval != SOCKET_ERROR)
			{
				session->_recvQ.MoveRear(recv_retval); 
			}
			else {
				if (GetLastError() != WSAEWOULDBLOCK)
				{
					std::string logEntry = std::format("Recv Error || FILE : %s, Func : %s , Line : %d error : %d\n",
						getFileName(__FILE__), __func__, __LINE__, GetLastError());
					EnterCriticalSection(&g_lock);
					LogQ.push_back(logEntry);
					LeaveCriticalSection(&g_lock);
				}
			}
		}

		if (FD_ISSET(session->_socket, &writeset[iDex / 64]))
		{

			send_retval = send(session->_socket, session->_sendQ.GetFront(),
				session->_sendQ.GetDirectDequeSize(), NULL);



			
			if (send_retval == SOCKET_ERROR && GetLastError() != WSAEWOULDBLOCK)
			{
				if (GetLastError() != 10054 && GetLastError() != 10053){
					std::string logEntry = std::format("Send Error || FILE : %s, Func : %s , Line : %d error : %d\n",
						getFileName(__FILE__), __func__, __LINE__, GetLastError());
					EnterCriticalSection(&g_lock);
					LogQ.push_back(logEntry);
					LeaveCriticalSection(&g_lock);
				}
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
			std::string logEntry = std::format("Accept Error - INVALID SOCKET || FILE : %s, Func : %s , Line : %d error : %d\n",
				getFileName(__FILE__), __func__, __LINE__, GetLastError());
			EnterCriticalSection(&g_lock);
			LogQ.push_back(logEntry);
			LeaveCriticalSection(&g_lock);
			delete newacptSession;
			break;
		}

		if (newacptSession->_socket == SOCKET_ERROR && GetLastError() == WSAEWOULDBLOCK)
		{
			std::string logEntry = std::format("Accept Error || FILE : %s, Func : %s , Line : %d error : %d\n",
				getFileName(__FILE__), __func__, __LINE__, GetLastError());
			EnterCriticalSection(&g_lock);
			LogQ.push_back(logEntry);
			LeaveCriticalSection(&g_lock);
			delete newacptSession;
			break;
		}

		newacptSession->_ip = clientAddr.sin_addr.s_addr;
		newacptSession->_port = clientAddr.sin_port;
		newacptSession->_player = new Player(newacptSession);
		newacptSession->_delete = false; //delete¿¡ false;
		Sector[newacptSession->_player->_x / SECTOR_RATIO][newacptSession->_player->_y / SECTOR_RATIO].push_back(newacptSession);

		CreateNewCharacter(newacptSession);

		SessionArr.push_back(newacptSession);
		playerCount++;

	}



}