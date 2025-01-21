#include "NetworkInit.h"


extern SOCKET listen_socket;

bool NetworkInit() {

	WSADATA wsa;
	int wsa_retval;
	int sock_retval;
	SOCKADDR_IN serverAddr;
	u_long non_block = 1;


	ZeroMemory(&serverAddr, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORTNUM);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);




	wsa_retval = WSAStartup(WINSOCK_VERSION, &wsa);
	if (wsa_retval != 0) {
		printf("wsa startup error : %d\n", GetLastError());
		return false;
	}

	listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET) {
		printf("%d socket error : %d\n", __LINE__, GetLastError());
		return false;
	}

	sock_retval = bind(listen_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (sock_retval != 0) {
		printf("%d bind error : %d\n", __LINE__, GetLastError());
		return false;
	}
	printf("bind\n");

	sock_retval = listen(listen_socket, SOMAXCONN);
	if (sock_retval != 0) {
		printf("%d listen error : %d\n", __LINE__, GetLastError());
		return false;
	}
	printf("listen \n");
	ioctlsocket(listen_socket, FIONBIO, &non_block);



	return true;
}