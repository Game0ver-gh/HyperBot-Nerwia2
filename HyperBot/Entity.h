#pragma once
#include "Item.h"
#include "Skill.h"
#undef GetJob

enum EntityType;
enum RaceType;
enum EmpireType;

class Entity
{
public:
	float					GetRotationTo(Entity* pEntity);
	float					GetRotationTo(const Vector& pos);
	float					GetDistanceTo(Entity* pEntity);
	float					GetDistanceTo(const Vector& pos);
	bool					IsCloseToPosition(const Vector& dstPos, float approx);
	bool					HasEffect(UINT effect);
	bool					IsSelectedTarget();
	bool					IsMonster();
	bool					IsBoss();
	bool					IsEnemy();
	bool					IsPC();
	bool					IsNPC();
	bool					IsStone();
	bool					IsAlive();
	bool					IsStuned();	//Special entity state before death
	bool					IsWalking();
	bool					IsBuilding();
	bool					IsDoor();
	bool					IsPolyPC();
	EntityType				GetType();
	void					GetPixelPosition(Vector* position);
	RaceType				GetRace();
	int 					GetJob();
	const char*				GetName();
	VNUM					GetVirtualNumber();
	VID						GetVID();
	float					GetRotation();
	DWORD					GetLevel();
	EmpireType				GetEmpireID();
	void					GetBoundBox(Vector* min, Vector* max);
	void					GetBoundBoxTransformed(Vector* min, Vector* max);
	bool					IsGameMaster();
	bool					IsAttacking();
	bool					IsSuraRace();
	bool					IsWarriorRace();
	bool					IsAssassinRace();
	bool					IsShamanRace();
	bool					IsOfflineShop();
	bool					IsPartyMember(const std::string& name);
	bool					IsPartyMember(VID vid);
	void					SCRIPT_SetPixelPosition(const Vector& pos);

//protected:
	std::map<DWORD, DWORD>& GetEffectMap();
};

class Player : public Entity, public Item, public Skill
{
public:
	void					LookAtDestPixelPosition(const Vector& pos);
	bool					MoveToDestPixelPosition(const Vector& pos);
	bool					MoveFromDestPosition(const Vector& pos, float angle, float dist);
	bool					AttackToDestInstance(Entity* pEntity);
	bool					SetAttackKey(bool enable);
	void					DismountHorse();
	void					MountHorse();
	bool					IsMountingHorse();
	bool					StopWalking();
	bool					StartWalking();
	bool					GetPickingPosition(Vector* pos);			//Visual only?
	void					SetRotation(float rotation);				//With -180* applied
	bool					ShortBlinkTo(const Vector& dst, bool check_collision, std::function<void(const Vector& point)> callback, bool bow_mode = false, bool go_back = true);
	bool					IsBowMode();
	float					GetBowRange();
	void					SetTarget(VID vid);
	VID						GetTarget();
};

enum EClickEvent
{
	ON_CLICK_EVENT_NONE = 0,
	ON_CLICK_EVENT_BATTLE = 1,
	ON_CLICK_EVENT_SHOP = 2,
	ON_CLICK_EVENT_TALK = 3,
	ON_CLICK_EVENT_VEHICLE = 4,

	ON_CLICK_EVENT_MAX_NUM,
};
enum EMobEnchants
{
	MOB_ENCHANT_CURSE,
	MOB_ENCHANT_SLOW,
	MOB_ENCHANT_POISON,
	MOB_ENCHANT_STUN,
	MOB_ENCHANT_CRITICAL,
	MOB_ENCHANT_PENETRATE,
	MOB_ENCHANTS_MAX_NUM
};
enum EMobResists
{
	MOB_RESIST_SWORD,
	MOB_RESIST_TWOHAND,
	MOB_RESIST_DAGGER,
	MOB_RESIST_BELL,
	MOB_RESIST_FAN,
	MOB_RESIST_BOW,
	MOB_RESIST_FIRE,
	MOB_RESIST_ELECT,
	MOB_RESIST_MAGIC,
	MOB_RESIST_WIND,
	MOB_RESIST_POISON,
	MOB_RESISTS_MAX_NUM
};

