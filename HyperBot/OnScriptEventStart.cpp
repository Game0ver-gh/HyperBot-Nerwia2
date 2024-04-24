#include "pch.h"

void __fastcall HookManager::OnScriptEventStart(void* pThis, void* edx, int skin, int index)
{
	//Bot only send packets and not simulating event clicks - script windows could stack on each other
	//preventing every mouse input to be sent to the game.
	//This is a workaround for that issue.
	if (!F::route_creator->ShouldOpenScriptWindow())
		return;

	return HookManager::oOnScriptEventStart(pThis, skin, index);
}