#pragma once
#include "Session.h"
#include "resource.h"

void MsgSectorBroadCasting(Session* _session, int x, int y, int MessageType);
void MsgSectorRSend();
void MsgSectorLSend();
void MsgSectorUSend();
void MsgSectorDSend();

void SendMoveStartMessage(char* src, char* dest);
void SendCreateOtherCharMessage(char* src, char* dest);
void SendDeleteMessage(char* src, char* dest);
void SendMoveStopMessage(char* src, char* dest);
void SendAttack1Message(char* src, char* dest, char* AttackPacket);
void SendAttack2Message(char* src, char* dest, char* AttackPacket);
void SendAttack3Message(char* src, char* dest, char* AttackPacket);
void SendDamageMessage(char* Attack, char* Damage, char* dest);