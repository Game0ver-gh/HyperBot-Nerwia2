#include "pch.h"


int __fastcall HookManager::ProcessTerrainCollision(void* pThis, void* edx)
{
	if (G::cfg[EBool::M_DISABLE_CAMERA_COLLISION])
		return 0;
	
	return HookManager::oProcessTerrainCollision(pThis);
}