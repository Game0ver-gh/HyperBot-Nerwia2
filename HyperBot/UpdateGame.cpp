#include "pch.h"

void InitOnAlive();

void __fastcall HookManager::UpdateGame(void* pThis, void* edx)
{
	HookManager::oUpdateGame(pThis);

	auto network = G::game->GetNetworkMgr();
	if (!network) return;

	network->UpdatePacketSent();
	
	if (network->IsConnected())
	{
		auto local = G::game->GetLocalPlayer();
		if (local && local->IsAlive())
		{
			InitOnAlive();

			//Pre iterations
			F::game_informer->PreIteration(local);
			F::wait_hack->PreIteration(local);
			F::auto_pickup->PreIteration(local);
			F::misc_hacks->PreIteration(local);
			F::route_creator->PreIteration(local);

			Vector local_pos; local->GetPixelPosition(&local_pos);
			
			//Entity map operations
			if (const auto* ent_map = G::game->GetEntityMapPtr(); ent_map && !ent_map->empty())
			{
				//Creating and sorting target list by distance to local player
				std::vector<Entity*> entities;
#ifndef _DEBUG 
				entities.reserve(ent_map->size());
#endif
				
				//Creating and sorting entity vector
				for (const auto& [vid, ent] : *ent_map)
				{
					if (ent && ent->IsAlive())
						entities.emplace_back(ent);
				}
				std::sort(entities.begin(), entities.end(), [&local_pos](Entity* a, Entity* b) -> bool
					{
						return a->GetDistanceTo(local_pos) > b->GetDistanceTo(local_pos);
					});

				//Iterating through sorted target list
				for (auto& entity : entities)
				{
					//On iterations
					F::wait_hack->OnIteration(entity);
					F::misc_hacks->OnIteration(entity);
				}
			}
			
			//Ground item map operations
			if (const auto* item_map = G::game->GetGroundItemMapPtr(); item_map && !item_map->empty())
			{
				//Creating and sorting ground items list by distance to local player
				std::vector<std::pair<VID, GroundItemData*>> items;
#ifndef _DEBUG
				items.reserve(item_map->size());
#endif
				
				for (const auto& [vid, item] : *item_map)
					if (item) items.emplace_back(vid, item);
				
				std::sort(items.begin(), items.end(), [&local_pos](std::pair<VID, GroundItemData*>& a, std::pair<VID, GroundItemData*>& b) -> bool
					{
						return a.second->GetDistanceTo(local_pos) > b.second->GetDistanceTo(local_pos);
					});

				//Iterating through sorted ground items list
				for (auto& item : items)
				{
					//On iterations
					F::auto_pickup->OnIteration(&item);
				}
			}

			//Post iterations
			F::game_informer->PostIteration();
			F::wait_hack->PostIteration();
			F::auto_pickup->PostIteration();
			F::misc_hacks->PostIteration();
			F::route_creator->PostIteration();
		}
	}
}

void InitOnAlive()
{
	static bool init = true;
	if (!init) return;
	init = false;

	F::wait_hack->Init();
	F::route_creator->Init();
}