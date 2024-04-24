#include "pch.h"

SkillData& Skill::GetSkillData(ESkillIndex skill_index)
{
	return GetSkillDataMap().at((VID)skill_index);
}

std::map<VID, SkillData>& Skill::GetSkillDataMap()
{
	static auto CPythonSkillPtr = G::memory->GetPointer("CPythonSkill");
	return *reinterpret_cast<std::map<VID, SkillData>*>(DWORD(*CPythonSkillPtr) + 0x4);
}

const std::vector<SkillData>& Skill::GetSelfBuffSkillList()
{
	static std::vector<VID> list = { 3, 4, 19, 34, 49, 63, 64, 65, 79 };
	static std::vector<SkillData> skill_list;
	static bool init = true;
	if (init)
	{
		init = false;
		for (const auto& vid : list)
			skill_list.push_back(GetSkillData((ESkillIndex)vid));
	}
	return skill_list;
}

const std::vector<SkillData>& Skill::GetTargetBuffSkillList()
{
	static std::vector<VID> list = { 94, 95, 96, 109, 110, 111 };
	static std::vector<SkillData> skill_list;
	static bool init = true;
	if (init)
	{
		init = false;
		for (const auto& vid : list)
			skill_list.push_back(GetSkillData((ESkillIndex)vid));
	}
	return skill_list;
}

const std::vector<SkillData>& Skill::GetAttackSkillList()
{
	static std::vector<VID> list = {
		1, 2, 5, 6, 16, 17, 18, 20, 21, 31, 32, 
		33, 35, 36, 46, 47, 48, 50, 51, 61, 62, 
		66, 76, 77, 78, 80, 81, 91, 92, 93, 106, 
		107, 108, 137, 138, 139, 140 };
	static std::vector<SkillData> skill_list;
	static bool init = true;
	if (init)
	{
		init = false;
		for (const auto& vid : list)
			skill_list.push_back(GetSkillData((ESkillIndex)vid));
	}
	return skill_list;
}

const std::vector<SkillData*>& Skill::GetSkillList(DWORD dwRace)
{
	static std::vector<SkillData*> skill_list;
	int job = dwRace % 4;
	static int prev_race = -1;
	static int prev_job = -1;

	if (prev_race != dwRace || prev_job != job)
	{
		skill_list.clear();
		prev_race = dwRace;
		prev_job = job;

		//TODO: job check

		int index = -1;
		switch (dwRace)
		{
		case 0:
		case 4:
			index = 1; //warrior
			break;
		case 1:
		case 5:
			index = 31; //Assassin
			break;
		case 2:
		case 6:
			index = 61; //Sura
			break;
		case 3:
		case 7:
			index = 91; //Shaman
			break;
		default:
			return skill_list;
		}

		for (int i = index; i <= index + 20; i++)
		{
			if (i > index + 5 && i < index + 15) //skill issue
				continue;

			skill_list.push_back(&GetSkillData((ESkillIndex)i));
		}


	}
	return skill_list;
}

bool SkillData::CanSelfBuff() const
{
	if (!(dwSkillAttribute & SKILL_ATTRIBUTE_ATTACK_SKILL))
	{
		if ((dwSkillAttribute & SKILL_ATTRIBUTE_ONLY_FOR_ALLIANCE)
			|| (dwSkillAttribute & SKILL_ATTRIBUTE_STANDING_SKILL))
			return true;
	}
	return false;
}

bool SkillData::CanTargetBuff() const
{
	return (
		!(dwSkillAttribute & SKILL_ATTRIBUTE_ATTACK_SKILL)
		&& (dwSkillAttribute & SKILL_ATTRIBUTE_ONLY_FOR_ALLIANCE)
		&& (dwSkillAttribute & SKILL_ATTRIBUTE_NEED_TARGET));
}

bool SkillData::IsAttackSkill() const
{
	return (dwSkillAttribute & SKILL_ATTRIBUTE_ATTACK_SKILL);
}

const LPDIRECT3DTEXTURE8 SkillData::GetTexture() const
{
	DWORD img = *(DWORD*)(0x40 + DWORD(this) + 0x178);
	return *(LPDIRECT3DTEXTURE8*)(img + 0x3C);
}

