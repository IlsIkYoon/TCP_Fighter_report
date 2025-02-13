#include "Log.h"
#include "TCPFighter.h"


extern DWORD playerCount;
extern DWORD frame;
extern DWORD sec;
extern DWORD timeOutCount;
DWORD SyncMessageCount = 0;
extern HANDLE g_ExitEvent;

void PrintLog()
{
	printf("\n--------------------------------\n");
	printf("PlayerCount : %d\n", playerCount);
	printf("frame : %d\n", frame);
	printf("Sync Message Count : %d\n", SyncMessageCount);
	printf("TimeOut Count : %d\n", timeOutCount);
	printf("--------------------------------\n");
}

unsigned int __stdcall LogThread(void*)
{
	DWORD eventRet;
	while (1)
	{
		if (sec != timeGetTime() / 1000)
		{
			//1초에 한번 할 일 
			PrintLog();
			sec = timeGetTime() / 1000;
			frame = 0;

		}

		eventRet = WaitForSingleObject(g_ExitEvent, 1000 - (timeGetTime() % 1000));

		if (eventRet != WAIT_TIMEOUT) break;

	}

	return 0;
}