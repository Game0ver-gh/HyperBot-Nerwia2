#include "pch.h"
#include "MiscHacks.h"

static bool auto_use_skills = false, back_to_horse = true;
char custom_target_name[32];
int step = 10;

void MiscHacks::Init()
{
	/*for (const auto& skill : G::game->GetLocalPlayer()->GetSkillDataMap())
	{
		if (skill.second.dwSkillAttribute & SKILL_ATTRIBUTE_TOGGLE)
			printf("%s\n",skill.second.strName.c_str());
	}*/
	return;
}

void MiscHacks::OnMenuFrame()
{
	ImGui::CustomCheckbox("Enable speedboost", &G::cfg[EBool::M_SB_ENABLED]);
	ImGui::CustomHotkey("Hotkey##SP", &G::cfg[EInt::M_SB_HOTKEY]);
	ImGui::CustomCheckbox("Check collision", &G::cfg[EBool::M_SB_CHECK_COLLISION]);
	ImGui::CustomSliderFloat("Boost multiplier", &G::cfg[EFloat::M_SB_MULTIPLIER], 50.f, 500.f, "%.0f");
	ImGui::Separator();
	ImGui::CustomCheckbox("Disable camera collision", &G::cfg[EBool::M_DISABLE_CAMERA_COLLISION]);
	ImGui::CustomCheckbox("Mob wallhack", &G::cfg[EBool::M_MOB_WALLHACK]);
	ImGui::CustomCheckbox("Wallhack", &G::cfg[EBool::M_WALLHACK]);
	ImGui::CustomCheckbox("No fly", &G::cfg[EBool::M_NO_FLY]);
	ImGui::Separator();
	ImGui::CustomCheckbox("Auto buff", &auto_use_skills); 
	ImGui::CustomCheckbox("Back on horse", &back_to_horse);
	ImGui::InputTextWithHint("##AutoBuff", "Custom target name...", custom_target_name, 32);
	auto local = G::game->GetLocalPlayer();
	if (local)
	{
		if (ImGui::BeginCombo("##AUTO_SKILL", "Sellect skills:"))
		{
			const auto& skills = local->GetSkillList(m_local->GetRace());

			for (size_t i = 0; i < skills.size(); i++)
			{
				if (!skills[i]) continue;

				bool enabled = false, was_enabled = false;
				for (auto& skill : m_skills)
				{
					if (skill->strName == skills[i]->strName)
					{
						enabled = true;
						break;
					}
				}
				
				was_enabled = enabled;
				if (ImGui::Selectable(skills[i]->strName.c_str(), &enabled, ImGuiSelectableFlags_DontClosePopups))
				{
					if (!enabled && was_enabled) //remove
					{
						m_skills.erase(std::remove_if(m_skills.begin(), m_skills.end(), [&](SkillData* skill) 
							{ return skill->strName == skills[i]->strName; }));
					}
					else if (enabled && !was_enabled) //add
					{
						m_skills.push_back(skills[i]);
					}
				}
			}
			ImGui::EndCombo();
		}
	}
	ImGui::Separator();
}

void MiscHacks::PreIteration(Player* local)
{
    m_local = local;
    m_local->GetPixelPosition(&m_local_pos);

	//Handle race switch so bot wont use skills which are not for its character race
	static auto race = m_local->GetRace();
	if (race != m_local->GetRace())
	{
		race = m_local->GetRace();
		m_skills.clear();
	}

	AutoUsePassiveSkills();
    RunSpeedBoost();
	UnlockCamerZoom();
	DisableFog();

	static auto addr = Memory::FindSignature("55 8B EC 51 F3 0F 10 45 ? 56 57 51 8B F9 F3 0F 11 04 24");
	static auto call = reinterpret_cast<void(__thiscall*)(Entity*, float)>(addr);

	//Fast rotate when on horse
	//call(m_local, 0.f);
	//auto* p = (DWORD*)m_local;
	//*(float*)(p + 0x4BC) = 0.f;
	//*(float*)((DWORD*)m_local + 0x1D8) = 1.f;
	//*(float*)((DWORD*)m_local + 0x1DC) = 1.f;
	//*(DWORD*)((DWORD*)m_local + 0x180) = 0;

}

void MiscHacks::OnIteration(Entity* entity)
{
	return;
}

void MiscHacks::PostIteration()
{
}

void MiscHacks::OnRenderWorld()
{
}

void MiscHacks::OnRenderScreen()
{
	return;
}

bool MiscHacks::IsValid(Entity* entity)
{
    return false;
}

void MiscHacks::RunSpeedBoost()
{
	if (!G::cfg[EBool::M_SB_ENABLED])
		return;

	auto network = G::game->GetNetworkMgr();
	if (!network || network->GetAvailablePackets() < 1)
		return;

	if (!(GetAsyncKeyState(G::cfg[EInt::M_SB_HOTKEY]) & 0x8000))
		return;

	if (!m_local->IsWalking()) return;

	float rotation = m_local->GetRotation();
	float mult = G::cfg[EFloat::M_SB_MULTIPLIER];

	Vector rotation_vec = Vector(mult * sinf(rotation * 0.017453f), mult * cosf(rotation * 0.017453f) * -1.f, 0.f);
	Vector new_pos = m_local_pos + rotation_vec;

	if (G::cfg[EBool::M_SB_CHECK_COLLISION] && G::game->GetBackground()->IsBlockedCoord(new_pos))
		return;

	static int count = 0;
	if (count >= 1)
	{
		count = 0;
		network->SendCharacterStatePacket(new_pos, rotation, 1, 0);
	}
	else
	{
		count++;
		network->SendCharacterStatePacket(new_pos, rotation, 0, 0);
	}

	m_local->SCRIPT_SetPixelPosition(new_pos);
}

void MiscHacks::UnlockCamerZoom()
{
	static auto max_camera_dist_addr = G::memory->GetAddress("max_camera_dist");
	*reinterpret_cast<float*>(max_camera_dist_addr) = G::cfg[EFloat::M_MAX_CAMERA_DISTANCE];
}

void MiscHacks::DisableFog()
{
	static auto min_fog_dist_addr = G::memory->GetAddress("min_fog_dist");
	*reinterpret_cast<float*>(min_fog_dist_addr) = G::cfg[EFloat::M_MAX_CAMERA_DISTANCE];
}

void MiscHacks::AutoUsePassiveSkills()
{
	if (!auto_use_skills)
		return;

	static bool is_skill_ready = false;

	for (const auto& skill : m_skills)
	{
		if (!skill) 
			continue;

		auto skill_idx = (ESkillIndex)skill->dwSkillIndex;
		auto skill_slot = m_local->GetSkillSlot(skill_idx);
		if (skill_slot == 0) 
			return;

		is_skill_ready = m_local->IsSkillReady(skill_idx);

		if (is_skill_ready)
		{
			static bool was_riding = false, riding = false;
			if (m_local->IsMountingHorse() && !was_riding)
			{
				was_riding = true;
				riding = false;
				m_local->DismountHorse();
			}
			else if (!m_local->IsMountingHorse())
			{
				m_local->UseSkill(skill_slot);

				if (was_riding && back_to_horse)
				{
					was_riding = false;
					m_local->MountHorse();
				}
				else if (back_to_horse && !riding)
				{
					riding = true;
					m_local->MountHorse();
				}
			}
		}
	}
}