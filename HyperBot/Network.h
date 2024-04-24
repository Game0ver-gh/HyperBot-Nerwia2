#pragma once
#include "Item.h"

enum EChatType;
class Entity;

class Network
{
public:
	bool		SendChatPacket(const char* msg, EChatType type);
	bool		SendAttackPacket(VID vid, UINT uMotion = 0);
	bool		SendPickUpItemPacket(VID vid);
	bool		__SendHack(int encrypted_msg);
	bool		SendAddFlyTargetingPacket(VID dwTargetVID, const Vector& kPPosTarget);
	bool		SendFlyTargetingPacket(VID dwTargetVID, const Vector& kPPosTarget);
	bool		SendSyncPositionPacket(VID dwVictimVID, DWORD dwVictimX, DWORD dwVictimY);
	bool		SendOnClickPacket(VID vid);
	bool		SendScriptAnswerPacket(BYTE answer);
	bool		SendGiveItemPacket(VID vid, TItemPos itemPos, BYTE count);
	bool		SendItemUsePacket(TItemPos itemPos);
	bool		SendItemUseToItemPacket(TItemPos source_pos, TItemPos target_pos);
	bool		SendShootPacket(UINT uiSkill);
	bool		SendUseSkillPacket(DWORD dwSkillIndex, VID dwTargetVID);
	bool		SendTargetPacket(VID vid);
	bool		SendCharacterStatePacket(const Vector& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg);
	bool		SendRefinePacket(BYTE pos, BYTE type);
	bool		Recv(int size, void* pBuffer);
	int			__Warp(const Vector& dst);
	bool		IsConnected();
	bool		ConnectGameServer(UINT charSlot);
	void		SetLoginInfo(const char* login, const char* password);
	void		SelectAnswer(int index, BYTE answer);
	size_t		GetPacketsPerSec();
	size_t		GetPacketsCount();
	size_t		GetAvailablePackets();
	size_t		GetMaxPacketsCount();
	void		UpdatePacketSent();
	Entity*		GetTargetEntity();
	bool		Send(int len, const void* buf);
	void		NotifyShowMap();

private:
	bool		ShouldSend();
};

enum EChatType
{
	CHAT_TYPE_TALKING,
	CHAT_TYPE_INFO,
	CHAT_TYPE_NOTICE,
	CHAT_TYPE_PARTY,
	CHAT_TYPE_GUILD,
	CHAT_TYPE_COMMAND,
	CHAT_TYPE_SHOUT,
	CHAT_TYPE_WHISPER,
	CHAT_TYPE_BIG_NOTICE,
	CHAT_TYPE_MAX_NUM,
};