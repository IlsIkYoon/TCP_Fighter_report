#include "NetworkInit.h"
#include "TCPFighter.h"
#include "Log.h"
#include "GameNetwork.h"

DWORD frame = 0;
DWORD sec;

DWORD g_prevFrameTime;
DWORD g_fixedDeltaTime;


bool TCPFighter() {

	NetworkInit(); 

	DWORD startTime = timeGetTime();

	DWORD dwUpdateTick = startTime - 40;
	sec = startTime / 1000;
	
	g_prevFrameTime = startTime - FrameSec;// 초기 값 설정

	//메인 루프 //-----------------------------------------

	while (1) 
	{
		DWORD currentTime = timeGetTime();
		DWORD deltaTime = currentTime - g_prevFrameTime; // 여기서 델타타임을 딱 맞게 올려줘야겠네 
		DWORD deltaCount = deltaTime / FrameSec;
		g_fixedDeltaTime = deltaCount * FrameSec;



		GameNetwork();

		UpdateGameLogic(g_fixedDeltaTime);


		DWORD logicTime = timeGetTime() - currentTime;

		if (logicTime < FrameSec)
		{
			
			Sleep(FrameSec - logicTime); 
			
		}

		if (sec != timeGetTime() / 1000)
		{
			//1초에 한번 할 일 
			PrintLog();
			sec = timeGetTime() / 1000;
			frame = 0;

		}

		frame++;

		g_prevFrameTime += g_fixedDeltaTime;


	}



	WSACleanup();




	return true;
}