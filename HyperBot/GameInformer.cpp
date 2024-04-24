#include "pch.h"
#include "GameInformer.h"

void GameInformer::Init()
{
	return;
}

void GameInformer::OnMenuFrame()
{
	static ImGuiTableFlags flags =
		ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
		| ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
		| ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
		| ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
		| ImGuiTableFlags_SizingFixedFit;

	if (ImGui::BeginTable("Players meet##GI", 5, flags))
	{
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 100.f);
		ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 50.f);
		ImGui::TableSetupColumn("GM", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 50.f);
		ImGui::TableSetupColumn("Empire", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 50.f);
		ImGui::TableSetupColumn("Race", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 50.f);

		ImGui::TableHeadersRow();

		for (const auto& it : m_all_meet_players)
		{
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%s", it.m_name.c_str());
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%d", it.m_level);
			ImGui::TableSetColumnIndex(2);
			if (it.m_is_gm)
				ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(Color::FireRed()), "%s", it.m_is_gm ? "GM" : " ");
			else
				ImGui::Text("%s", it.m_is_gm ? "GM" : " ");
			ImGui::TableSetColumnIndex(3);
			ImGui::Text("%s", empire_str[it.m_empire]);
			ImGui::TableSetColumnIndex(4);
			ImGui::Text("%s", race_type_str[it.m_race]);
		}

		ImGui::EndTable();
	}
}

void GameInformer::PreIteration(Player* local)
{
	m_gms.clear();
	m_mobs.clear();
	m_players.clear();
	m_metins.clear();
	m_is_spectated = 0;
	m_local = G::game->GetLocalPlayer();
	
	if (!m_local)
		return;

	const auto* ent_map = G::game->GetEntityMapPtr();
	if (!ent_map || ent_map->empty())
		return;

	size_t ignore_count = 0;
	for (const auto& [vid, entity] : *ent_map)
	{
		if (!entity || entity == m_local)
			continue;

		if (entity->IsPC() || entity->IsPolyPC())						
		{
			if (std::strcmp(entity->GetName(), "Roksanka") == 0) //TODO: PT check
				ignore_count++;

			m_players.push_back(entity);
		}
		if (entity->IsEnemy() && !entity->IsPC() && !entity->IsNPC() && entity->IsAlive() && !entity->IsStuned())	
			m_mobs.push_back(entity);
		if (entity->IsGameMaster()) 
			m_gms.push_back(entity);
		if (entity->IsStone()) 
			m_metins.push_back(entity);
		if (entity->IsPC() || entity->IsPolyPC())
		{
			MeetPlayersData data;
			data.m_name = entity->GetName();
			data.m_level = entity->GetLevel();
			data.m_is_gm = entity->IsGameMaster();
			data.m_empire = entity->GetEmpireID();
			data.m_race = entity->GetRace();

			m_all_meet_players.insert(data);
		}
	}

	m_is_spectated = (m_players.size() + m_gms.size() - ignore_count > 0);
}

void GameInformer::OnIteration(Entity* entity)
{
	return;
}

void GameInformer::PostIteration()
{
	return;
}

void GameInformer::OnRenderWorld()
{
	return;
}

void GameInformer::OnRenderScreen()
{
	ShowLegacyInfoBox();
}

bool GameInformer::IsValid(Entity* entity)
{
    return false;
}

bool GameInformer::IsLocalSpectated()
{
	return m_is_spectated;
}

