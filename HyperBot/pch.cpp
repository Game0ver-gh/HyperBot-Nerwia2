#include "pch.h"

namespace DX8
{
	//device
	//textures
	//fonts
	std::unique_ptr<HyperGUI> gui;
	//UI notifications
	std::unique_ptr<Screen> screen = std::make_unique<Screen>();

	IDirect3DDevice8* device = nullptr;
	bool is_gui_active = false;
	bool is_hovering_gui = false;

	namespace Font
	{
		ImFont* digital_8 = nullptr;
		ImFont* digital_10 = nullptr;
		ImFont* digital_14 = nullptr;
		ImFont* digital_18 = nullptr;

		ImFont* verdana_8 = nullptr;
		ImFont* verdana_10 = nullptr;
		ImFont* verdana_14 = nullptr;
		ImFont* verdana_18 = nullptr;
		ImFont* verdana_32 = nullptr;
	}

	namespace Icon
	{
		PDIRECT3DTEXTURE8 bg_blue = nullptr;
		PDIRECT3DTEXTURE8 bg_black = nullptr;
		PDIRECT3DTEXTURE8 logo = nullptr;
		PDIRECT3DTEXTURE8 logo4 = nullptr;
		PDIRECT3DTEXTURE8 bg_logo = nullptr;
		PDIRECT3DTEXTURE8 sword = nullptr;
		PDIRECT3DTEXTURE8 pickup = nullptr;
		PDIRECT3DTEXTURE8 exit = nullptr;
		PDIRECT3DTEXTURE8 dbg = nullptr;
		PDIRECT3DTEXTURE8 route = nullptr;
		PDIRECT3DTEXTURE8 players = nullptr;
		PDIRECT3DTEXTURE8 misc = nullptr;

		Vector2D bg_blue_size{};
		Vector2D bg_black_size{};
		Vector2D logo_size{};
		Vector2D logo4_size{};
		Vector2D bg_logo_size{};
	}
}
 
namespace G
{
	std::unique_ptr<Memory> memory;
	std::unique_ptr<HookManager> hooks;
	std::unique_ptr<Console> console = std::make_unique<Console>();
	ConfigManager cfg;
	std::unique_ptr<GameMainInstance> game;

	std::string game_location;
	HMODULE hModule = NULL;
}

namespace F
{
	std::unique_ptr<WaitHack> wait_hack = std::make_unique<WaitHack>();
	std::unique_ptr<PacketSniffer> packet_sniffer = std::make_unique<PacketSniffer>();
	std::unique_ptr<GameInformer> game_informer = std::make_unique<GameInformer>();
	std::unique_ptr<AutoItemPickup> auto_pickup = std::make_unique<AutoItemPickup>();
	std::unique_ptr<MiscHacks> misc_hacks = std::make_unique<MiscHacks>();
	std::unique_ptr<RouteCreator> route_creator = std::make_unique<RouteCreator>();

	bool should_exit = false;
}