#define MOB_ATTRIBUTE_MAX_NUM	12
#define MOB_SKILL_MAX_NUM		5
#define CHARACTER_NAME_MAX_LEN	24

typedef struct SPartyMemberInfo
{
	DWORD dwVID;
	DWORD dwPID;
	std::string strName;
	BYTE byState;
	BYTE byHPPercentage;
	short sAffects[7];
} TPartyMemberInfo;

#pragma pack(push)
#pragma pack(1)
typedef struct SMobSkillLevel
{
	DWORD       dwVnum;
	BYTE        bLevel;
} TMobSkillLevel;

typedef struct SMobTable
{
	DWORD       dwVnum;
	char        szName[CHARACTER_NAME_MAX_LEN + 1];
	char        szLocaleName[CHARACTER_NAME_MAX_LEN + 1];

	BYTE        bType;                  // Monster, NPC
	BYTE        bRank;                  // PAWN, KNIGHT, KING
	BYTE        bBattleType;            // MELEE, etc..
	BYTE        bLevel;                 // Level
	BYTE        bSize;

	DWORD       dwGoldMin;
	DWORD       dwGoldMax;
	DWORD       dwExp;
	DWORD       dwMaxHP;
	BYTE        bRegenCycle;
	BYTE        bRegenPercent;
	WORD        wDef;

	DWORD       dwAIFlag;
	DWORD       dwRaceFlag;
	DWORD       dwImmuneFlag;

	BYTE        bStr, bDex, bCon, bInt;
	DWORD       dwDamageRange[2];

	short       sAttackSpeed;
	short       sMovingSpeed;
	BYTE        bAggresiveHPPct;
	WORD        wAggressiveSight;
	WORD        wAttackRange;

	char        cEnchants[MOB_ENCHANTS_MAX_NUM];
	char        cResists[MOB_RESISTS_MAX_NUM];

	DWORD       dwResurrectionVnum;
	DWORD       dwDropItemVnum;

	BYTE        bMountCapacity;
	BYTE        bOnClickType;

	BYTE        bEmpire;
	char        szFolder[64 + 1];
	float       fDamMultiply;
	DWORD       dwSummonVnum;
	DWORD       dwDrainSP;
	DWORD		dwMonsterColor;
	DWORD       dwPolymorphItemVnum;

	TMobSkillLevel	Skills[MOB_SKILL_MAX_NUM];

	BYTE		bBerserkPoint;
	BYTE		bStoneSkinPoint;
	BYTE		bGodSpeedPoint;
	BYTE		bDeathBlowPoint;
	BYTE		bRevivePoint;
} TMobTable;
#pragma pack(pop)

enum RaceType
{
	MAIN_RACE_WARRIOR_M,
	MAIN_RACE_ASSASSIN_W,
	MAIN_RACE_SURA_M,
	MAIN_RACE_SHAMAN_W,
	MAIN_RACE_WARRIOR_W,
	MAIN_RACE_ASSASSIN_M,
	MAIN_RACE_SURA_W,
	MAIN_RACE_SHAMAN_M,
	MAIN_RACE_MAX_NUM,
};

static constexpr const char* race_type_str[] =
{
	"Warrior",
	"Ninja",
	"Sura",
	"Shaman",
	"Warrior",
	"Ninja",
	"Sura",
	"Shaman"
};

enum EntityType
{
	TYPE_ENEMY,			// mobs (red name)
	TYPE_NPC,			// npc (green name)
	TYPE_STONE,			// metins, stones, statues, etc (yellow name)
	TYPE_WARP,			// warps
	TYPE_DOOR,			// doors/gates
	TYPE_BUILDING,		// buildings 
	TYPE_PC,			// players (yellow name)
	TYPE_POLY,			// polymorphed players
	TYPE_HORSE,			// horses
	TYPE_GOTO,			// random coords (for quest i guess)

