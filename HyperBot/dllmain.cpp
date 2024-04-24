#include "pch.h"

VOID APIENTRY MainThread()
{
    G::memory = std::make_unique<Memory>();
    G::game = std::make_unique<GameMainInstance>();
    DX8::gui = std::make_unique<HyperGUI>();
    G::hooks = std::make_unique<HookManager>();
    
    while (true)
    {
        if (GetAsyncKeyState(VK_RCONTROL) || F::should_exit) break;
        Sleep(100);
    }

    G::hooks->~HookManager();

    Sleep(500);

    FreeLibraryAndExitThread(G::hModule, EXIT_SUCCESS);
}
 
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved )
{
    DisableThreadLibraryCalls(hModule);
    G::hModule = hModule;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), NULL, NULL, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

