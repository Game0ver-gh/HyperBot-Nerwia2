#include "pch.h"
#include "Item.h"

int Item::GetItemSlot(DWORD vNum, BYTE storageType)
{
    for (WORD i = 0; i < EQ_TOTAL_SLOTS; i++)
    {
        int cur_vnum = __GetItemVID(TItemPos(storageType, i));
        if (vNum == cur_vnum) return i;
    }
    return -1;
}

int Item::GetItemSlot(const std::string& name, BYTE storageType)
{
    const auto& item_pair = G::game->GetItem(name);
    if (!item_pair.second) return -1;

    for (WORD i = 0; i < EQ_TOTAL_SLOTS; i++)
    {
        auto cur_vid = __GetItemVID(TItemPos(storageType, i));
        if (cur_vid == item_pair.first)
            return i;
    }
    return -1;
}

ItemData* Item::GetItemData(WORD slot, BYTE storageType)
{
    VID vid = __GetItemVID(TItemPos(storageType, slot));
    auto* item_map = G::game->GetItemMapPtr();
    if (item_map)
    {
        auto result = item_map->find(vid);
        if (result != item_map->end())
            return result->second;
    }

    return nullptr;
}

VID Item::GetItemVID(WORD slot, BYTE storageType)
{
    return __GetItemVID(TItemPos(storageType, slot));
}

VID Item::__GetItemVID(const TItemPos& item_pos)
{
    static auto python_player = G::memory->GetPointer("CPythonPlayer");
    if (!python_player) return 0;

    auto ptr = (int)(*python_player);
    auto v1 = *(DWORD*)(ptr + 4);

    return (VID)(*(int(__thiscall**)(int, TItemPos))(v1 + 0x48))(ptr + 0x4, item_pos);
}

Vector GroundItemData::GetPixelPosition()
{
    auto vec = *(Vector*)((DWORD)this + 0x40);
    vec.y *= -1.f;
    return vec;
}

VNUM GroundItemData::GetVirtualNumber()
{
    return dwVirtualNumber;
}

void GroundItemData::GetBoundBox(Vector* min, Vector* max)
{
    *min = m_v3Min;
	*max = m_v3Max;
}

void GroundItemData::GetBoundBoxTransformed(Vector* min, Vector* max)
{
	*min = m_v3Min + GetPixelPosition();
	*max = m_v3Max + GetPixelPosition();
}

float GroundItemData::GetRadius()
{
    return m_fRadius;
}

const char* GroundItemData::GetOwnerName()
{
    auto result = (DWORD*)((DWORD)(this) + 0x2F8);
    if (*(DWORD*)((DWORD)(this) + 0x30C) >= 16)
        result = (DWORD*)*result;
    return (const char*)result;
}

float GroundItemData::GetDistanceTo(const Vector& pos)
{
    Vector this_pos = this->GetPixelPosition();
    return sqrtf((this_pos.x - pos.x) * (this_pos.x - pos.x) + (this_pos.y - pos.y) * (this_pos.y - pos.y));
}

bool GroundItemData::CanBePickedUp()
{
    auto* name = GetOwnerName();
    if (!name || name[0] == '\0') return true;
    
	if (std::strcmp(name, G::game->GetLocalPlayer()->GetName()) == 0)
		return true;

    return false;
}

const wchar_t* ItemData::GetNameW()
{
    return (wchar_t*)(DWORD(this) + 0xED);
}

const char* ItemData::GetName()
{
    return (char*)(DWORD(this) + 0xED);
}

const char* ItemData::GetDescryption()
{
    return (char*)(DWORD(this) + 0x64);
}

const char* ItemData::GetIconPath()
{
    auto result = (DWORD*)(DWORD(this) + 0x4C);
    result = (DWORD*)*result;
    return (const char*)result;
}

const char* ItemData::GetModelPath()
{
    return (char*)(DWORD(this) + 0x34);
}

EItemType ItemData::GetType()
{
    auto result = (DWORD(this) + 0x106);
    return (EItemType)*(BYTE*)result;
}

EItemSubType ItemData::GetSubType()
{
    auto result = (DWORD(this) + 0x107);
    return (EItemSubType)*(BYTE*)result;
}

int ItemData::GetRefineLevel()
{
    std::string tmp = GetName();
    if (tmp.empty()) return -1;
    int offset = tmp.at(tmp.size() - 1);
    if (offset >= '0' && offset <= '9')
        return offset - 48;
    return -1;
}
