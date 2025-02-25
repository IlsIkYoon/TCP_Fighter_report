#include "Log.h"
#include "TCPFighter.h"


extern DWORD playerCount;
extern DWORD frame;
extern DWORD sec;
extern DWORD timeOutCount;
DWORD SyncMessageCount = 0;
extern HANDLE g_ExitEvent;

std::list<std::string> LogQ;
CRITICAL_SECTION g_lock;

DWORD messageErrorCount = 0;
DWORD errorCount = 0;


void PrintLog()
{
	printf("\n--------------------------------\n");
	printf("Error Count : %d\n", errorCount);
	printf("PlayerCount : %d\n", playerCount);
	printf("frame : %d\n", frame);
	printf("Sync Message Count : %d\n", SyncMessageCount);
	printf("TimeOut Count : %d\n", timeOutCount);
	//printf("Message Error Count : %d\n", messageErrorCount);
	printf("Message Error Count : Disabled\n");
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

			if (frame < FrameError)
			{
				WriteLog();

			}

			if (LogQ.size() > 0)
			{
				WriteLogQToFile();
			}

			frame = 0;

		}

		eventRet = WaitForSingleObject(g_ExitEvent, 1000 - (timeGetTime() % 1000));

		if (eventRet != WAIT_TIMEOUT) break;

	}

	return 0;
}


void WriteLog()
{
	FILE* fpWrite;
	char fileName[40] = { 0, };
	strcpy_s(fileName, __DATE__);
	strcat_s(fileName, "Log.txt");

	fopen_s(&fpWrite, fileName, "a");

	if (fpWrite == 0)
	{
		EnqueLog("Log File Open", __FILE__, __func__, __LINE__, GetLastError());

		return;
	}



	fprintf(fpWrite, "\n--------------------------------\n");
	fprintf(fpWrite, "Error Count : %d\n", errorCount);
	fprintf(fpWrite, "PlayerCount : %d\n", playerCount);
	fprintf(fpWrite, "frame : %d\n", frame);
	fprintf(fpWrite, "Sync Message Count : %d\n", SyncMessageCount);
	fprintf(fpWrite, "TimeOut Count : %d\n", timeOutCount);
	//fprintf(fpWrite, "Message Error Count : %d\n", messageErrorCount);
	fprintf(fpWrite, "Message Error Count : Disabled\n");
	fprintf(fpWrite, "--------------------------------\n");

	fclose(fpWrite);

}

void WriteLogQToFile()
{
	FILE* fpWrite;
	char fileName[40] = { 0, };
	strcpy_s(fileName, __DATE__);
	strcat_s(fileName, "Log.txt");

	fopen_s(&fpWrite, fileName, "a");
	if (fpWrite == 0)
	{
		EnqueLog("Log File Open", __FILE__, __func__, __LINE__, GetLastError());

		return;
	}

	while (1)
	{
		if (LogQ.size() == 0) break;
		EnterCriticalSection(&g_lock);
		std::string logEntry = LogQ.front();
		LogQ.pop_front();
		LeaveCriticalSection(&g_lock);
		fwrite(logEntry.c_str(), 1, logEntry.size(), fpWrite);
	}

	fclose(fpWrite);
}



std::string getFileName(const std::string& path) {
	return path.substr(path.find_last_of("/\\") + 1);
}

void EnqueLog(const char* name, const char* FileName, const char* FuncName, int Line, int errorCode)
{
	errorCount++;

	std::string logEntry = std::format("{} Error || FILE : {}, Func : {} , Line : {} error : {}\n",
		name, getFileName(__FILE__), __func__, __LINE__, GetLastError());
	EnterCriticalSection(&g_lock);
	LogQ.push_back(logEntry);
	LeaveCriticalSection(&g_lock);


}