#include "TCPFighter.h"
#include "PacketDefine.h"






bool TCPFighter() {


	int wsa_retval;

	WSADATA wsa;


	wsa_retval = WSAStartup(WINSOCK_VERSION, &wsa);
	if (wsa_retval != 0) {
		printf("wsa startup error : %d\n", GetLastError());
		return -1;
	}








	WSACleanup();




	return true;
}