#pragma once
#include "resource.h"
#include "Session.h"
//#include "Session2.h"


bool MoveSectorL(Session* _session, int x, int y, int oldX, int oldY);
bool MoveSectorR(Session* _session, int x, int y, int oldX, int oldY);
bool MoveSectorU(Session* _session, int x, int y, int oldX, int oldY);
bool MoveSectorD(Session* _session, int x, int y, int oldX, int oldY);