bool Skill::IsSkillCoolTime(ESkillIndex skill_index)
{
	static auto CPythonPlayerPtr = G::memory->GetPointer("CPythonPlayer");
	static const auto fnAddr = G::memory->GetAddress("IsSkillCoolTime");
	static auto call = reinterpret_cast<bool(__thiscall*)(void*, int)>(fnAddr);
	return call(*(void**)CPythonPlayerPtr, (int)skill_index);
}

bool Skill::IsSkillActive(ESkillIndex skill_index)
{
	static auto CPythonPlayerPtr = G::memory->GetPointer("CPythonPlayer");
	static const auto fnAddr = G::memory->GetAddress("IsSkillActive");
	static auto call = reinterpret_cast<
		bool(__thiscall*)(DWORD*, DWORD)>(*(DWORD*)(fnAddr + 1) + fnAddr + 5); // relative call
	return call(*(DWORD**)CPythonPlayerPtr, (int)skill_index);
}

bool Skill::IsSkillReady(ESkillIndex skill_index)
{
	if (((GetSkillData(skill_index).dwSkillAttribute) & SKILL_ATTRIBUTE_TOGGLE))
		return !IsSkillActive(skill_index);
	else
		return !IsSkillCoolTime(skill_index);
}

ESkillIndex Skill::GetSkillIndex(DWORD dwSlotIndex)
{
	static auto CPythonPlayerPtr = *(DWORD**)G::memory->GetPointer("CPythonPlayer");
	if (dwSlotIndex < 255)
		return (ESkillIndex)CPythonPlayerPtr[0xA * dwSlotIndex + 0x3BA9];
	return ESkillIndex::COUNT;
}

DWORD Skill::GetSkillSlot(ESkillIndex skill_index)
{
	static auto CPythonPlayerPtr = *(DWORD**)G::memory->GetPointer("CPythonPlayer");
	const auto& skill_dict = *reinterpret_cast<std::map<DWORD, DWORD>*>(CPythonPlayerPtr + 5);
	auto found = skill_dict.find((DWORD)skill_index);
	if (found != skill_dict.end())
		return found->second;
	return 0;
}

bool Skill::UseSkill(DWORD dwSlotIndex)
{
	static auto CPythonPlayerPtr = G::memory->GetPointer("CPythonPlayer");
	static auto fnAddr = G::memory->GetAddress("UseSkill");
	static auto __UseSkill = reinterpret_cast<bool(__thiscall*)(void*, DWORD)>(fnAddr);
	return __UseSkill(*(void**)CPythonPlayerPtr, dwSlotIndex);
}

bool Skill::IsSkillUsedByRace(ESkillIndex skill_index, DWORD dwRace)
{
	switch (dwRace)
	{
	case 0:
	case 4:
		return (skill_index >= 1 && skill_index <= 21); //warrior
	case 1:
	case 5:
		return (skill_index >= 31 && skill_index <= 51); //Assassin
	case 2:
	case 6:
		return (skill_index >= 61 && skill_index <= 81); //Sura
	case 3:
	case 7:
		return (skill_index >= 91 && skill_index <= 111); //Shaman
	}
	return false;
}

bool Skill::IsSkillUsedByJob(ESkillIndex skill_index, DWORD dwJob)
{
	return false;

	//ITS WRONG
	switch (dwJob)
	{
	case 0: return (skill_index >= 1 && skill_index <= 21 - 6); //warrior
	case 4: return (skill_index >= 1 && skill_index <= 21); //warrior
	case 1: return (skill_index >= 31 && skill_index <= 51 - 6); //Assassin
	case 5: return (skill_index >= 31 && skill_index <= 51); //Assassin
	case 2: return (skill_index >= 61 && skill_index <= 81 - 6); //Sura
	case 6: return (skill_index >= 61 && skill_index <= 81); //Sura
	case 3: return (skill_index >= 91 && skill_index <= 111 - 6); //Shaman
	case 7: return (skill_index >= 91 && skill_index <= 111); //Shaman
	}
	return false;
}