	TYPE_OBJECT, // Only For Client
};

enum
{
	WEAPON_DUALHAND,
	WEAPON_ONEHAND,
	WEAPON_TWOHAND,
	WEAPON_NUM,
};

enum
{
	AFFECT_YMIR,
	AFFECT_INVISIBILITY,
	AFFECT_SPAWN,

	AFFECT_POISON,
	AFFECT_SLOW,
	AFFECT_STUN,

	AFFECT_DUNGEON_READY,
	AFFECT_SHOW_ALWAYS,				// AFFECT_DUNGEON_UNIQUE

	AFFECT_BUILDING_CONSTRUCTION_SMALL,
	AFFECT_BUILDING_CONSTRUCTION_LARGE,
	AFFECT_BUILDING_UPGRADE,

	AFFECT_MOV_SPEED_POTION,		// 11
	AFFECT_ATT_SPEED_POTION,		// 12

	AFFECT_FISH_MIND,				// 13

	AFFECT_JEONGWI,					// 14
	AFFECT_GEOMGYEONG,				// 15
	AFFECT_CHEONGEUN,				// 16
	AFFECT_GYEONGGONG,				// 17
	AFFECT_EUNHYEONG,				// 18
	AFFECT_GWIGEOM,					// 19
	AFFECT_GONGPO,					// 20
	AFFECT_JUMAGAP,					// 21
	AFFECT_HOSIN,					// 22
	AFFECT_BOHO,					// 23
	AFFECT_KWAESOK,					// 24
	AFFECT_HEUKSIN,					// 25
	AFFECT_MUYEONG,					// 26
	AFFECT_REVIVE_INVISIBILITY,		// 27
	AFFECT_FIRE,					// 28
	AFFECT_GICHEON,					// 29
	AFFECT_JEUNGRYEOK,				// 30
	AFFECT_DASH,					// 31
	AFFECT_PABEOP,					// 32
	AFFECT_FALLEN_CHEONGEUN,		// 33
	AFFECT_POLYMORPH,				// 34
	AFFECT_WAR_FLAG1,				// 35
	AFFECT_WAR_FLAG2,				// 36
	AFFECT_WAR_FLAG3,				// 37
	AFFECT_CHINA_FIREWORK,			// 38
	AFFECT_PREMIUM_SILVER,
	AFFECT_PREMIUM_GOLD,
	AFFECT_RAMADAN_RING,			// 41

	AFFECT_NUM = 64,

	AFFECT_HWAYEOM = AFFECT_GEOMGYEONG,
};

enum
{
	EMOTICON_EXCLAMATION = 1,
	EMOTICON_FISH = 11,
	EMOTICON_NUM = 128,

	TITLE_NUM = 9,
	TITLE_NONE = 4,
};

enum EmpireType
{
	EMPIRE_NONE,
	EMPIRE_A,
	EMPIRE_B,
	EMPIRE_C,
	EMPIRE_NUM,
};

static constexpr const char* empire_str[] = 
{
	"NONE",
	"Shinso",
	"Chunjo",
	"Jinno",
};

enum
{
	EFFECT_REFINED_NONE,

	EFFECT_SWORD_REFINED7,
	EFFECT_SWORD_REFINED8,
	EFFECT_SWORD_REFINED9,

	EFFECT_BOW_REFINED7,
	EFFECT_BOW_REFINED8,
	EFFECT_BOW_REFINED9,

	EFFECT_FANBELL_REFINED7,
	EFFECT_FANBELL_REFINED8,
	EFFECT_FANBELL_REFINED9,

	EFFECT_SMALLSWORD_REFINED7,
	EFFECT_SMALLSWORD_REFINED8,
	EFFECT_SMALLSWORD_REFINED9,

