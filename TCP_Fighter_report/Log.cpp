#include "Log.h"
#include "TCPFighter.h"


extern DWORD playerIdex;
extern DWORD frame;
extern DWORD sec;
extern DWORD timeOutCount;
DWORD SyncMessageCount = 0;


void PrintLog()
{
	printf("\n--------------------------------\n");
	printf("PlayerCount : %d\n", playerIdex);
	printf("frame : %d\n", frame);
	printf("Sync Message Count : %d\n", SyncMessageCount);
	printf("TimeOut Count : %d\n", timeOutCount);
	printf("--------------------------------\n");
}