#include "pch.h"


bool __fastcall HookManager::__SendHack(void* pThis, void* edx, const char* msg)
{
	G::console->WriteDebug("%s(): %s\n", __func__, msg);
	return HookManager::o__SendHack(pThis, "https://www.youtube.com/watch?v=dQw4w9WgXcQ"); // B)
}