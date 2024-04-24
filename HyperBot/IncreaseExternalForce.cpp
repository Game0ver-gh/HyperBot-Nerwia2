#include "pch.h"


int __fastcall HookManager::IncreaseExternalForce(void* pThis, void* edx, const Vector& base_pos, float force)
{
    if (G::cfg[EBool::M_NO_FLY])
    {
        //Do not disable push force entirely, just reduce it to prevent server side checks
        if (force >= 15.f) force = 0.1f;
        return HookManager::oIncreaseExternalForce(pThis, base_pos, force);
    }
    else return HookManager::oIncreaseExternalForce(pThis, base_pos, force);
}