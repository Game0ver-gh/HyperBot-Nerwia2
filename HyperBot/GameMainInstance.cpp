#include "pch.h"
#include "GameMainInstance.h"

GameMainInstance::GameMainInstance()
{
	python_network_stream = G::memory->GetPointer("CPythonNetworkStream");
	python_char_mgr = G::memory->GetPointer("CPythonCharManager");
	python_state_mgr = G::memory->GetPointer("CPythonStateManager");
	item_manager = G::memory->GetPointer("CItemManager");
	python_item = G::memory->GetPointer("CPythonItem");
	python_background = G::memory->GetPointer("CPythonBackground");
	python_non_player = G::memory->GetPointer("CPythonNonPlayer");
}

GameMainInstance::~GameMainInstance()
{
	python_network_stream = nullptr;
	python_char_mgr = nullptr;
	python_state_mgr = nullptr;
	item_manager = nullptr;
	python_item = nullptr;
	python_background = nullptr;
	python_non_player = nullptr;
}

Entity* GameMainInstance::GetCursorEntity()
{
	if (!python_char_mgr) return nullptr;
	return *reinterpret_cast<Entity**>(DWORD(*python_char_mgr) + 0x10);
}

Entity* GameMainInstance::GetSelectedEntity()
{
	if (!python_network_stream) return nullptr;
	return *(Entity**)((DWORD)python_network_stream + 0xD8);
}

Entity* GameMainInstance::GetEntity(VID vid)
{
	const auto* pMap = GetEntityMapPtr();
	if (!pMap) return nullptr;

#ifdef _DEBUG
	for (const auto& it : *pMap)
	{
		if (vid == it.first)
			return it.second;
	}
	return nullptr;
#endif

	const auto it = pMap->find(vid);
	if (it != pMap->end())
		return it->second;

	return nullptr;
}

Entity* GameMainInstance::GetEntity(VNUM vnum)
{
	const auto* pMap = GetEntityMapPtr();
	if (!pMap) return nullptr;

	for (const auto& it : *pMap)
	{
		if (it.second && it.second->GetVirtualNumber() == vnum)
			return it.second;
	}

	return nullptr;
}

Entity* GameMainInstance::GetEntity(EntityType type)
{
	const auto* pMap = GetEntityMapPtr();
	if (!pMap) return nullptr;

	for (const auto& it : *pMap)
	{
		if (it.second && it.second->GetType() == type)
			return it.second;
	}

	return nullptr;
}

Entity* GameMainInstance::GetEntity(const std::string& name)
{
	const auto* pMap = GetEntityMapPtr();
	if (!pMap) return nullptr;

	for (const auto& it : *pMap)
	{
		if (it.second && !name.compare(it.second->GetName()))
			return it.second;
	}

	return nullptr;
}

bool GameMainInstance::GetEntities(EntityVec& ent_vec_out, EntityType type)
{
	auto* map = GetEntityMapPtr();
	if (map)
	{
		for (const auto& p : *map)
		{
			if (p.second && p.second->GetType() == type)
				ent_vec_out.push_back(p.second);
		}
	}
	return !ent_vec_out.empty();
}

GameMainInstance::ItemPair GameMainInstance::GetItem(const std::string& name, bool skip_non_ascii)
{
	auto* map = GetItemMapPtr();
	if (map)
	{
		if (skip_non_ascii)
		{
			std::string ascii_name{};
			for (const auto& c : name)
			{
				if (c > 0 && c < 127)
					ascii_name.push_back(c);
			}

			for (const auto& p : *map)
			{
				if (!p.second)
					continue;

				std::string ascii_map{};
				const char* str = p.second->GetName();
				size_t len = strlen(str);
				for (size_t i = 0; i < len; i++)
				{
					if (str[i] > 0 && str[i] < 127)
						ascii_map.push_back(str[i]);
				}

				if (p.second && !ascii_name.compare(ascii_map))
					return std::make_pair(p.first, p.second);
			}
		}
		else
		{
			for (const auto& p : *map)
			{
				
				if (p.second && !name.compare(p.second->GetName()))
					return std::make_pair(p.first, p.second);
			}
		}
	}
	return std::make_pair(NULL, nullptr);
}

GameMainInstance::ItemPair GameMainInstance::GetItem(VNUM vnum)
{
	auto* map = GetItemMapPtr();
	if (map)
	{
		auto item = map->find(vnum);
		if (item != map->end())
			return std::make_pair(vnum, map->at(vnum));
	}
	return { 0, nullptr };
}

