#include "pch.h"


void __fastcall HookManager::UpdateKeyboard(void* pThis, void* edx)
{
    if (DX8::is_gui_active || DX8::is_hovering_gui)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureKeyboard || io.WantTextInput)
            return;
    }
    return HookManager::oUpdateKeyboard(pThis);
}