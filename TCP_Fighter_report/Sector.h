#pragma once
#include "resource.h"
#include "Session.h"



bool MoveSectorL(Session* _session, int x, int y, int oldX, int oldY);
bool MoveSectorR(Session* _session, int x, int y, int oldX, int oldY);
bool MoveSectorU(Session* _session, int x, int y, int oldX, int oldY);
bool MoveSectorD(Session* _session, int x, int y, int oldX, int oldY);


//-----------------------------------------------------------
// oldSector주변에 양방향으로 _session Delete메세지 전송
// newSector주변에 양방향으로 _session Create메세지 전송
// newSector 기준으로 moveFlag가 켜져 있다면 MoveStart메세지도 바로 전송
//-----------------------------------------------------------
bool SyncSector(Session* _session, int oldSectorX, int oldSectorY, int newSectorX, int newSectorY);


//-----------------------------------------------------------
// 섹터 돌면서 타격 성공한 대상 return해주는 함수 
// 타격에 실패하면 HIT_FAILED 반환
//-----------------------------------------------------------
Session* CheckHit(Session* _session, int rangeX, int rangeY);

