#include "pch.h"
#include "Entity.h"
#include <d3dx8.h>
#include <DirectXMath.h>

float Entity::GetRotationTo(Entity* pEntity)
{
    return 0.f;
}

float Entity::GetRotationTo(const Vector& pos)
{
    return 0.f;
}

float Entity::GetDistanceTo(Entity* pEntity)
{
    Vector this_pos, pos; 
    this->GetPixelPosition(&this_pos);
    pEntity->GetPixelPosition(&pos);
	return sqrtf((this_pos.x - pos.x) * (this_pos.x - pos.x) + (this_pos.y - pos.y) * (this_pos.y - pos.y));
}

float Entity::GetDistanceTo(const Vector& pos)
{
    Vector this_pos;
    GetPixelPosition(&this_pos);
    return sqrtf((this_pos.x - pos.x) * (this_pos.x - pos.x) + (this_pos.y - pos.y) * (this_pos.y - pos.y));
}

bool Entity::IsCloseToPosition(const Vector& dstPos, float approx)
{
    return (GetDistanceTo(dstPos) <= approx);
}

bool Entity::HasEffect(UINT effect)
{
	const auto& effect_map = GetEffectMap();
    if (effect_map.empty()) return false;
    return (effect_map.find(effect) != effect_map.end());
}

bool Entity::IsSelectedTarget()
{
    return HasEffect(EFFECT_TARGET);
}

bool Entity::IsMonster()
{
    return !IsPC() && !IsStone() && IsEnemy();
}

bool Entity::IsBoss() 
{
    static VNUM prev_vnum = 0;
	VNUM cur_vnum = GetVirtualNumber();
    if (prev_vnum != cur_vnum)
    {
        prev_vnum = cur_vnum;
        TMobTable* mob_data = G::game->GetNonPlayerData(GetVirtualNumber()); //calling map->find()
        if (mob_data)
            return (int(mob_data->bRank) >= 4 && IsMonster());
    }
    return false;
}

bool Entity::IsEnemy()
{
    return GetType() == EntityType::TYPE_ENEMY;
}

bool Entity::IsPC()
{
    return GetType() == EntityType::TYPE_PC;
}

bool Entity::IsNPC()
{
    return GetType() == EntityType::TYPE_NPC;
}

bool Entity::IsStone()
{
    return GetType() == EntityType::TYPE_STONE;
}

bool Entity::IsAlive()
{
    auto graphicThingPtr = (int)((uintptr_t)this + 0x1E4);
    if (!(*(bool*)(graphicThingPtr + 0x448))) return true;
    return false;
}

bool Entity::IsStuned()
{
    auto graphicThingPtr = (int)((uintptr_t)this + 0x1E4);
    if ((*(bool*)(graphicThingPtr + 0x444))) return true;
    return false;
}

bool Entity::IsWalking()
{
    static auto fnAddr = G::memory->GetAddress("IsWalking");
    static auto GetMoveMotion = reinterpret_cast<bool(__thiscall*)(void*)>(fnAddr);
    if (GetMoveMotion) return GetMoveMotion((void*)((uintptr_t)this + 0x1E4));
    return false;
}

bool Entity::IsBuilding()
{
    return (GetType() == EntityType::TYPE_BUILDING);
}

bool Entity::IsDoor()
{
	return (GetType() == EntityType::TYPE_DOOR);
}

bool Entity::IsPolyPC()
{
	return (GetType() == EntityType::TYPE_POLY);
}

EntityType Entity::GetType()
{
    return *(EntityType*)(((DWORD)this + 0x1E4) + 0x3FC);
}

void Entity::GetPixelPosition(Vector* position)
{
    const static auto fnAddr = G::memory->GetAddress("GetPixelPosition");
    static auto GetPixelPositionFn = reinterpret_cast<void(__thiscall*)(void*, Vector*)>(fnAddr);
    GetPixelPositionFn(this, position);
}

RaceType Entity::GetRace()
{
    if (this) return *(RaceType*)((uintptr_t)this + 0x16C);
    return RaceType::MAIN_RACE_MAX_NUM;
}

int Entity::GetJob()
{
    return (GetRace() % 4);
}

const char* Entity::GetName()
{
    auto result = (DWORD*)(uintptr_t(this) + 0x10);
    if (*(DWORD*)(uintptr_t(this) + 0x24) >= 16)
        result = (DWORD*)*result;
    return (const char*)result;
}

