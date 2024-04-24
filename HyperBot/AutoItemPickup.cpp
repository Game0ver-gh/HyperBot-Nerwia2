#include "pch.h"
#include "AutoItemPickup.h"


void AutoItemPickup::Init()
{
	LoadItemsWhitelistFile();
}

void AutoItemPickup::OnMenuFrame()
{
	ImGui::CustomCheckbox("Enable", &G::cfg[EBool::AP_ENABLED]);
	ImGui::CustomHotkey("Hotkey##AP", &G::cfg[EInt::AP_HOTKEY]);
	ImGui::Separator();
	{
		ImGui::CustomCheckbox("Range exploit", &G::cfg[EBool::AP_RANGE_EXPLOIT]);
		ImGui::CustomCheckbox("Disable on sight", &G::cfg[EBool::AP_ANTI_SPECTATE]);
		ImGui::CustomCheckbox("Check collision", &G::cfg[EBool::AP_CHECK_COLLISION]);
		ImGui::CustomCheckbox("Use whitelist", &G::cfg[EBool::AP_USE_WHITELIST]);
		ImGui::CustomSliderFloat("Range", &G::cfg[EFloat::AP_RANGE], 500.f, 5000.f, "%.0f");
		ImGui::CustomCheckbox("Draw range", &G::cfg[EBool::AP_DRAW_RANGE], &G::cfg[EColor::AP_RANGE]);
		ImGui::CustomCheckbox("Draw target", &G::cfg[EBool::AP_DRAW_CUR_TARGET], &G::cfg[EColor::AP_CUR_TARGET]);
	}

	ImGui::Separator();
	{
		if (ImGui::Button("Save whitelist"))
			WriteItemsWhitelistFile(); ImGui::SameLine();

		if (ImGui::Button("Reload whitelist"))
			LoadItemsWhitelistFile();

		static char search_phrase[32] = { 0 };
		
		ImGui::SetNextItemWidth(-1.f);
		ImGui::InputTextWithHint("Search:", "Search for item...", search_phrase, sizeof(search_phrase)); 

		ImGui::BeginChild("##whitelist", ImVec2((ImGui::GetWindowSize().x - ImGui::GetStyle().ItemInnerSpacing.x * 2.f) * 0.5f, 0), true);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

			std::vector<std::string> display;
			display.reserve(m_items_whitelist_display.size());
			for (const auto& name : m_items_whitelist_display)
			{
				if (name.find(search_phrase) != std::string::npos)
					display.emplace_back(name);
			}

			if (display.empty())
				ImGui::Text("No items found.");
			else
			{
				int item_to_remove_idx = -1;
				ImGuiListClipper clipper;
				clipper.Begin(display.size());
				while (clipper.Step())
				{
					for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
					{
						const auto& name = display[i];
						if (name.find(search_phrase) == std::string::npos)
							continue;
						
						if (ImGui::Selectable(name.c_str()))	
						{
							for (size_t i = 0; i < m_items_whitelist_display.size(); i++)
							{
								if (m_items_whitelist_display[i] == name)
								{
									item_to_remove_idx = i;
									break;
								}
							}
						}
					}
				}

				if (item_to_remove_idx > -1)
				{
					m_items_whitelist_display.erase(m_items_whitelist_display.begin() + item_to_remove_idx);
					std::sort(m_items_whitelist_display.begin(), m_items_whitelist_display.end());
				}
			}

			ImGui::PopStyleVar();
		}
		ImGui::EndChild(); ImGui::SameLine();

		ImGui::BeginChild("##item_list", ImVec2(0, 0), true);
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

			int found_items = m_items_display_filtered.size();
			std::vector<std::string> display;
			display.reserve(m_items_display_filtered.size());
			for (const auto& name : m_items_display_filtered)
			{
				if (name.find(search_phrase) != std::string::npos)
					display.emplace_back(name);
			}

			if (display.empty())
				ImGui::Text("No items found.");
			else
			{
				ImGuiListClipper clipper;
				clipper.Begin(display.size());
				while (clipper.Step())
				{
					for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
					{
						const auto& name = display[i];
						if (name.size() <= 1)
							continue;

						if (name.find(search_phrase) == std::string::npos)
							continue;

						if (ImGui::Selectable(name.c_str()))
						{
							bool is_unique = true;
							for (const auto& item : m_items_whitelist_display)
							{
								if (item == name)
								{
									is_unique = false;
									break;
								}
							}

							if (is_unique)
							{
								m_items_whitelist_display.push_back(name);
								std::sort(m_items_whitelist_display.begin(), m_items_whitelist_display.end());
							}
						}
					}
				}
			}
			
			ImGui::PopStyleVar();
		}
		ImGui::EndChild();
	}
}

