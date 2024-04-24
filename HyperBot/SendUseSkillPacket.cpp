#include "pch.h"

//We can send useSkillPacket to the server without animating the player
//However I disabled that because it's not safe to do so (at least in Nerwia2)

//bool __fastcall HookManager::SendUseSkillPacket(void* pThis, void* edx, DWORD skill, VID vid)
//{
//    auto* local = G::game->GetLocalPlayer();
//    if (local)
//    {
//        auto wasMoving = local->IsWalking();
//        Vector picked_pos;
//        local->GetPickingPosition(&picked_pos);
//        local->StopWalking();
//        auto result = HookManager::oSendUseSkillPacket(pThis, skill, vid);
//        if (wasMoving)
//            local->MoveToDestPixelPosition(picked_pos);
//
//        return result;
//    }
//    else return HookManager::oSendUseSkillPacket(pThis, skill, vid);
//}