VNUM Entity::GetVirtualNumber()
{
    return *(int*)((uintptr_t)this + 0x174);
}

VID Entity::GetVID()
{
    return *(VID*)((uintptr_t)this + 0x744);
}

float Entity::GetRotation()
{
    return (180.0f - *(float*)(uintptr_t(this) + 0x208));
}

DWORD Entity::GetLevel()
{
    return *(DWORD*)(DWORD(this) + 0x44);
}

EmpireType Entity::GetEmpireID()
{
    return *(EmpireType*)(DWORD(this) + 0x48);
}

void Entity::GetBoundBox(Vector* min, Vector* max)
{
    const static auto fnAddr = G::memory->GetAddress("GetBoundBox2D");
    static auto GetBoundBoxOnlyXY = reinterpret_cast<int(__thiscall*)(void*, Vector*, Vector*)>(fnAddr);
    GetBoundBoxOnlyXY(this, min, max);
}

void Entity::GetBoundBoxTransformed(Vector* min, Vector* max)
{
    Vector _pos;
    GetPixelPosition(&_pos);
    GetBoundBox(min, max);
    *min += _pos;
    *max += _pos;
}

bool Entity::IsGameMaster()
{
    const static auto fnAddr = G::memory->GetAddress("IsGameMaster");
    static auto IsGM = reinterpret_cast<bool(__thiscall*)(void*)>(fnAddr);
    if (IsGM) return IsGM(this);
    return false;
}

bool Entity::IsAttacking()
{
    const static auto fnAddr = G::memory->GetAddress("IsAttacking");
    static auto fun = reinterpret_cast<bool(__thiscall*)(void*)>(fnAddr);
    return fun((void*)((uintptr_t)this + 0x1E4));
}

bool Entity::IsSuraRace()
{
    return (GetRace() == RaceType::MAIN_RACE_SURA_M || GetRace() == RaceType::MAIN_RACE_SURA_W);
}

bool Entity::IsWarriorRace()
{
    return (GetRace() == RaceType::MAIN_RACE_WARRIOR_M || GetRace() == RaceType::MAIN_RACE_WARRIOR_W);
}

bool Entity::IsAssassinRace()
{
    return (GetRace() == RaceType::MAIN_RACE_ASSASSIN_M || GetRace() == RaceType::MAIN_RACE_ASSASSIN_W);
}

bool Entity::IsShamanRace()
{
    return (GetRace() == RaceType::MAIN_RACE_SHAMAN_M || GetRace() == RaceType::MAIN_RACE_SHAMAN_W);
}

bool Entity::IsOfflineShop()
{
    return (GetType() == EntityType::TYPE_NPC && (*(int*)((uintptr_t)this + 0x50) == 0x4));
}

bool Entity::IsPartyMember(const std::string& name)
{
    //TODO
    //std::map<DWORD, TPartyMemberInfo>
    return false;
}

bool Entity::IsPartyMember(VID vid)
{
    //TODO
    return false;
}

std::map<DWORD, DWORD>& Entity::GetEffectMap()
{
    return (*(std::map<DWORD, DWORD>*)(this + 0x158));
}

void Entity::SCRIPT_SetPixelPosition(const Vector& pos)
{
    static const auto fnAddr = G::memory->GetAddress("SCRIPT_SetPixelPosition");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*, float, float)>(fnAddr);
    call(this, pos.x, pos.y);
}

void Player::LookAtDestPixelPosition(const Vector& pos)
{
    static const auto addr = G::memory->GetAddress("LookAtDestPixelPosition");
    static auto LookAtDestPixelPosition = reinterpret_cast<bool(__thiscall*)(void*, const Vector&)>(addr);
    LookAtDestPixelPosition(this, pos);
}

bool Player::MoveToDestPixelPosition(const Vector & pos)
{
    static const auto addr = G::memory->GetAddress("MoveToDestPixelPosition");
    static auto NEW_Goto = reinterpret_cast<bool(__thiscall*)(void*, const Vector&, float)>(addr);
    return NEW_Goto(this, pos, 0.0f);
}

bool Player::MoveFromDestPosition(const Vector& pos, float angle, float dist)
{
    //TODO
    return false;
}

bool Player::AttackToDestInstance(Entity* pEntity)
{
    static auto fnAddr = G::memory->GetAddress("AttackToDestInstance");
    static auto call = reinterpret_cast<bool(__thiscall*)(void*, const Vector&)>(fnAddr);
    if (pEntity)
    {
        Vector pos; pEntity->GetPixelPosition(&pos);
        return call(this, pos);
    }
    return false;
}

