#include "pch.h"
#include "ConfigManager.h"
#define SET_COLOR(EColor, val) m_colors[(size_t)EColor] = val
#define SET_BOOL(EBool, val) m_bools[(size_t)EBool] = val
#define SET_INT(EInt, val) m_ints[(size_t)EInt] = val
#define SET_FLOAT(EFloat, val) m_floats[(size_t)EFloat] = val

ConfigManager::ConfigManager()
{
	m_bools.fill(false);
	m_floats.fill(0.0f);
	m_ints.fill(0);
	m_colors.fill(Color::White());
	
	LoadGameDirectory();

	std::ifstream items_whitelist_file(m_cheat_directory + m_file_names[FILE_NAME_ITEM_WHITE_LIST]);
	if (!items_whitelist_file.good())
	{
		std::ofstream file(m_cheat_directory + m_file_names[FILE_NAME_ITEM_WHITE_LIST]);
		file << ";Enter item names here to add them to the white list.\n";
		file.close();
		G::console->WriteDebug("Default item white list file created in %s\n", (m_cheat_directory + m_file_names[FILE_NAME_ITEM_WHITE_LIST]).c_str());
	}

	std::ifstream config_file(m_cheat_directory + m_file_names[FILE_NAME_CONFIG]);
	if (!config_file.good())
	{
		if (GenerateConfigFile())
		{
			G::console->WriteDebug("Default config file created in %s\n", (m_cheat_directory + m_file_names[FILE_NAME_CONFIG]).c_str());
		}
	}

	LoadConfigFromFile();
}

void ConfigManager::CreateDump()
{
	std::ifstream mobs_dump_file(m_cheat_directory + m_file_names[FILE_NAME_MOBS_DUMP]);
	if (!mobs_dump_file.good())
		CreateMobsDump();
	
	std::ifstream items_dump_file(m_cheat_directory + m_file_names[FILE_NAME_ITEMS_DUMP]);
	if (!items_dump_file.good())
		CreateItemsDump();
}

const std::string& ConfigManager::GetCheatDirectory() const
{
	return m_cheat_directory;
}

const std::string& ConfigManager::GetFileName(int EFileNameIdx) const
{
	return m_file_names[EFileNameIdx];
}

void ConfigManager::WriteConfigToFile()
{
	mINI::INIFile config(m_cheat_directory + m_file_names[FILE_NAME_CONFIG]);
	
	size_t bool_iter = 0, int_iter = 0, float_iter = 0, color_iter = 0;
	for (const auto& [section, collection] : m_ini_file)
	{
		for (const auto& [key, value] : collection)
		{
			if (key[0] == 'b')
				m_ini_file[section][key] = std::to_string(m_bools[bool_iter++]);
			else if (key[0] == 'i')
				m_ini_file[section][key] = std::to_string(m_ints[int_iter++]);
			else if (key[0] == 'f')
				m_ini_file[section][key] = std::to_string(m_floats[float_iter++]);
			else if (key[0] == 'c')
			{
				const auto& color = m_colors[color_iter++];
				std::string build_value = "0x";
				for (int i = 3; i >= 0; i--)
				{
					char hex[3]{};
					if ((int)color[i] == 0)
						sprintf_s(hex, "00");
					else
						sprintf_s(hex, "%02X", (int)color[i]);
					build_value += hex;
				}
				m_ini_file[section][key] = build_value;
			}
		}
	}
	
	config.write(m_ini_file);
}

void ConfigManager::RegenerateConfigFile()
{
	if (std::filesystem::remove(m_cheat_directory + m_file_names[FILE_NAME_CONFIG]))
	{
		GenerateConfigFile();
		LoadConfigFromFile();
	}
}

double ConfigManager::GetKeyValue(const std::pair<std::string, std::string>& key_val)
{
	if (key_val.first[0] == 'b')
	{
		if (key_val.second == "true")
			return 1.;
		else if (key_val.second == "false")
			return 0.;
		else
			return static_cast<double>(std::stoi(key_val.second));
	}
	else if (key_val.first[0] == 'i')
	{
		return static_cast<double>(std::stoi(key_val.second));
	}
	else if (key_val.first[0] == 'f')
	{
		return std::stod(key_val.second);
	}
	else if (key_val.first[0] == 'c')
	{
		ImVec4 col_val{};
		for (size_t col = 2, idx = 0; col < key_val.second.size(); col += 2, idx++)
		{
			std::string sub = key_val.second.substr(col, 2);
			col_val[idx] = ((int)std::stoul(sub, nullptr, 16)) / 255.f;
		}
		return ImGui::ColorConvertFloat4ToU32(col_val);
	}

	return std::numeric_limits<double>::infinity();
}

