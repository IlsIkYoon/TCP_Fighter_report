#include "NetworkInit.h"
#include "TCPFighter.h"
#include "Log.h"
#include "GameNetwork.h"
#include <process.h>

DWORD frame = 0;
DWORD sec;

DWORD g_prevFrameTime;
DWORD g_fixedDeltaTime;
HANDLE logThread;
HANDLE g_ExitEvent;

bool TCPFighter() {

	NetworkInit(); 

	DWORD startTime = timeGetTime();

	DWORD dwUpdateTick = startTime - 40;
	sec = startTime / 1000;
	
	g_prevFrameTime = startTime - FrameSec;// 초기 값 설정

	g_ExitEvent = CreateEvent(NULL, true, false, NULL);
	logThread = (HANDLE)_beginthreadex(NULL, 0, LogThread, NULL, NULL, NULL);


	//메인 루프 //-----------------------------------------

	while (1) 
	{
		DWORD currentTime = timeGetTime();
		DWORD deltaTime = currentTime - g_prevFrameTime; 
		DWORD deltaCount = deltaTime / FrameSec;
		g_fixedDeltaTime = deltaCount * FrameSec;



		GameNetwork();

		UpdateGameLogic(g_fixedDeltaTime);


		DWORD logicTime = timeGetTime() - currentTime;

		if (logicTime < FrameSec)
		{
			
			Sleep(FrameSec - logicTime); 
		}

		frame++;

		g_prevFrameTime += g_fixedDeltaTime;


	}



	WSACleanup();
	SetEvent(g_ExitEvent);
	WaitForSingleObject(LogThread, INFINITE);
	CloseHandle(g_ExitEvent);
	CloseHandle(LogThread);

	return true;
}