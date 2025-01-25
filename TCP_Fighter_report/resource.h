#ifndef __RESOURCE__
#define __RESOURCE__


#pragma comment(lib, "Ws2_32")
#pragma comment (lib, "Winmm.lib")



#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <list>

#define PORTNUM 5000
#define GAMEFPS 50
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define SECTOR_RATIO 200 //100분의 1 비율 6400에서 나눠서 떨어지는 수를 하는게 좋다

#define PLAYERMAXCOUNT 7000
#define SELECTCOUNT 110
#define SELECTDEFINE 64

//----------------------------------------------------------------
//# 화면 이동영역-------------------------- -
//----------------------------------------------------------------
#define dfRANGE_MOVE_TOP	0
#define dfRANGE_MOVE_LEFT	0
#define dfRANGE_MOVE_RIGHT	6400
#define dfRANGE_MOVE_BOTTOM	6400




//-----------------------------------------------------------------
// 이동 오류체크 범위
//-----------------------------------------------------------------
#define dfERROR_RANGE		50


//---------------------------------------------------------------
// 공격범위.
//---------------------------------------------------------------
#define dfATTACK1_RANGE_X		80
#define dfATTACK2_RANGE_X		90
#define dfATTACK3_RANGE_X		100
#define dfATTACK1_RANGE_Y		10
#define dfATTACK2_RANGE_Y		10
#define dfATTACK3_RANGE_Y		20


//-------------------------------------------------------------
// 기본 설정
//-------------------------------------------------------------

#define dfHP 100
#define dfAttack1Damage 1
#define dfAttack2Damage 2
#define dfAttack3Damage 3

#define LL 0x00
#define RR 0x04

/*

서버 포트 5000

게임프레임 : 50fps
화면사이즈 : 640x480
컬러 비트 : 32bit

Z : 주먹
X : 큰주먹
C : 돌려차기

*/
//# 화면 이동영역 ---------------------------
/*
#define dfRANGE_MOVE_TOP	50
#define dfRANGE_MOVE_LEFT	10
#define dfRANGE_MOVE_RIGHT	630
#define dfRANGE_MOVE_BOTTOM	470
*/
/*
위 좌표에 가지 못하게 해야하며,
해당 좌표에 닿는 경우 움직임을 멈추어야 함.
(작거나 같으면 멈춤)

예) 왼쪽 상단으로 이동 중 왼쪽 범위에 걸리면
상단으로 벽을 타고 이동하는게 아니며, 그자리에 멈춰야 함.

왜냐면!! 클라이언트가 그렇게 만들어 졌기 때문에, 서버도 그렇게 만들어 져야만 함


# 프레임당 이동 단위 -----------------------------------

X 축 - 3
Y 축 - 2

//-----------------------------------------------------------------
// 이동 오류체크 범위
//-----------------------------------------------------------------


//---------------------------------------------------------------
// 공격범위.
//---------------------------------------------------------------
*/
#define dfATTACK1_RANGE_X		80
#define dfATTACK2_RANGE_X		90
#define dfATTACK3_RANGE_X		100
#define dfATTACK1_RANGE_Y		10
#define dfATTACK2_RANGE_Y		10
#define dfATTACK3_RANGE_Y		20


#define dfERROR_RANGE		50

//// 30초 이상이 되도록 아무런 메시지 수신도 없는경우 접속 끊음.
#define dfNETWORK_PACKET_RECV_TIMEOUT	30000


//-----------------------------------------------------------------
// 캐릭터 이동 속도   // 25fps 기준 이동속도
//-----------------------------------------------------------------
#define dfSPEED_PLAYER_X	3//6	// 3   50fps
#define dfSPEED_PLAYER_Y	2//4	// 2   50fps




#endif