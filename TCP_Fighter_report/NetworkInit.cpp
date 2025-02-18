#include "NetworkInit.h"


extern SOCKET listen_socket;
extern CRITICAL_SECTION g_lock;


bool NetworkInit()
{

	WSADATA wsa;
	int wsa_retval;
	int sock_retval;
	SOCKADDR_IN serverAddr;
	u_long non_block = 1;

	srand(50);
	timeBeginPeriod(1);

	InitializeCriticalSection(&g_lock);

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


	sock_retval = listen(listen_socket, SOMAXCONN_HINT(65536));
	if (sock_retval != 0) {
		printf("%d listen error : %d\n", __LINE__, GetLastError());
		return false;
	}
	printf("listen \n");
	ioctlsocket(listen_socket, FIONBIO, &non_block);

	struct linger ling;
	ling.l_onoff = 1;  // linger 옵션 활성화
	ling.l_linger = 0;  // linger 시간을 10초로 설정

	setsockopt(listen_socket, SOL_SOCKET, SO_LINGER, (char*)&ling, sizeof(ling)


	return true;
}