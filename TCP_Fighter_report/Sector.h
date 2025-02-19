#pragma once
#include "resource.h"
#include "Session.h"
//#include "Session2.h"


bool MoveSectorL(Session* _session, int x, int y, int oldX, int oldY);
bool MoveSectorR(Session* _session, int x, int y, int oldX, int oldY);
bool MoveSectorU(Session* _session, int x, int y, int oldX, int oldY);
bool MoveSectorD(Session* _session, int x, int y, int oldX, int oldY);
bool MoveSectorRD(Session* _session, int x, int y, int oldX, int oldY);
bool MoveSectorRU(Session* _session, int x, int y, int oldX, int oldY);
bool MoveSectorLU(Session* _session, int x, int y, int oldX, int oldY);
bool MoveSectorLD(Session* _session, int x, int y, int oldX, int oldY);
bool SyncSector(Session* _session, int oldX, int oldY, int newX, int newY);