bool ConfigManager::IsValidValue(const std::string& val)
{
	return std::all_of(val.begin(), val.end(), [](unsigned char c) 
		{
			return std::isdigit(c) || std::isalpha(c) || c == '.' || c == '-';
		});
}

bool ConfigManager::GenerateConfigFile()
{
	mINI::INIFile config(m_cheat_directory + m_file_names[FILE_NAME_CONFIG]);

	m_ini_file["WaitHack"].set(
		{
			{ "bEnabled", "false" },
			{ "bRangeExploit", "false" },
			{ "bKillStuned", "false" },
			{ "bSelectedTargetOnly", "true" },
			{ "bAntiSpectate", "true" },
			{ "bCheckCollision", "true" },
			{ "bIgnoreNormalAttacks", "true" },
			{ "bFilterPlayers", "false" },
			{ "bFilterMobs", "false" },
			{ "bFilterMetins", "false" },
			{ "bFilterBosses", "false" },
			{ "iMaxHitsPerTick", "35" },
			{ "iEnableHotkey", "20"}, //VK_CAPITAL (Caps Lock)
			{ "fRange", "1500.0" },
			{ "fAttackSpeed", "280.0" },
			{ "bDrawRange", "true" },
			{ "bDrawCurrentTarget", "true" },
			{ "cRangeColor", "0x00FF00FF" },
			{ "cRangeCoveredColor", "0xFF0000FF" },
			{ "cCurrentTargetColor", "0xFF7878FF" },
			{ "cCurrentTargetCoveredColor", "0x7878FFFF" }
		});

	m_ini_file["AutoPickup"].set(
		{
			{ "bEnabled", "false" },
			{ "bRangeExploit", "true" },
			{ "bUseWhitelist", "true" },
			{ "bAntiSpectate", "true" },
			{ "bCheckCollision", "true" },
			{ "bDrawRange", "true" },
			{ "bDrawCurrentTarget", "true" },
			{ "iEnableHotkey", "-1"}, //None (...)
			{ "fRange", "3200.0" },
			{ "cRangeColor", "0xFF00FFFF" },
			{ "cRangeCoveredColor", "0xFF9933FF" },
			{ "cCurrentTargetColor", "0xFFFFFFFF" },
			{ "cCurrentTargetCoveredColor", "0x9966FFFF" }
		});

	m_ini_file["Misc"].set(
		{
			{ "bDisableCameraCollision", "true" },
			{ "fMaxCameraDistance", "50000.0" },
			{ "bMobWallhack", "true" },
			{ "bWallhack", "false" },
			{ "bNoFly", "false" },
			{ "bNoFog", "true" },
			{ "bSpeedBoost", "true" },
			{ "bSpeedBoostCheckCollision", "true" },
			{ "iEnableHotkey", "160"}, //VK_LSHIFHT (LEFT SHIFT)
			{ "iMaxPacketPerTick", "50" },
			{ "fSpeedBoostMultiplier", "110.0" }
		});

	m_ini_file["Gui"].set(
		{
			{ "bShowInfoBox", "true" }
		});

	m_ini_file["Debug"].set(
		{
			{ "bPrintRawPackets", "false" }
		});

	return config.generate(m_ini_file, true);
}