GameMainInstance::ItemVec GameMainInstance::GetItems(const std::string& name, bool skip_non_ascii)
{
	auto* map = GetItemMapPtr();
	ItemVec vec;
	if (map)
	{
		if (skip_non_ascii)
		{
			std::string ascii_name{};
			for (const auto& c : name)
			{
				if (c > 0 && c < 127)
					ascii_name.push_back(c);
			}

			for (const auto& p : *map)
			{
				if (!p.second)
					continue;

				std::string ascii_map{};
				const char* str = p.second->GetName();
				size_t len = strlen(str);
				for (size_t i = 0; i < len; i++)
				{
					if (str[i] > 0 && str[i] < 127)
						ascii_map.push_back(str[i]);
				}

				if (p.second && !ascii_name.compare(ascii_map))
					vec.push_back(std::make_pair(p.first, p.second));
			}
			return vec;
		}
		else
		{
			for (const auto& p : *map)
			{
				if (p.second && !name.compare(p.second->GetName()))
					vec.push_back(std::make_pair(p.first, p.second));
			}
			return vec;
		}
	}
	return {};
}

Player* GameMainInstance::GetLocalPlayer()
{
	if (!python_char_mgr) return nullptr;
	return *reinterpret_cast<Player**>(DWORD(*python_char_mgr) + 0xC);
}

Network* GameMainInstance::GetNetworkMgr()
{
	if (!python_network_stream) return nullptr;
	return *reinterpret_cast<Network**>(python_network_stream);
}

Background* GameMainInstance::GetBackground()
{
	if (!python_background) return nullptr;
	return *reinterpret_cast<Background**>(python_background);
}

TMobTable* GameMainInstance::GetNonPlayerData(VNUM vnum)
{
	auto* map = GetMobTableMapPtr();
	if (map)
	{
#ifdef _DEBUG
		for (const auto& p : *map)
		{
			if (p.first == vnum)
				return p.second;
		}
		return nullptr;
#endif
		auto found = map->find(vnum);
		if (found != map->end())
			return found->second;
	}
	
	return nullptr;
}

TMobTable* GameMainInstance::GetNonPlayerData(const std::string& name)
{
	auto* map = GetMobTableMapPtr();
	if (map)
	{
		for (const auto& p : *map)
		{
			if (p.second && !name.compare(p.second->szLocaleName))
				return p.second;
		}
	}

	return nullptr;
}

GameMainInstance::ItemMap* GameMainInstance::GetItemMapPtr()
{
#ifdef _DEBUG
	if (auto itemPtr1 = *(DWORD*)(DWORD(*item_manager) + 0x1C); itemPtr1) //CItemManager
		if (auto itemPtr2 = *(DWORD*)(itemPtr1 + 0x4); itemPtr2)
			return (ItemMap*)itemPtr2;
	return nullptr;
#endif
	if (auto itemPtr1 = *reinterpret_cast<DWORD*>(DWORD(*item_manager) + 0x1C); itemPtr1)
		return reinterpret_cast<ItemMap*>(itemPtr1 + 0x4);
	return nullptr;
}

GameMainInstance::EntityMap* GameMainInstance::GetEntityMapPtr()
{
#ifdef _DEBUG
	if (auto entityPtr1 = *(DWORD*)(DWORD(*python_char_mgr) + 0x20); entityPtr1)
		if (auto entityPtr2 = *(DWORD*)(entityPtr1 + 0x4); entityPtr2)
			return (EntityMap*)(entityPtr2);

	return nullptr;
#endif
	return reinterpret_cast<EntityMap*>(DWORD(*python_char_mgr) + 0x20);
}

GameMainInstance::GroundItemMap* GameMainInstance::GetGroundItemMapPtr()
{
#ifdef _DEBUG
	return (GroundItemMap*)(*reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>((*python_item) + 0x4) + 0x4));
#endif
	return reinterpret_cast<GroundItemMap*>(DWORD(*python_item) + 0x4);
}

GameMainInstance::MobTableMap* GameMainInstance::GetMobTableMapPtr()
{
#ifdef _DEBUG
	if (auto entityPtr1 = *(DWORD*)(DWORD(*python_non_player) + 0x4); entityPtr1)
		if (auto entityPtr2 = *(DWORD*)(entityPtr1 + 0x4); entityPtr2)
			return (MobTableMap*)(entityPtr2);

	return nullptr;
#endif
	return reinterpret_cast<MobTableMap*>(DWORD(*python_non_player) + 0x4);
}