#pragma once
#include "Session.h"
#include "resource.h"

void MsgSectorBroadCasting(void (*Func)(char* src, char* dest), char* _src, int SectorX, int SectorY, bool SendMe);
void MsgSectorRSend(void (*Func)(char* src, char* dest), char* _src,int SectorX, int SectorY, bool SendMe);
void MsgSectorLSend(void (*Func)(char* src, char* dest), char* _src,int SectorX, int SectorY, bool SendMe);
void MsgSectorUSend(void (*Func)(char* src, char* dest), char* _src,int SectorX, int SectorY, bool SendMe);
void MsgSectorDSend(void (*Func)(char* src, char* dest), char* _src,int SectorX, int SectorY, bool SendMe);


void MsgSectorBroadCasting(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src,  char* AttackPacket, int SectorX, int SectorY, bool SendMe);
void MsgSectorRSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe);
void MsgSectorLSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe);
void MsgSectorUSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe);
void MsgSectorDSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe);







void SendMoveStartMessage(char* src, char* dest);
void SendCreateOtherCharMessage(char* src, char* dest);
void SendDeleteMessage(char* src, char* dest);
void SendMoveStopMessage(char* src, char* dest);
void SendAttack1Message(char* src, char* dest, char* AttackPacket);
void SendAttack2Message(char* src, char* dest, char* AttackPacket);
void SendAttack3Message(char* src, char* dest, char* AttackPacket);
void SendDamageMessage(char* Attack, char* dest, char* Damaged);
void SendSyncMessage(char* src, char* dest);