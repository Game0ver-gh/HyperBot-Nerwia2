#include "pch.h"
#include "Backend.h"


HookManager::HookManager()
{
	created_hooks = 0;
	hWnd = FindWindowA(NULL, WINDOW_NAME);
	
	Clock timer;
	timer.Begin(); 

	auto err = MH_Initialize();
	if (err != MH_OK)
	{
		G::console->WriteError("MH_Initialize() failed with error %s!\n", MH_StatusToString(err));
		F::should_exit = true;
		return;
	}
	
	DX8::device = *(IDirect3DDevice8**)(*(DWORD*)(*G::memory->GetPointer("CStateManager")) + 0x13378);
	CREATE_HOOK_ADDR(EndScene, *(DWORD*)(*(DWORD*)DX8::device + 35 * sizeof(uintptr_t)));
	CREATE_HOOK_ADDR(Reset, *(DWORD*)(*(DWORD*)DX8::device + 14 * sizeof(uintptr_t)));
	CREATE_HOOK(UpdateGame);
	CREATE_HOOK(RenderGame);
	//C-REATE_HOOK(RecvWhisperPacket); //no sig yet
	// 
	//CREATE_HOOK(__SendHack);		 //no sig yet	
	CREATE_HOOK(IncreaseExternalForce);
	CREATE_HOOK(BlockMovement);
	CREATE_HOOK(TestActorCollision);
	CREATE_HOOK(OnScriptEventStart);
	CREATE_HOOK(SendAttackPacket);

	CREATE_HOOK(UpdateKeyboard);
	CREATE_HOOK(ProcessTerrainCollision);
	CREATE_HOOK(Send);
	CREATE_HOOK(Recv);

	CREATE_HOOK(RecvDamageInfoPacket);
	CREATE_HOOK(__ShowMapName);
	
#ifdef DEV_MODE

	CREATE_HOOK(SendUseSkillPacket);
	
	CREATE_HOOK(SendOnClickPacket);
	CREATE_HOOK(SendScriptAnswerPacket);
	CREATE_HOOK(SendGiveItemPacket);
	CREATE_HOOK(SendShootPacket);
	CREATE_HOOK(SendTargetPacket);
	CREATE_HOOK(SendAddFlyTargetingPacket);
	CREATE_HOOK(SendFlyTargetingPacket);
	CREATE_HOOK(SendScriptButtonPacket);
	CREATE_HOOK(SendRefinePacket);
	CREATE_HOOK(SendSyncPositionPacket);
	CREATE_HOOK(SendCharacterStatePacket);
#endif

	err = MH_EnableHook(MH_ALL_HOOKS);
	if (err != MH_OK)
	{
		G::console->WriteError("MH_EnableHook() failed with error %s!\n", MH_StatusToString(err));
		F::should_exit = true;
	}

	timer.End();
	G::console->SetColor(Console::TextColor::YELLOW);
	G::console->WriteInfo("Created %d function hooks in %.1fs\n", created_hooks, timer.GetElapsed());
}

HookManager::~HookManager()
{
	SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(oWndproc));
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}





Console::Console()
{
	color = TextColor::WHITE;
	if (!AllocConsole())
	{
		this->WriteError("Console::%s() failed!\n", __func__);
		return;
	}
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	std::string title = "HyperBot MT2 | Build: ";
	title += __DATE__; title += " "; title += __TIME__;
	SetConsoleTitleA(title.c_str());
}

Console::~Console()
{
	fclose((FILE*)stdin);
	fclose((FILE*)stdout);
	if (!FreeConsole())
	{
		this->WriteError("Console::%s() failed!\n", __func__);
		return;
	}
	PostMessageW(GetConsoleWindow(), WM_CLOSE, 0, 0);
}

void Console::SetColor(TextColor color)
{
	this->color = color;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WORD(color));
}