void AutoItemPickup::PreIteration(Player* local)
{
	if (GetAsyncKeyState(G::cfg[EInt::AP_HOTKEY]) & 1)
		G::cfg[EBool::AP_ENABLED] = !G::cfg[EBool::AP_ENABLED];

	static auto attack_timer = Clock::GetCurTime();

	m_local = local;
	m_local->GetPixelPosition(&m_local_pos);

	m_ap_targets.clear();
	m_is_ranged = G::cfg[EBool::AP_RANGE_EXPLOIT];
	m_ap_distance = m_is_ranged ? G::cfg[EFloat::AP_RANGE] : 175.f;
	m_ap_speed = 200.f;
	m_ap_time = Clock::GetCurTime() - attack_timer;
	m_is_ap_ready = m_ap_time >= m_ap_speed;
	if (m_is_ap_ready)
		attack_timer = Clock::GetCurTime();
}

void AutoItemPickup::OnIteration(std::pair<VID, GroundItemData*>* item)
{
	if (!G::cfg[EBool::AP_ENABLED])
		return;

	if (!IsValid(item))
		return;

	AddNewTarget(item);

	if (!m_ap_targets.empty())
		PickUpTarget(m_ap_targets.back());
}

void AutoItemPickup::PostIteration()
{
	return;
}

void AutoItemPickup::OnRenderWorld()
{
	if (!G::cfg[EBool::AP_ENABLED])
		return;

	if (G::cfg[EBool::AP_DRAW_RANGE])
	{
		DX8::screen->SetDepth(false);
		DX8::screen->RenderCircle3D(m_local_pos, G::cfg[EFloat::AP_RANGE], 64, G::cfg[EColor::AP_RANGE_COVERED]);
		DX8::screen->SetDepth(true);
		DX8::screen->RenderCircle3D(m_local_pos, G::cfg[EFloat::AP_RANGE], 64, G::cfg[EColor::AP_RANGE]);
	}

	if (G::cfg[EBool::AP_DRAW_CUR_TARGET] && !m_ap_targets.empty())
	{
		for (const auto& target : m_ap_targets)
		{
			DX8::screen->SetDepth(false);
			DX8::screen->RenderLine3D(m_local_pos, target.m_position, G::cfg[EColor::AP_CUR_TARGET_COVERED]);
			DX8::screen->SetDepth(true);
			DX8::screen->RenderLine3D(m_local_pos, target.m_position, G::cfg[EColor::AP_CUR_TARGET]);

			DX8::screen->SetDepth(false);
			DX8::screen->RenderAABB3D(target.m_bound_box_min, target.m_bound_box_max, G::cfg[EColor::AP_CUR_TARGET_COVERED]);
			DX8::screen->SetDepth(true);
			DX8::screen->RenderAABB3D(target.m_bound_box_min, target.m_bound_box_max, G::cfg[EColor::AP_CUR_TARGET]);
		}
	}
}

void AutoItemPickup::OnRenderScreen()
{
	return;
}

bool AutoItemPickup::IsValid(std::pair<VID, GroundItemData*>* item)
{
	if (!item || !item->second || !item->second->CanBePickedUp())
		return false;

	if (G::cfg[EBool::AP_USE_WHITELIST])
	{
		if (m_items_whitelist.find(item->second->GetVirtualNumber()) == m_items_whitelist.end())
			return false;
	}

	return true;
}

bool AutoItemPickup::PickUpTarget(const Target& target)
{
	if (!m_is_ap_ready || (G::cfg[EBool::AP_ANTI_SPECTATE] && F::game_informer->IsLocalSpectated()))
		return false;

	auto* network = G::game->GetNetworkMgr();
	if (!network) return false;

	if (m_is_ranged)
	{
		return m_local->ShortBlinkTo(target.m_position, G::cfg[EBool::AP_CHECK_COLLISION], [&](const Vector& last_point) -> void
			{
				network->SendPickUpItemPacket(target.m_vid);
			});
	}
	else if (m_local->IsCloseToPosition(target.m_position, 150.f))
	{
		return network->SendPickUpItemPacket(target.m_vid);
	}
	else
	{
		m_local->MoveToDestPixelPosition(target.m_position);
		return false;
	}

	return false;
}

