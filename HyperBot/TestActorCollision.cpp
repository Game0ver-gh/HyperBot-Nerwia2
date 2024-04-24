#include "pch.h"


int __fastcall HookManager::TestActorCollision(void* pThis, void* edx, int refActorInstanceVictim)
{
    if (G::cfg[EBool::M_MOB_WALLHACK]) 
        return false;
    
    else return HookManager::oTestActorCollision(pThis, refActorInstanceVictim);
}