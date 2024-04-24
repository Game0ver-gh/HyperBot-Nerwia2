#include "pch.h"

HRESULT APIENTRY HookManager::EndScene(void* pDevice)
{
	auto result = HookManager::oEndScene(pDevice);

	static auto init_once = true;
	if (init_once)
	{
		init_once = false;

        DWORD currentThreadId = GetCurrentThreadId();
        HWND hwnd = NULL;

        HWND topWindow = GetTopWindow(NULL);
        while (topWindow != NULL)
        {
            DWORD windowThreadId = GetWindowThreadProcessId(topWindow, NULL);
            if (windowThreadId == currentThreadId && IsWindowVisible(topWindow))
            {
                hwnd = topWindow;
                break;
            }

            topWindow = GetNextWindow(topWindow, GW_HWNDNEXT);
        }
        HookManager::hWnd = topWindow;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(HookManager::hWnd);
		ImGui_ImplDX8_Init(DX8::device);

		ImGui::GetIO().IniFilename = nullptr;
		HookManager::oWndproc = (WNDPROC)SetWindowLongPtr(HookManager::hWnd, GWL_WNDPROC, (LONG)WndProc);

        TCHAR windir[MAX_PATH];
        GetWindowsDirectoryA(windir, MAX_PATH);
        auto fontDir = std::string(windir) + std::string("\\Fonts\\");
        std::string dir = std::string(fontDir.begin(), fontDir.end());

        auto GetRange = []() -> const ImWchar*
        {
            static const ImWchar ranges[] =
            {
                0x0020, 0x00FF,
                0x0100, 0x017F,
                0
            };

            return &ranges[0];
        };

        auto LoadFontFromMemory = [&](void* fontData, float size) -> ImFont*
        {
            auto& io = ImGui::GetIO();
            return io.Fonts->AddFontFromMemoryTTF(fontData, sizeof(fontData), size, nullptr, GetRange());
        };
        auto LoadFontFromFile = [&](const char* name, float size) -> ImFont*
        {
            auto& io = ImGui::GetIO();
            auto t = dir + std::string(name) + ".ttf";
            return io.Fonts->AddFontFromFileTTF(t.c_str(), size, nullptr, GetRange());
        };
        auto LoadTextureFrommemory = [&](BYTE* data, size_t size, float* out_width, float* out_height) -> PDIRECT3DTEXTURE8
        {
			PDIRECT3DTEXTURE8 texture = nullptr;
			D3DXCreateTextureFromFileInMemory(DX8::device, data, size, &texture);
			D3DSURFACE_DESC desc;
			texture->GetLevelDesc(0, &desc);
            if (out_width) *out_width = desc.Width;
            if (out_height) *out_height = desc.Height;
			return texture;
		};


        DX8::Font::digital_8 = LoadFontFromMemory(CompressedFonts::digitalFont, 8.f);
        DX8::Font::digital_10 = LoadFontFromMemory(CompressedFonts::digitalFont, 10.f);
        DX8::Font::digital_14 = LoadFontFromMemory(CompressedFonts::digitalFont, 14.f);
        DX8::Font::digital_18 = LoadFontFromMemory(CompressedFonts::digitalFont, 18.f);

        DX8::Font::verdana_8 = LoadFontFromFile("Verdana", 8.f);
        DX8::Font::verdana_10 = LoadFontFromFile("Verdana", 10.f);
        DX8::Font::verdana_14 = LoadFontFromFile("Verdana", 14.f);
        DX8::Font::verdana_18 = LoadFontFromFile("Verdana", 18.f);
        DX8::Font::verdana_32 = LoadFontFromFile("Verdana", 32.f);

        DX8::Icon::bg_black = LoadTextureFrommemory(CompressedIcons::bg_black, sizeof(CompressedIcons::bg_black), &DX8::Icon::bg_black_size.x, &DX8::Icon::bg_black_size.y);
        DX8::Icon::bg_blue = LoadTextureFrommemory(CompressedIcons::bg_blue, sizeof(CompressedIcons::bg_blue), &DX8::Icon::bg_blue_size.x, &DX8::Icon::bg_blue_size.y);
        DX8::Icon::bg_logo = LoadTextureFrommemory(CompressedIcons::bg_logo, sizeof(CompressedIcons::bg_logo), &DX8::Icon::bg_logo_size.x, &DX8::Icon::bg_logo_size.y);
        DX8::Icon::logo = LoadTextureFrommemory(CompressedIcons::logo, sizeof(CompressedIcons::logo), &DX8::Icon::logo_size.x, &DX8::Icon::logo_size.y);
        DX8::Icon::logo4 = LoadTextureFrommemory(CompressedIcons::logo4, sizeof(CompressedIcons::logo4), &DX8::Icon::logo4_size.x, &DX8::Icon::logo4_size.y);
        DX8::Icon::sword = LoadTextureFrommemory(CompressedIcons::sword, sizeof(CompressedIcons::sword), nullptr, nullptr);
        DX8::Icon::pickup = LoadTextureFrommemory(CompressedIcons::pickup, sizeof(CompressedIcons::pickup), nullptr, nullptr);
        DX8::Icon::exit = LoadTextureFrommemory(CompressedIcons::exit, sizeof(CompressedIcons::exit), nullptr, nullptr);
        DX8::Icon::dbg = LoadTextureFrommemory(CompressedIcons::dbg, sizeof(CompressedIcons::dbg), nullptr, nullptr);
        DX8::Icon::route = LoadTextureFrommemory(CompressedIcons::route, sizeof(CompressedIcons::route), nullptr, nullptr);
        DX8::Icon::players = LoadTextureFrommemory(CompressedIcons::players, sizeof(CompressedIcons::players), nullptr, nullptr);
        DX8::Icon::misc = LoadTextureFrommemory(CompressedIcons::misc, sizeof(CompressedIcons::misc), nullptr, nullptr);

        ImGui::GetIO().FontDefault = DX8::Font::verdana_14;

        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.7f, 0.7f, 0.7f, 1.f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
        //colors[ImGuiCol_FrameBgHovered] = ImVec4(0.08f, 0.08f, 0.08f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 0.67f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.30f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.30f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.7f, 0.7f, 0.7f, 1.f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.7f, 0.7f, 0.7f, 1.f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.7f, 0.7f, 0.7f, 1.f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.30f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

        ImVec4* _colors = ImGui::GetCustomSettings()->colors;
        auto alpha = ImGui::GetStyle().Alpha;
        _colors[ImGuiCustomCol_ChildBg] = ImVec4(0.11f, 0.11f, 0.11f, alpha);
        _colors[ImGuiCustomCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, alpha);
        _colors[ImGuiCustomCol_FrameEnabled] = ImVec4(0.86f, 0.86f, 0.86f, alpha);
        _colors[ImGuiCustomCol_FrameDisabled] = ImVec4(0.16f, 0.16f, 0.16f, alpha);
        _colors[ImGuiCustomCol_CheckboxBorderDisabled] = ImVec4(0.24f, 0.24f, 0.24f, alpha);
        _colors[ImGuiCustomCol_CheckboxBorderEnabled] = ImVec4(0.94f, 0.94f, 0.94f, alpha);
        _colors[ImGuiCustomCol_CheckboxThumbDisabled] = ImVec4(0.50f, 0.50f, 0.50f, alpha);
        _colors[ImGuiCustomCol_CheckboxThumbEnabled] = ImVec4(0.16f, 0.16f, 0.16f, alpha);
        _colors[ImGuiCustomCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, alpha);
        _colors[ImGuiCustomCol_TextDisabled] = ImVec4(1.00f, 1.00f, 1.00f, alpha);
        _colors[ImGuiCustomCol_TextActive] = ImVec4(1.00f, 1.00f, 1.00f, alpha);
        _colors[ImGuiCustomCol_SliderThumbSmall] = ImVec4(0.38f, 0.80f, 1.00f, alpha);
        _colors[ImGuiCustomCol_SliderThumbBig] = ImVec4(0.27f, 0.27f, 0.27f, alpha);
        _colors[ImGuiCustomCol_SliderThumbLarge] = ImVec4(0.27f, 0.27f, 0.27f, alpha);
        _colors[ImGuiCustomCol_RailBorderDisabled] = ImVec4(1.00f, 1.00f, 1.00f, alpha);
        _colors[ImGuiCustomCol_RailBorderEnabled] = ImVec4(0.38f, 0.80f, 1.00f, alpha);
	}

    //temp
    static bool init_config_files = true;
    if (init_config_files)
    {
        if (auto* local = G::game->GetLocalPlayer(); local)
        {
            init_config_files = false;
            G::cfg.CreateDump();
        }
    }

	ImGui_ImplDX8_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

    if ((GetAsyncKeyState(VK_INSERT) & 1) && GetActiveWindow() == HookManager::hWnd)
        DX8::is_gui_active = !DX8::is_gui_active;

    F::route_creator->OnRenderScreen();
    F::game_informer->OnRenderScreen();

	DX8::gui->Render();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX8_RenderDrawData(ImGui::GetDrawData());

	return result;
}

HRESULT APIENTRY HookManager::Reset(void* pThis, void* pPresentationParameters)
{
	ImGui_ImplDX8_InvalidateDeviceObjects();
	HRESULT result = HookManager::oReset(pThis, pPresentationParameters);
	ImGui_ImplDX8_CreateDeviceObjects();
	G::console->WriteError("Reset called with result: 0x%X\n", result);
	return result;
}


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall HookManager::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (DX8::is_gui_active || DX8::is_hovering_gui)
	{
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

		switch (uMsg)
		{
		case WM_MOUSEMOVE: return false;
		default:
			break;
		}

		return true;
	}
    return CallWindowProc(HookManager::oWndproc, hWnd, uMsg, wParam, lParam);
}