void AutoItemPickup::AddNewTarget(std::pair<VID, GroundItemData*>* item)
{
	Target target;
	target.m_target = item->second;
	target.m_position = target.m_target->GetPixelPosition();
	target.m_distance = m_local->GetDistanceTo(target.m_position);
	if (target.m_distance > m_ap_distance)
		return;

	target.m_vid = item->first;
	target.m_target->GetBoundBoxTransformed(&target.m_bound_box_min, &target.m_bound_box_max);
	m_ap_targets.emplace_back(target);
}

void AutoItemPickup::LoadItemsWhitelistFile()
{
	std::ifstream file(G::cfg.GetCheatDirectory() + G::cfg.GetFileName(ConfigManager::FILE_NAME_ITEM_WHITE_LIST));

	m_items_whitelist.clear();
	m_items_whitelist_display.clear();
	
	if (!file.is_open())
	{
		G::console->WriteError("Failed to open items whitelist file for read\n");
		return;
	}

	std::string line;
	while (std::getline(file, line))
	{
		if (line.empty() || line[0] == ';')
			continue;

		auto item_vec = G::game->GetItems(line);
		if (item_vec.empty())
		{
			G::console->WriteError("Invalid item name in whitelist!: %s\n", line.c_str());
			continue;
		}

		for (const auto& item : item_vec)
			m_items_whitelist.insert(item);
	}

	file.close();

	//Creating whitelist of unique item names for display purposes
	for (const auto& [vid, item_data] : m_items_whitelist)
	{
		if (!item_data || !item_data->GetName() || std::strlen(item_data->GetName()) <= 1)
			continue;

		if (m_items_whitelist_display.empty())
			m_items_whitelist_display.push_back(item_data->GetName());

		//add item name to m_items_whitelist_display only when not already in list
		bool is_unique = true;
		for (const auto& item : m_items_whitelist_display)
		{
			if (!item.compare(item_data->GetName())) //already in list
			{	
				is_unique = false;
				break;
			}
		}

		if (is_unique)
			m_items_whitelist_display.push_back(item_data->GetName());
	}
	std::sort(m_items_whitelist_display.begin(), m_items_whitelist_display.end());

	//Do it only once cuz of over 9k items capacity..
	if (m_items_display_filtered.empty())
	{
		//Creating list of unique item names for display purposes
		for (const auto& [vid, item_data] : *G::game->GetItemMapPtr())
		{
			if (!item_data || !item_data->GetName() || std::strlen(item_data->GetName()) <= 1)
				continue;

			if (m_items_display_filtered.empty())
				m_items_display_filtered.push_back(item_data->GetName());

			bool is_unique = true;
			for (auto& item : m_items_display_filtered)
			{
				if (!item.compare(item_data->GetName())) //already in list
				{
					is_unique = false;
					break;
				}
			}

			if (is_unique)
				m_items_display_filtered.push_back(item_data->GetName());
		}
		std::sort(m_items_display_filtered.begin(), m_items_display_filtered.end());
	}
}

void AutoItemPickup::WriteItemsWhitelistFile()
{
	std::ofstream file(G::cfg.GetCheatDirectory() + G::cfg.GetFileName(ConfigManager::FILE_NAME_ITEM_WHITE_LIST));
	if (!file.is_open())
	{
		G::console->WriteError("Failed to open items whitelist file to write\n");
		return;
	}
	
	file << ";Enter item names here to add them to the white list.\n";

	for (const auto& item : m_items_whitelist_display)
		file << item << '\n';
	
	file.close();

	//Update actual whitelist map
	m_items_whitelist.clear();

	for (const auto& item_name : m_items_whitelist_display)
	{
		auto item_vec = G::game->GetItems(item_name);

		for (const auto& item : item_vec)
			m_items_whitelist.insert(item);
	}
}

void AutoItemPickup::DumpItemsWhitelist()
{
	G::console->WriteInfo("Dumping items whitelist:\n");
	for (const auto& item : m_items_whitelist)
	{
		G::console->WriteInfo("Item[%d]: %s\n", item.first, item.second->GetName());
	}
}