void ConfigManager::LoadConfigFromFile()
{
	if (m_ini_file.size() == 0)
	{
		mINI::INIFile config(m_cheat_directory + m_file_names[FILE_NAME_CONFIG]);
		config.read(m_ini_file);
	}

	size_t bool_iter = 0, int_iter = 0, float_iter = 0, color_iter = 0;
	for (const auto& [section, collection] : m_ini_file)
	{
		for (const auto& [key, value] : collection)
		{
			if (!IsValidValue(value))
				continue;

			auto v = GetKeyValue(std::make_pair(key, value));
			if (!std::isinf(v))
			{
				if (key[0] == 'b')
					m_bools[bool_iter++] = static_cast<bool>(v);
				else if (key[0] == 'i')
					m_ints[int_iter++] = static_cast<int>(v);
				else if (key[0] == 'f')
					m_floats[float_iter++] = static_cast<float>(v);
				else if (key[0] == 'c')
					m_colors[color_iter++] = static_cast<unsigned int>(v);
			}
			else 
				G::console->WriteError("Incorrect config value for key!: %s : %.1f\n", key.c_str(), float(v));
		}
	}

	size_t read_size = bool_iter + int_iter + float_iter + color_iter;
	size_t total_size = (int)EBool::COUNT + (int)EInt::COUNT + (int)EFloat::COUNT + (int)EColor::COUNT;
	if (read_size < total_size)
	{
		G::console->WriteError("Incorrect config size!: %d/%d\n", read_size, total_size);
		//That means that the config file is either corrupted or outdated
		//So we will just generate only missing records
		//GenerateConfig(); TODO
	}
}

void ConfigManager::CreateMobsDump()
{
	auto* map = G::game->GetMobTableMapPtr();
	if (!map) return;

	std::string path(G::game_location);
	path.append("HyperBot\\").append(m_file_names[FILE_NAME_MOBS_DUMP]);

	std::ofstream file(path);
	for (const auto& mob : *map)
	{
		if (!mob.second) continue;

		file <<
			"[ " << mob.first << " ]" <<
			"\tName: " << mob.second->szLocaleName <<
			"\tRank: " << (int)mob.second->bRank <<
			"\tLvl: " << (int)mob.second->bLevel <<
			"\tType: " << (int)mob.second->bType <<
			"\tGold range: " << (int)mob.second->dwGoldMin << " - " << (int)mob.second->dwGoldMax <<
			"\tExp: " << (int)mob.second->dwExp <<
			"\tHP: " << (int)mob.second->dwMaxHP <<
			"\tEmpire: " << (int)mob.second->bEmpire << std::endl;
	}
	file << "\nFound " << map->size() << " mobs." << std::endl;
	file.close();
	G::console->WriteDebug("Mobs[%d] dump created in: %s\n", map->size(), path.c_str());
}

void ConfigManager::CreateItemsDump()
{
	auto* map = G::game->GetItemMapPtr();
	if (!map) return;

	std::string path(G::game_location);
	path.append("HyperBot\\").append(m_file_names[FILE_NAME_ITEMS_DUMP]);

	std::ofstream file(path);
	for (const auto& item : *map)
	{
		if (!item.second) continue;

		file <<
			"[ " << item.first << " ]" <<
			"\tName: " << item.second->GetName() <<
			"\tType: " << (int)item.second->GetType() <<
			"\tSubType: " << (int)item.second->GetSubType() <<
			"\tLevel: " << item.second->GetRefineLevel() << std::endl;
	}
	file << "\nFound " << map->size() << " items." << std::endl;
	file.close();
	G::console->WriteDebug("Items[%d] dump created in: %s\n", map->size(), path.c_str());
}

void ConfigManager::LoadGameDirectory()
{
	HMODULE hModuleMain = GetModuleHandle(nullptr);
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(hModuleMain, szPath, MAX_PATH);

	std::string strPath(szPath);
	std::string strDirectory, strFileName;

	size_t nLastBackslash = strPath.find_last_of('\\');

	if (nLastBackslash != std::string::npos)
	{
		strFileName = strPath.substr(nLastBackslash + 1);
		strDirectory = strPath.substr(0, nLastBackslash);
	}
	else
	{
		strDirectory = strPath;
	}
	strDirectory += '\\';
	G::game_location = strDirectory;

	m_cheat_directory = G::game_location + "HyperBot";
	if (!std::filesystem::exists(m_cheat_directory))
	{
		try
		{
			std::filesystem::create_directory(m_cheat_directory);
		}
		catch (const std::exception& e)
		{
			G::console->WriteError("Failed to create cheat directory! Error: %s\n", e.what());
			return;
		}
	}

	m_cheat_directory += "\\";
}