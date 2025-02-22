#pragma once
#include "Session.h"
#include "resource.h"
//------------------------------------------------------
//메세지 섹터에 뿌리는 함수 , sendMe에 true가 오면 자기에게도 보냄
//------------------------------------------------------
void MsgSectorBroadCasting(void (*Func)(char* src, char* dest), char* _src, int SectorX, int SectorY, bool SendMe);
void MsgSectorRSend(void (*Func)(char* src, char* dest), char* _src,int SectorX, int SectorY, bool SendMe);
void MsgSectorLSend(void (*Func)(char* src, char* dest), char* _src,int SectorX, int SectorY, bool SendMe);
void MsgSectorUSend(void (*Func)(char* src, char* dest), char* _src,int SectorX, int SectorY, bool SendMe);
void MsgSectorDSend(void (*Func)(char* src, char* dest), char* _src,int SectorX, int SectorY, bool SendMe);


//------------------------------------------------------
//char* 인자 한개 더 받는 Attack message를 위한 오버로딩 함수, sendMe에 true가 오면 자기에게도 보냄
//------------------------------------------------------
void MsgSectorBroadCasting(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src,  char* AttackPacket, int SectorX, int SectorY, bool SendMe);
void MsgSectorRSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe);
void MsgSectorLSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe);
void MsgSectorUSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe);
void MsgSectorDSend(void (*Func)(char* src, char* dest, char* AttackPacket), char* _src, char* AttackPacket, int SectorX, int SectorY, bool SendMe);



//Message패킷 만들어서 Sector에 Send하는 함수

//------------------------------------------------------
//src의 MoveStsart Message패킷 만들어서 dest의 SendBuf에 Enque하는 함수
//------------------------------------------------------
void SendMoveStartMessage(char* src, char* dest);

//------------------------------------------------------
//src의 CreateOtherChar Message패킷 만들어서 dest의 SendBuf에 Enque하는 함수
//------------------------------------------------------
void SendCreateOtherCharMessage(char* src, char* dest);

//------------------------------------------------------
//src의 CreatemyChar Message패킷 만들어서 src의 SendBuf에 Enque하는 함수
//------------------------------------------------------
void SendCreateMyCharMessage(char* src);


//------------------------------------------------------
//src의 주변 섹터에 있는 캐릭터들에 대한 CreateOthermessage 패킷을 만들어서 src의 SendBuf에 Enque
//Move플래그가 켜져 있으면 MoveStart 패킷도 보내줌
//------------------------------------------------------
void SendCreateSurroundCharMessage(char* src);

//------------------------------------------------------
//src의 Deletemessage를 dest의 SendBuf에 Enque
//------------------------------------------------------
void SendDeleteMessage(char* src, char* dest);

//------------------------------------------------------
//src의 MoveStop Message를 dest의 SendBuf에 Enque
//------------------------------------------------------
void SendMoveStopMessage(char* src, char* dest);

//------------------------------------------------------
//dest의 SendBuf에 AttackPacket의 Dir과 src의 좌표를 이용해 Attack Message를 만들어서 Enque
//------------------------------------------------------
void SendAttack1Message(char* src, char* dest, char* AttackPacket);
//------------------------------------------------------
//dest의 SendBuf에 AttackPacket의 Dir과 src의 좌표를 이용해 Attack Message를 만들어서 Enque
//------------------------------------------------------
void SendAttack2Message(char* src, char* dest, char* AttackPacket);
//------------------------------------------------------
//dest의 SendBuf에 AttackPacket의 Dir과 src의 좌표를 이용해 Attack Message를 만들어서 Enque
//------------------------------------------------------
void SendAttack3Message(char* src, char* dest, char* AttackPacket);

//------------------------------------------------------
//dest의 SendQ에 Damage패킷 만들어서 Enque
//------------------------------------------------------
void SendDamageMessage(char* Attack, char* dest, char* Damaged);
//------------------------------------------------------
//현 시점에서 쓰는 순간이 있나?
//------------------------------------------------------
void SendSyncMessage(char* src, char* dest);



//------------------------------------------------------
//buf에 남은 데이터를 전부 빼고 헤더를 넣고 뺀 데이터를 다시 넣어주는 함수
//------------------------------------------------------
void RestorePacket(Session* _session, int packetSize, int packetType);



//----------------------------------------------------------------
// 서버의 좌표를 SyncMessage에 실어서 pSession에게 보내고 SyncMessageCount++;
// 서버의 좌표로 클라이언트 동기화
//----------------------------------------------------------------
void SyncPos(Session* pSession);