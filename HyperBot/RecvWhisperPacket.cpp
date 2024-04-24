#include "pch.h"


bool __fastcall HookManager::RecvWhisperPacket(void* pThis, void* edx)
{
	//Flash whindow (ex. when metin is minimized) when whisper is received
	FlashWindow(HookManager::hWnd, TRUE);
	return HookManager::oRecvWhisperPacket(pThis);
}