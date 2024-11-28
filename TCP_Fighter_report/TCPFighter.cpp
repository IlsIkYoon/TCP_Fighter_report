#include "TCPFighter.h"
#include "PacketDefine.h"






bool TCPFighter() {

	//return value //-----------------------------
	int wsa_retval;
	int sock_retval;


	//socket inteligense//----------------
	WSADATA wsa;
	SOCKADDR_IN serverAddr;
	SOCKADDR_IN clientAddr;

	ZeroMemory(&serverAddr, sizeof(serverAddr));
	ZeroMemory(&clientAddr, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORTNUM);



	//select inteligense//----------------------------
	fd_set readset;
	fd_set writeset;



	//NetWork//---------------------------

	wsa_retval = WSAStartup(WINSOCK_VERSION, &wsa);
	if (wsa_retval != 0) {
		printf("wsa startup error : %d\n", GetLastError());
		return -1;
	}








	WSACleanup();




	return true;
}