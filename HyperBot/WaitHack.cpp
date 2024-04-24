#include "pch.h"
#include "WaitHack.h"

#define MAX_WH_RANGE 175.f //Max range, after which player will be rubberbanded back

WaitHack::~WaitHack()
{
}

void WaitHack::Init()
{
	return;
}

void WaitHack::OnMenuFrame()
{
	ImGui::CustomCheckbox("Enable", &G::cfg[EBool::WH_ENABLED]);
	ImGui::CustomHotkey("Hotkey##WH", &G::cfg[EInt::WH_HOTKEY]);
	ImGui::Separator();
	{
		ImGui::CustomCheckbox("Range exploit", &G::cfg[EBool::WH_RANGE_EXPLOIT]);
		ImGui::CustomCheckbox("Attack on target", &G::cfg[EBool::WH_SELECTED_TARGET_ONLY]);
		ImGui::CustomCheckbox("Kill stunned", &G::cfg[EBool::WH_KILL_STUNED]);
		ImGui::CustomCheckbox("Disable on sight", &G::cfg[EBool::WH_ANTI_SPECTATE]);
		ImGui::CustomCheckbox("Check collision", &G::cfg[EBool::WH_CHECK_COLLISION]);
		ImGui::CustomCheckbox("Ignore normal attacks", &G::cfg[EBool::WH_IGNORE_NORMAL_ATTACKS]);
		//ImGui::CustomCheckbox("Ignore PT players", &G::cfg[EBool::WH_IGNORE_SKILL_ATTACKS]);
		ImGui::CustomSliderFloat("Range", &G::cfg[EFloat::WH_RANGE], 500.f, 10000.f, "%.0f");
		ImGui::CustomSliderFloat("Attack speed interval", &G::cfg[EFloat::WH_ATTACK_SPEED], 50.f, 1000.f, "%.0fms");
		ImGui::CustomCheckbox("Draw range", &G::cfg[EBool::WH_DRAW_RANGE], &G::cfg[EColor::WH_RANGE]);
		ImGui::CustomCheckbox("Draw target", &G::cfg[EBool::WH_DRAW_CUR_TARGET], &G::cfg[EColor::WH_CUR_TARGET]);

		static std::string disp = "";
		static const std::string filter[] = { " Players", " Mobs", " Metins", " Bosses" };
		static bool init = true;
		if (init)
		{
			init = false;
			for (size_t i = size_t(EBool::WH_F_PLAYERS); i <= size_t(EBool::WH_F_BOSSES); i++)
			{
				if (G::cfg[EBool(i)]) disp.append(filter[i - size_t(EBool::WH_F_PLAYERS)]).append(",");
			}
			if (disp.empty()) disp = "...";
		}

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 4.f);
		ImGui::Text("Filter: "); ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowWidth() / 2.f);
		ImGui::SetNextItemWidth(177.f);
		if (ImGui::BeginCombo("##WH_Filter", disp.c_str()))
		{
			
			std::vector<std::string> vec;
			
			for (size_t i = size_t(EBool::WH_F_PLAYERS); i <= size_t(EBool::WH_F_BOSSES); i++)
			{
				ImGui::Selectable(filter[i - size_t(EBool::WH_F_PLAYERS)].c_str(), &G::cfg[EBool(i)], ImGuiSelectableFlags_DontClosePopups);
				if (G::cfg[EBool(i)]) vec.push_back(filter[i - size_t(EBool::WH_F_PLAYERS)]);
			}
			if (!vec.empty())
			{
				disp = "";
				for (size_t i = 0; i < vec.size(); i++)
				{
					disp += vec[i];
					if (i < vec.size() - 1) disp += ",";
				}
			}
			else disp = "...";

			ImGui::EndCombo();
		}
	}
}

void WaitHack::PreIteration(Player* local)
{
	if (GetAsyncKeyState(G::cfg[EInt::WH_HOTKEY]) & 1)
		G::cfg[EBool::WH_ENABLED] = !G::cfg[EBool::WH_ENABLED];

	if (!G::cfg[EBool::WH_ENABLED])
		return;

	static auto ap_timer = Clock::GetCurTime();

	m_local = local;
	m_local->GetPixelPosition(&m_local_pos);
	
	m_hit_targets.clear();
	m_is_ranged = G::cfg[EBool::WH_RANGE_EXPLOIT];
	m_wh_distance = m_is_ranged ? G::cfg[EFloat::WH_RANGE] : MAX_WH_RANGE;
	m_attack_speed = G::cfg[EFloat::WH_ATTACK_SPEED];
	m_attack_time = Clock::GetCurTime() - ap_timer;
	m_is_attack_ready = m_attack_time >= m_attack_speed;
	if (m_is_attack_ready)
		ap_timer = Clock::GetCurTime(); 
}

