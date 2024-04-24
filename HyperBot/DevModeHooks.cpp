#include "pch.h"

void __fastcall HookManager::__ShowMapName(void* pThis, void* edx, LONG x, LONG y)
{
	G::game->GetNetworkMgr()->NotifyShowMap();

	HookManager::o__ShowMapName(pThis, x, y);
}

//stack then move to inv
/*
[Send -> HEADER_CG_SAFEBOX_ITEM_MOVE]
4D 01 11 00 01 00 00 01                           |  M.......

[Send -> HEADER_CG_SAFEBOX_CHECKOUT]
47 00 01 4C 00                                    |  G..L.
*/

bool __fastcall HookManager::SendAttackPacket(void* pThis, void* edx, UINT motion, VID vid)
{
	if (G::cfg[EBool::WH_ENABLED] && G::cfg[EBool::WH_IGNORE_NORMAL_ATTACKS] && G::game->GetLocalPlayer()->IsAttacking())
	{
		if ((DWORD)_ReturnAddress() == 0x58DFD7) //Sig that!
			return true;
	}
	return HookManager::oSendAttackPacket(pThis, motion, vid);
}

#ifdef DEV_MODE

bool __fastcall HookManager::SendOnClickPacket(void* pThis, void* edx, VID vid)
{
	DEV_PRINTF("VID = %d", vid);
	return HookManager::oSendOnClickPacket(pThis, vid);
}

bool __fastcall HookManager::SendScriptAnswerPacket(void* pThis, void* edx, BYTE answer)
{
	DEV_PRINTF("Answer = %d", (int)answer);
	return HookManager::oSendScriptAnswerPacket(pThis, answer);
}

bool __fastcall HookManager::SendGiveItemPacket(void* pThis, void* edx, VID vid, TItemPos item_pos, BYTE count)
{
	DEV_PRINTF("VID = %d, cell = %d, win_type = %d, count = %d", vid, item_pos.cell, item_pos.window_type, (int)count);
	return HookManager::oSendGiveItemPacket(pThis, vid, item_pos, count);
}

bool __fastcall HookManager::SendShootPacket(void* pThis, void* edx, UINT skill)
{
	DEV_PRINTF("Skill = %d", (int)skill);
	return HookManager::oSendShootPacket(pThis, skill);
}

bool __fastcall HookManager::SendTargetPacket(void* pThis, void* edx, VID vid)
{
	DEV_PRINTF("VID = %d", vid);
	return HookManager::oSendTargetPacket(pThis, vid);
}

bool __fastcall HookManager::SendAddFlyTargetingPacket(void* pThis, void* edx, VID vid, Vector& pos)
{
	DEV_PRINTF("VID = %d, pos = (%f, %f, %f)", vid, pos.x, pos.y, pos.z);
	return HookManager::oSendAddFlyTargetingPacket(pThis, vid, pos);
}

bool __fastcall HookManager::SendFlyTargetingPacket(void* pThis, void* edx, VID vid, Vector& pos)
{
	DEV_PRINTF("VID = %d, pos = (%f, %f, %f)", vid, pos.x, pos.y, pos.z);
	return HookManager::oSendFlyTargetingPacket(pThis, vid, pos);
}

bool __fastcall HookManager::SendScriptButtonPacket(void* pThis, void* edx, int button_id)
{
	DEV_PRINTF("Button ID = %d", button_id);
	return HookManager::oSendScriptButtonPacket(pThis, button_id);
}

bool __fastcall HookManager::SendRefinePacket(void* pThis, void* edx, BYTE pos, BYTE type)
{
	DEV_PRINTF("Pos = %d, type = %d", (int)pos, (int)type); //blacksmith = 0; DT = 4
	return HookManager::oSendRefinePacket(pThis, pos, type);
}

bool __fastcall HookManager::SendSyncPositionPacket(void* pThis, void* edx, VID vid, DWORD x, DWORD y)
{
	DEV_PRINTF("VID = %d, x = %d, y = %d", vid, (int)x, (int)y); 
	return HookManager::oSendSyncPositionPacket(pThis, vid, x, y);
}

bool __fastcall HookManager::SendCharacterStatePacket(void* pThis, void* edx, const Vector& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg)
{
	DEV_PRINTF("pos = (%f, %f, %f), rot = %f, eFunc = %d, uArg = %d, dist = %f", c_rkPPosDst.x, c_rkPPosDst.y, c_rkPPosDst.z, fDstRot, (int)eFunc, (int)uArg, c_rkPPosDst.Distance(prev_pos));
	return HookManager::oSendCharacterStatePacket(pThis, c_rkPPosDst, fDstRot, eFunc, uArg);
}

bool __fastcall HookManager::SendUseSkillPacket(void* pThis, void* edx, DWORD dwSkillIndex, VID dwTargetVID)
{
	DEV_PRINTF("Skill index = %d, target VID = %d", (int)dwSkillIndex, (int)dwTargetVID);
	return HookManager::oSendUseSkillPacket(pThis, dwSkillIndex, dwTargetVID);
}

#endif