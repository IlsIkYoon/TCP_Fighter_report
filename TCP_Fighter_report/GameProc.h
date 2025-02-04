#ifndef __GAMEPROC__
#define __GAMEPROC__

#include "resource.h"
#include "Session.h"



//------------------------------------------------------//
//게임에서 프레임마다 진행되어야 하는 컨텐츠 로직
//	DecodeMessages, Character Move, Delete, TimeOutcheck 기능
//------------------------------------------------------//
void UpdateGameLogic(DWORD deltaTime);


bool MoveStart(Session* _session);
bool MoveStop(Session* _session);
bool Attack1(Session* _session); 
bool Attack2(Session* _session);
bool Attack3(Session* _session);
bool Sync(Session* _session);
bool Echo(Session* _session);



#endif
