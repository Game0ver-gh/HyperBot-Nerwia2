#pragma once
using VID = DWORD;
using VNUM = int;
#include "Network.h"
#include "Entity.h"
#include "Background.h"

class GameMainInstance
{
public:
	explicit GameMainInstance();
	~GameMainInstance();

	using EntityMap =		std::map<VID, Entity*>;
	using ItemMap =			std::map<VNUM, ItemData*>;
	using ItemPair =		std::pair<VNUM, ItemData*>;
	using ItemVec =			std::vector<ItemPair>;
	using GroundItemMap =	std::map<VID, GroundItemData*>; 
	using EntityVec =		std::vector<Entity*>;
	using MobTableMap =		std::map<VNUM, TMobTable*>;

	//Returns map pointer of every item in the game (only valid when in game)
	ItemMap*				GetItemMapPtr();	

	//Returns map pointer of living entities inluding doors and other shit
	EntityMap*				GetEntityMapPtr();			

	//Returns map pointer of items lying on the ground
	GroundItemMap*			GetGroundItemMapPtr();				

	//Returns entity pointer under mouse cursor - nullptr otherwise
	Entity*					GetCursorEntity();				

	//Returns entity pointer from selected entity
	Entity*					GetSelectedEntity();

	//Returns entity pointer pointed by virtual ID (unique entity number in current entity map)
	Entity*					GetEntity(VID vid);

	//Returns entity pointer pointed by virtual number (unique entity number in game scale)
	Entity*					GetEntity(VNUM vnum);

	//Returns entity pointer pointed by type
	Entity*					GetEntity(EntityType type);		

	//Returns entity pointer pointed by display name
	Entity*					GetEntity(const std::string& name);	

	//Returns true when at least one entity of given type has been found
	bool					GetEntities(EntityVec& ent_vec_out, EntityType type);	

	//Returns ItemPair of item by given name
	ItemPair				GetItem(const std::string& name, bool skip_non_ascii = true);

	//Returns ItemPair of item by given virtual number
	ItemPair				GetItem(VNUM vnum);

	//Returns ItemVec of items by given name
	ItemVec					GetItems(const std::string& name, bool skip_non_ascii = true);

	//Returns local player pointer 
	Player*					GetLocalPlayer();					

	//Returns network stream pointer
	Network*				GetNetworkMgr();

	//Returns Background pointer
	Background*				GetBackground();

	//Returns MobTable pointer
	MobTableMap*			GetMobTableMapPtr();
	
	//Returns mob data pointer pointed by virtual number
	TMobTable*				GetNonPlayerData(VNUM vnum);

	//Returns mob data pointer pointed by name
	TMobTable*				GetNonPlayerData(const std::string& name);

private:
	DWORD* python_network_stream;
	DWORD* python_background;
	DWORD* python_char_mgr;
	DWORD* python_state_mgr;
	DWORD* python_item;
	DWORD* python_non_player;
	DWORD* item_manager;
};