void WaitHack::OnIteration(Entity* entity)
{
	if (!G::cfg[EBool::WH_ENABLED])
		return;

	if (!IsValid(entity))
		return;

	AddNewTarget(entity);

	if (!m_hit_targets.empty())
		AttackTarget(m_hit_targets.back());
}
 
void WaitHack::PostIteration()
{
	return;
}

void WaitHack::OnRenderWorld()
{
	if (!G::cfg[EBool::WH_ENABLED])
		return;

	if (G::cfg[EBool::WH_DRAW_RANGE])
	{
		DX8::screen->SetDepth(false);
		DX8::screen->RenderCircle3D(m_local_pos, G::cfg[EFloat::WH_RANGE], 64, G::cfg[EColor::WH_RANGE_COVERED]);
		DX8::screen->SetDepth(true);
		DX8::screen->RenderCircle3D(m_local_pos, G::cfg[EFloat::WH_RANGE], 64, G::cfg[EColor::WH_RANGE]);
	}
	
	if (G::cfg[EBool::WH_DRAW_CUR_TARGET] && !m_hit_targets.empty())
	{
		for (const auto& target : m_hit_targets)
		{
			DX8::screen->SetDepth(false);
			DX8::screen->RenderLine3D(m_local_pos, target.m_position, G::cfg[EColor::WH_CUR_TARGET_COVERED]);
			DX8::screen->SetDepth(true);
			DX8::screen->RenderLine3D(m_local_pos, target.m_position, G::cfg[EColor::WH_CUR_TARGET]);

			DX8::screen->SetDepth(false);
			DX8::screen->RenderAABB3D(target.m_bound_box_min, target.m_bound_box_max, G::cfg[EColor::WH_CUR_TARGET_COVERED]);
			DX8::screen->SetDepth(true);
			DX8::screen->RenderAABB3D(target.m_bound_box_min, target.m_bound_box_max, G::cfg[EColor::WH_CUR_TARGET]);
		}
	}
}

void WaitHack::OnRenderScreen()
{
	return;
}

bool WaitHack::IsValid(Entity* entity)
{
	//Basic filtering
	if (!entity
		|| !entity->IsAlive()
		|| entity == m_local
		|| entity->IsNPC()
		|| entity->IsBuilding()
		|| entity->IsDoor()
		|| (G::cfg[EBool::WH_KILL_STUNED] ? false : entity->IsStuned()))
		return false;

	if (G::cfg[EBool::WH_SELECTED_TARGET_ONLY] && !entity->IsSelectedTarget())
		return false;
	
	if (G::cfg[EBool::WH_F_PLAYERS] && entity->IsPC()) 
		return true;
	
	if (G::cfg[EBool::WH_F_MOBS] && entity->IsMonster())
		return true;
	
	if (G::cfg[EBool::WH_F_METINS] && entity->IsStone())
		return true;
	
	if (G::cfg[EBool::WH_F_BOSSES] && entity->IsBoss())
		return true;
	
	return false;
}

const std::vector<WaitHack::Target>& WaitHack::GetTargetList()
{
	return m_hit_targets;
}

bool WaitHack::AttackTarget(const Target& target)
{
	if (!G::cfg[EBool::WH_IGNORE_NORMAL_ATTACKS])
	{
		if (m_local->IsAttacking())
			return false;
	}

	if (!m_is_attack_ready)
		return false;

	if ((G::cfg[EBool::WH_ANTI_SPECTATE] && F::game_informer->IsLocalSpectated()))
		return false;

	auto* network = G::game->GetNetworkMgr();
	if (!network) return false;

	if (m_is_ranged)
	{
		return m_local->ShortBlinkTo(target.m_position, G::cfg[EBool::WH_CHECK_COLLISION], [&](const Vector& last_point) -> void
			{
				if (m_local->IsBowMode())
				{
					network->SendFlyTargetingPacket(target.m_target->GetVID(), target.m_position);
					network->SendCharacterStatePacket(last_point, 120.f, 3, 14); //for func and arg - check leaked metin2 source
					network->SendShootPacket(0);
				}
				else
				{
					network->SendAttackPacket(target.m_target->GetVID());
				}
			}, m_local->IsBowMode());
	}
	else
	{
		if (m_local->IsBowMode())
		{
			network->SendFlyTargetingPacket(target.m_target->GetVID(), target.m_position);
			network->SendCharacterStatePacket(m_local_pos, 120.f, 3, 14);
			return network->SendShootPacket(0);
		}
		else return network->SendAttackPacket(target.m_target->GetVID());
	}
}

void WaitHack::AddNewTarget(Entity* entity)
{
	Target target;
	target.m_target = entity;
	target.m_target->GetPixelPosition(&target.m_position);
	target.m_distance = m_local->GetDistanceTo(target.m_position);
	if (target.m_distance > m_wh_distance)
		return;

	target.m_target->GetBoundBoxTransformed(&target.m_bound_box_min, &target.m_bound_box_max);
	m_hit_targets.emplace_back(target);
}