	EFFECT_SMALLSWORD_REFINED7_LEFT,
	EFFECT_SMALLSWORD_REFINED8_LEFT,
	EFFECT_SMALLSWORD_REFINED9_LEFT,

	EFFECT_BODYARMOR_REFINED7,
	EFFECT_BODYARMOR_REFINED8,
	EFFECT_BODYARMOR_REFINED9,

	EFFECT_BODYARMOR_SPECIAL,
	EFFECT_BODYARMOR_SPECIAL2,

	EFFECT_REFINED_NUM,
};

enum 
{
	EFFECT_DUST,
	EFFECT_STUN,
	EFFECT_HIT,
	EFFECT_FLAME_ATTACK,
	EFFECT_FLAME_HIT,
	EFFECT_FLAME_ATTACH,
	EFFECT_ELECTRIC_ATTACK,
	EFFECT_ELECTRIC_HIT,
	EFFECT_ELECTRIC_ATTACH,
	EFFECT_SPAWN_APPEAR,
	EFFECT_SPAWN_DISAPPEAR,
	EFFECT_LEVELUP,
	EFFECT_SKILLUP,
	EFFECT_HPUP_RED,
	EFFECT_SPUP_BLUE,
	EFFECT_SPEEDUP_GREEN,
	EFFECT_DXUP_PURPLE,
	EFFECT_CRITICAL,
	EFFECT_PENETRATE,
	EFFECT_BLOCK,
	EFFECT_DODGE,
	EFFECT_FIRECRACKER,
	EFFECT_SPIN_TOP,
	EFFECT_WEAPON,
	EFFECT_WEAPON_END = EFFECT_WEAPON + WEAPON_NUM,
	EFFECT_AFFECT,
	EFFECT_AFFECT_GYEONGGONG = EFFECT_AFFECT + AFFECT_GYEONGGONG,
	EFFECT_AFFECT_KWAESOK = EFFECT_AFFECT + AFFECT_KWAESOK,
	EFFECT_AFFECT_END = EFFECT_AFFECT + AFFECT_NUM,
	EFFECT_EMOTICON,
	EFFECT_EMOTICON_END = EFFECT_EMOTICON + EMOTICON_NUM,
	EFFECT_SELECT,
	EFFECT_TARGET,
	EFFECT_EMPIRE,
	EFFECT_EMPIRE_END = EFFECT_EMPIRE + EMPIRE_NUM,
	EFFECT_HORSE_DUST,
	EFFECT_REFINED,
	EFFECT_REFINED_END = EFFECT_REFINED + EFFECT_REFINED_NUM,
	EFFECT_DAMAGE_TARGET,
	EFFECT_DAMAGE_NOT_TARGET,
	EFFECT_DAMAGE_SELFDAMAGE,
	EFFECT_DAMAGE_SELFDAMAGE2,
	EFFECT_DAMAGE_POISON,
	EFFECT_DAMAGE_MISS,
	EFFECT_DAMAGE_TARGETMISS,
	EFFECT_DAMAGE_CRITICAL,
	EFFECT_SUCCESS,
	EFFECT_FAIL,
	EFFECT_FR_SUCCESS,
	EFFECT_LEVELUP_ON_14_FOR_GERMANY,
	EFFECT_LEVELUP_UNDER_15_FOR_GERMANY,
	EFFECT_PERCENT_DAMAGE1,
	EFFECT_PERCENT_DAMAGE2,
	EFFECT_PERCENT_DAMAGE3,
	EFFECT_AUTO_HPUP,
	EFFECT_AUTO_SPUP,
	EFFECT_RAMADAN_RING_EQUIP,
	EFFECT_HALLOWEEN_CANDY_EQUIP,
	EFFECT_HAPPINESS_RING_EQUIP,
	EFFECT_LOVE_PENDANT_EQUIP,
	EFFECT_TEMP,
	EFFECT_NUM,
};