void GameInformer::ShowLegacyInfoBox()
{
	if (!G::cfg[EBool::GUI_SHOW_INFO_BOX]) return;

	auto offset = ImGui::GetMainViewport()->Pos;
	static Rect bb = Rect(250 + 950, 20, 520 + 950, 220);

	static bool click = false, wasDragging = false;
	static ImVec2 delta = { 0, 0 };
	ImVec2 localDragPoint = ImGui::GetMousePos();
	bool isHovering = ImGui::IsMouseHoveringRect((ImVec2&)bb.GetMin(), (ImVec2&)bb.GetMax(), false);
	if (isHovering && ImGui::IsMouseDown(0))
	{
		if (!click) //calculate delta
		{
			delta = { localDragPoint.x - bb.left, localDragPoint.y - bb.top };
			click = true;
		}

		bb.left = localDragPoint.x - delta.x;
		bb.top = localDragPoint.y - delta.y;
		bb.right = bb.left + (520 - 250);
		bb.bot = bb.top + (220 - 20);
		wasDragging = true;
	}
	else if (wasDragging && !isHovering && ImGui::IsMouseDown(0))
	{
		bb.left = localDragPoint.x - delta.x;
		bb.top = localDragPoint.y - delta.y;
		bb.right = bb.left + (520 - 250);
		bb.bot = bb.top + (220 - 20);
	}
	else { click = false; wasDragging = false; }

	auto network = G::game->GetNetworkMgr();
	if (!network) return;

	Vector2D textMin = { bb.GetMin().x + 20, bb.GetMin().y + 20 };
	DX8::screen->DrawRectGradient(bb, Color::Black(128), Color::Black(128), { 18, 128, 255, 0 }, { 18, 128, 255, 0 });

	ImGui::PushFont(DX8::Font::digital_10);
	DX8::screen->DrawString(textMin, 0, Color::Orange(), "FPS: %d", (int)ImGui::GetIO().Framerate);
	ImGui::PopFont();

	ImGui::PushFont(DX8::Font::verdana_18);
	textMin.y += 20;
	DX8::screen->DrawString(textMin, 0, Color(255, 202, 77), "Entities around:");

	textMin.y += 20;
	DX8::screen->DrawString(textMin, 0, Color(77, 142, 255), "Players: %d", m_players.size());
	DX8::screen->DrawCountingBar({ textMin.x + ((bb.GetSize().x - 40) / 2), textMin.y + 19 }, { (bb.GetSize().x - 40) , 3 }, m_players.size(), 140, true);

	textMin.y += 20;
	DX8::screen->DrawString(textMin, 0, Color(77, 142, 255), "Monsters: %d", m_mobs.size());
	DX8::screen->DrawCountingBar({ textMin.x + ((bb.GetSize().x - 40) / 2), textMin.y + 19 }, { (bb.GetSize().x - 40) , 3 }, m_mobs.size(), 230, true);

	textMin.y += 20;
	DX8::screen->DrawString(textMin, 0, Color(77, 142, 255), "Metins: %d", m_metins.size());
	DX8::screen->DrawCountingBar({ textMin.x + ((bb.GetSize().x - 40) / 2), textMin.y + 19 }, { (bb.GetSize().x - 40) , 3 }, m_metins.size(), 10, true);
	
	textMin.y += 20;
	DX8::screen->DrawString(textMin, 0, Color(77, 142, 255), "PPS: %d | MPPT: %d", network->GetPacketsPerSec(), network->GetMaxPacketsCount());
	textMin.y += 10;
	ImGui::SetNextWindowPos({ textMin.x - 8, textMin.y });
	ImGui::SetNextWindowSize({ bb.GetSize().x - 27, 0 });

	if (m_gms.size() > 0)
	{
		ImGui::PushFont(DX8::Font::verdana_32);
		Vector2D pos = { ImGui::GetMainViewport()->Size.x / 2.f,  40.f };
		DX8::screen->DrawString(pos, (DWORD)EStrFlag::CENTER_X | (DWORD)EStrFlag::CENTER_Y, Color::FireRed(), "GM found nearby!");
		ImGui::PopFont();
	}

	static auto plot_list = std::make_unique<std::deque<float>>();
	auto plot = plot_list.get();
	if (plot->size() > network->GetMaxPacketsCount()) plot->pop_front();
	else plot->push_back(network->GetPacketsCount());

	auto alpha_backup = ImGui::GetStyle().Alpha;
	ImGui::GetStyle().Alpha = 1.f;
	ImGui::Begin("##plot", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::PlotLines("##plotLines", [](void* data, int idx) { return reinterpret_cast<std::deque<float>*>(data)->at(idx); }, (void*)plot, plot->size());
	ImGui::End();
	ImGui::GetStyle().Alpha = alpha_backup;

	ImGui::PopFont();
}
