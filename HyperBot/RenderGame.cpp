#include "pch.h"

//Metin2 is singlethreaded game, so we can just run everythin in one function hook
//Game's call order: UpdateGame -> RenderGame -> EndScene

void __fastcall HookManager::RenderGame(void* pThis, void* edx)
{
	DX8::screen->UpdateWorldState();
	HookManager::oRenderGame(pThis);
	
	F::game_informer->OnRenderWorld();
	F::auto_pickup->OnRenderWorld();
	F::wait_hack->OnRenderWorld();
	F::route_creator->OnRenderWorld();
	F::misc_hacks->OnRenderWorld();
	
	DX8::screen->StartRender();
	{	
		DX8::screen->RenderDrawList();
	}
	DX8::screen->EndRender();

	static bool init = true;
	if (init)
	{
		init = false;
		F::auto_pickup->Init();
		F::game_informer->Init();
		F::misc_hacks->Init();
	}
}