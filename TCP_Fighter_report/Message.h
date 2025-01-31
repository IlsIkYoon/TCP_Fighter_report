#pragma once
#include "Session.h"
#include "resource.h"

//메세지 섹터에 뿌리는 함수 //
void MsgSectorBroadCasting(void (*Func)(char* src, char* dest), char* _src, int SectorX, int SectorY, bool SendMe);
void MsgSectorRSend(void (*Func)(char* src, char* dest), char* _src,int SectorX, int SectorY, bool SendMe);
void MsgSectorLSend(void (*Func)(char* src, char* dest), char* _src,int SectorX, int SectorY, bool SendMe);
void MsgSectorUSend(void (*Func)(char* src, char* dest), char* _src,int SectorX, int SectorY, bool SendMe);
void MsgSectorDSend(void (*Func)(char* src, char* dest), char* _src,int SectorX, int SectorY, bool SendMe);

//char* 인자 한개 더 받는 오버로딩 함수//
void MsgSectorBroadCasting(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src,  char* AttackPacket, int SectorX, int SectorY, bool SendMe);
void MsgSectorRSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe);
void MsgSectorLSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe);
void MsgSectorUSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe);
void MsgSectorDSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe);



//Message패킷 만들어서 Sector에 Send하는 함수
void SendMoveStartMessage(char* src, char* dest);
void SendCreateOtherCharMessage(char* src, char* dest);
void SendDeleteMessage(char* src, char* dest);
void SendMoveStopMessage(char* src, char* dest);
void SendAttack1Message(char* src, char* dest, char* AttackPacket);
void SendAttack2Message(char* src, char* dest, char* AttackPacket);
void SendAttack3Message(char* src, char* dest, char* AttackPacket);
void SendDamageMessage(char* Attack, char* dest, char* Damaged);
void SendSyncMessage(char* src, char* dest);

void RestorePacket(Session* _session, int packetSize, int packetType);



void SyncPos(Session* pSession, int sX, int sY, int cX, int cY);