#ifndef PCH_H
#define PCH_H

#define GAME_NAME "Nerwia.exe"
#define MODULE_NAME "Nerwia.exe"
#define WINDOW_NAME "NERWIA"

#include "framework.h"
#pragma warning(disable : 26812)
#pragma warning(disable : 4244)
#pragma comment(lib, "libMinHook.x86.lib")

#include "SimpleClock.h"
#include "ini.h"
#include "easing.h"
#include "imgui.h"
#include "imgui_impl_DX8.h"
#include "imgui_impl_win32.h"
#include "resource.h"
#include "GUI.h"
#include "CompressedFonts.h"

#include "Screen.h"
#include "Memory.h"
#include "MinHook.h"
#include "GameMainInstance.h"
#include "PacketDefs.h"
#include "Backend.h"
#include "Math.h"
#include "ConfigManager.h"

#include "ICheatBase.h"
#include "GameInformer.h"
#include "WaitHack.h"
#include "PacketSniffer.h"
#include "AutoItemPickup.h"
#include "MiscHacks.h"
#include "RouteCreator.h"

namespace DX8
{
	//textures
	extern std::unique_ptr<HyperGUI> gui;
	extern std::unique_ptr<Screen> screen;
	//UI notifications

	extern IDirect3DDevice8* device;
	extern bool is_gui_active;
	extern bool is_hovering_gui;

	namespace Font
	{
		extern ImFont* digital_8;
		extern ImFont* digital_10;
		extern ImFont* digital_14;
		extern ImFont* digital_18;

		extern ImFont* verdana_8;
		extern ImFont* verdana_10;
		extern ImFont* verdana_14;
		extern ImFont* verdana_18;
		extern ImFont* verdana_32;
	}

	namespace Icon
	{
		extern PDIRECT3DTEXTURE8 bg_blue;
		extern PDIRECT3DTEXTURE8 bg_black;
		extern PDIRECT3DTEXTURE8 logo;
		extern PDIRECT3DTEXTURE8 logo4;
		extern PDIRECT3DTEXTURE8 bg_logo;
		extern PDIRECT3DTEXTURE8 sword;
		extern PDIRECT3DTEXTURE8 pickup;
		extern PDIRECT3DTEXTURE8 exit;
		extern PDIRECT3DTEXTURE8 dbg;
		extern PDIRECT3DTEXTURE8 route;
		extern PDIRECT3DTEXTURE8 players;
		extern PDIRECT3DTEXTURE8 misc;

		extern Vector2D bg_blue_size;
		extern Vector2D bg_black_size;
		extern Vector2D logo_size;
		extern Vector2D logo4_size;
		extern Vector2D bg_logo_size;
	}
}

namespace G
{
	extern std::unique_ptr<Memory> memory;
	extern std::unique_ptr<HookManager> hooks;
	extern std::unique_ptr<Console> console;
	extern ConfigManager cfg;
	extern std::unique_ptr<GameMainInstance> game;

	extern std::string game_location;
	extern HMODULE hModule;
}

namespace F
{
	extern std::unique_ptr<WaitHack> wait_hack;
	extern std::unique_ptr<PacketSniffer> packet_sniffer;
	extern std::unique_ptr<GameInformer> game_informer;
	extern std::unique_ptr<AutoItemPickup> auto_pickup;
	extern std::unique_ptr<MiscHacks> misc_hacks;
	extern std::unique_ptr<RouteCreator> route_creator;

	extern bool should_exit;
}

#endif //PCH_H
