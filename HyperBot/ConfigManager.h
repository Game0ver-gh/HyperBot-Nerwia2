#pragma once
#undef max

enum class EBool
{
	WH_ENABLED,
	WH_RANGE_EXPLOIT,
	WH_KILL_STUNED,
	WH_SELECTED_TARGET_ONLY,
	WH_ANTI_SPECTATE,
	WH_CHECK_COLLISION, 
	WH_IGNORE_NORMAL_ATTACKS,
	WH_F_PLAYERS,
	WH_F_MOBS,
	WH_F_METINS,
	WH_F_BOSSES,
	WH_DRAW_RANGE,
	WH_DRAW_CUR_TARGET,

	AP_ENABLED,
	AP_RANGE_EXPLOIT,
	AP_USE_WHITELIST,
	AP_ANTI_SPECTATE,
	AP_CHECK_COLLISION,
	AP_DRAW_RANGE,
	AP_DRAW_CUR_TARGET,

	M_DISABLE_CAMERA_COLLISION,
	M_MOB_WALLHACK,
	M_WALLHACK,
	M_NO_FLY,
	M_NO_FOG,
	M_SB_ENABLED,
	M_SB_CHECK_COLLISION,
	
	GUI_SHOW_INFO_BOX,
	DBG_PRINT_PACKETS,
	
	COUNT
};

enum class EInt
{
	WH_HITS_PER_TICK_LIMIT,
	WH_HOTKEY,
	AP_HOTKEY,

	M_SB_HOTKEY,
	M_MAX_PACKET_PER_TICK,

	COUNT
};

enum class EFloat
{
	WH_RANGE,
	WH_ATTACK_SPEED,
	
	AP_RANGE,

	M_MAX_CAMERA_DISTANCE,
	M_SB_MULTIPLIER,
	
	COUNT
};

enum class EColor
{
	WH_RANGE,
	WH_RANGE_COVERED,
	WH_CUR_TARGET,
	WH_CUR_TARGET_COVERED,

	AP_RANGE,
	AP_RANGE_COVERED,
	AP_CUR_TARGET,
	AP_CUR_TARGET_COVERED,

	COUNT
};

class ConfigManager
{
	std::array<bool, (size_t)EBool::COUNT>		m_bools{};
	std::array<int, (size_t)EInt::COUNT>		m_ints{};
	std::array<float, (size_t)EFloat::COUNT>	m_floats{};
	std::array<Color, (size_t)EColor::COUNT>	m_colors{};

	mINI::INIStructure m_ini_file;
	
	void CreateMobsDump();
	void CreateItemsDump();
	void LoadGameDirectory();
	double GetKeyValue(const std::pair<std::string, std::string>& key_val);
	bool IsValidValue(const std::string& val);
	bool GenerateConfigFile();
	
public:
	ConfigManager();
	void CreateDump();
	const std::string& GetCheatDirectory() const;
	const std::string& GetFileName(int EFileNameIdx) const;

	enum
	{
		FILE_NAME_MOBS_DUMP,
		FILE_NAME_ITEMS_DUMP,
		FILE_NAME_CONFIG,
		FILE_NAME_ITEM_WHITE_LIST,
		FILE_NAME_COUNT
	};
	
	//Reads config file to memory
	void LoadConfigFromFile();

	//Write config file from memory
	void WriteConfigToFile();

	//Regenerates config file
	void RegenerateConfigFile();
	
	bool& operator[](EBool index) { return m_bools[(int)index]; }
	int& operator[](EInt index) { return m_ints[(int)index]; }
	float& operator[](EFloat index) { return m_floats[(int)index]; }
	Color& operator[](EColor index) { return m_colors[(int)index]; }

private:
	std::string m_cheat_directory;
	std::string m_file_names[FILE_NAME_COUNT] =
	{
		"mobs_dump.txt",
		"items_dump.txt",
		"config.ini",
		"item_white_list.txt",
	};
};