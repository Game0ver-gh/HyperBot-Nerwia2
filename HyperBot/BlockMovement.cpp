#include "pch.h"


void __fastcall HookManager::BlockMovement(void* pThis)
{
	if (G::cfg[EBool::M_WALLHACK])
		return;

	return HookManager::oBlockMovement(pThis);
}