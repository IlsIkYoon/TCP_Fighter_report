#ifndef __GAMEPROC__
#define __GAMEPROC__

#include "resource.h"
#include "Session.h"
//#include "Session2.h"

bool MoveStart(Session* _session);
bool MoveStop(Session* _session);
bool Attack1(Session* _session); 
bool Attack2(Session* _session);
bool Attack3(Session* _session);
bool Sync(Session* _session);
bool Echo(Session* _session);



#endif