bool Player::SetAttackKey(bool enable)
{
    static const auto fnAddr = G::memory->GetAddress("SetAttackKey");
    auto call = reinterpret_cast<void(__thiscall*)(void*, bool)>(fnAddr);
    call(this, enable);
    return true;
}

void Player::DismountHorse()
{
    G::game->GetNetworkMgr()->SendChatPacket("/unmount", EChatType::CHAT_TYPE_COMMAND);
}

void Player::MountHorse()
{
    G::game->GetNetworkMgr()->SendChatPacket("/ride", EChatType::CHAT_TYPE_COMMAND);
}

bool Player::IsMountingHorse()
{
    return *(bool*)((DWORD)this + 0x4);
}

bool Player::StopWalking()
{
    static const auto fnAddr = G::memory->GetAddress("StopWalking");
    auto call = reinterpret_cast<bool(__thiscall*)(void*, int)>(fnAddr);
    return call(this, 0);
}

bool Player::StartWalking()
{
    static const auto fnAddr = G::memory->GetAddress("StartWalking");
    auto call = reinterpret_cast<bool(__thiscall*)(void*)>(fnAddr);
    return call(this);
}

bool Player::GetPickingPosition(Vector* pos)
{
    static auto fnAddr = G::memory->GetAddress("GetPickingPosition");
    static auto call = reinterpret_cast<bool(__stdcall*)(Vector*)>(fnAddr);
    return call(pos);
}

void Player::SetRotation(float rotation)
{
    *(float*)(DWORD(this) + 0x208) = rotation;
}

bool Player::ShortBlinkTo(const Vector& dst, bool check_collision, std::function<void(const Vector& point)> callback, bool bow_mode, bool go_back)
{
    auto* network = G::game->GetNetworkMgr();
    if (!network) return false;
    
    Vector src; GetPixelPosition(&src);
    float dist = GetDistanceTo(dst);
    const auto step_delta = IsMountingHorse() ? 1999.0f : 399.0f; //Max values dumped from Nerwia2
	const float rotation = GetRotation();
    if (dist > step_delta || (bow_mode && dist > GetBowRange()))
    {
        std::vector<Vector> path;
        if (Math::CalcDeltaPoints(src, dst, step_delta, path, bow_mode ? GetBowRange() : -1.f))
        {
            //Check if we can blink back safely without risking of getting rubberbanded back
            //2 * path points + 2 for callback
            if (network->GetAvailablePackets() < (path.size() * 2) + 2)
                return false;

            //Check for collisions on path (one way only)
            if (check_collision)
            {
                for (const auto& point : path)
                {
                    if (G::game->GetBackground()->IsBlockedCoord(point))
                        return false;
                }
            }
            
            Vector last_point = src;
			for (const auto& point : path)
            {
                network->SendCharacterStatePacket(point, rotation - 180.0f, 0, 0);
                last_point = point;
            }
            if (!bow_mode)
            {
                network->SendCharacterStatePacket(dst, rotation - 180.0f, 0, 0);
                last_point = dst;
            }
            
            callback(last_point);

            if (go_back)
            {
                for (auto it = path.rbegin(); it != path.rend(); ++it)
                {
                    network->SendCharacterStatePacket(*it, rotation, 0, 0);
                }
                network->SendCharacterStatePacket(src, rotation, 0, 0);
            }

            return true;
        }
    }

    if (network->GetAvailablePackets() < 3)
        return false;

    if (check_collision && G::game->GetBackground()->IsBlockedCoord(dst))
		return false;

    if (!bow_mode)
        network->SendCharacterStatePacket(dst, rotation - 180.0f, 0, 0);

    callback(dst);
    
    if (!bow_mode)
	    network->SendCharacterStatePacket(src, rotation, 0, 0);

    return true;
}

bool Player::IsBowMode()
{
    static const auto fnAddr = G::memory->GetAddress("IsBowMode");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*)>(fnAddr);
    return call(this);
}

float Player::GetBowRange()
{
    static const auto fnAddr = G::memory->GetAddress("GetBowRange");
    static const auto call = reinterpret_cast<float(__thiscall*)(void*)>(fnAddr);
    return call(this);
}

void Player::SetTarget(VID vid)
{
    *(DWORD*)(this + 0x46CA) = vid;
}

VID Player::GetTarget()
{
    return *(VID*)(this + 0x46CA);
}
