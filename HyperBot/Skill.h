#pragma once

enum ESkillIndex;
struct SkillData;

class Skill
{
public:
    SkillData&                      GetSkillData(ESkillIndex skill_index);
    std::map<VID, SkillData>&       GetSkillDataMap();
    const std::vector<SkillData>&   GetSelfBuffSkillList();
    const std::vector<SkillData>&   GetTargetBuffSkillList();
    const std::vector<SkillData>&   GetAttackSkillList();

    const std::vector<SkillData*>&  GetSkillList(DWORD dwRace);
    

    bool					        IsSkillCoolTime(ESkillIndex skill_index);	//for NonToggleable skills
    bool					        IsSkillActive(ESkillIndex skill_index);		//for Toggleable skills
    bool					        IsSkillReady(ESkillIndex skill_index);
    ESkillIndex						GetSkillIndex(DWORD dwSlotIndex);
    DWORD                           GetSkillSlot(ESkillIndex skill_index);

    bool					        UseSkill(DWORD dwSlotIndex);
    bool                            IsSkillUsedByRace(ESkillIndex skill_index, DWORD dwRace);

    //Broken
    bool                            IsSkillUsedByJob(ESkillIndex skill_index, DWORD dwJob);
};

enum ESkillIndex
{
    TRZYSTRONNE_CIECIE = 1,
    WIR_MIECZA = 2,
    BERSERK = 3,
    AURA_MIECZA = 4,
    SZARZA = 5,
    WOLA_ZYCIA = 6,
    DUCHOWE_UDERZENIE = 16,
    WALNIECIE = 17,
    TAPNIECIE = 18,
    SILNE_CIALO = 19,
    UDERZENIE_MIECZA = 20,
    KRAG_MIECZY = 21,

    ZASADZKA = 31,
    SZYBKI_ATAK = 32,
    WIRUJACY_SZTYLET = 33,
    KRYCIE_SIE = 34,
    TRUJACA_CHMURA = 35,
    WOLNO_DZIALAJACA_TRUCIZNA = 36,
    POWTARZALNY_STRZAL = 46,
    DESZCZ_STRZAL = 47,
    OGNISTA_STRZALA = 48,
    BEZSZELESTNY_CHOD = 49,
    TRUJACA_STRZALA = 50,
    ISKRZACE_UDERZENIE = 51,

    UDERZENIE_PALCEM = 61,
    SMOCZY_WIR = 62,
    CZAROWANE_OSTRZE = 63,
    STRACH = 64,
    CZAROWANA_ZBROJA = 65,
    ROZPROSZENIE_MAGII = 66,
    MROCZNE_UDERZENIE = 76,
    OGNISTE_UDERZENIE = 77,
    OGNISTY_DUCH = 78,
    MROCZNA_OCHRONA = 79,
    DUCHOWY_CIOS = 80,
    MROCZNA_SFERA = 81,

    LATAJACY_TALIZMAN = 91,
    STRZELAJACY_SMOK = 92,
    SMOCZY_SKOWYT = 93,
    BLOGOSLAWIENSTWO = 94,
    ODBICIE = 95,
    POMOC_SMOKA = 96,
    RZUT_PIORUNEM = 106,
    PRZYWOLANIE_BLYSKAWICY = 107,
    BURZOWY_SZPON = 108,
    LECZENIE = 109,
    ZWINNOSC_ = 110,
    ZWIEKSZENIE_ATAKU = 111,

    DOWODZENIE = 121,
    KOMBINACJA = 122,
    LOWIENIE = 123,
    GORNICTWO = 124,
    TWORZENIE_PRZEDMIOTU = 125,
    JEZYK_SHINSOO = 126,
    JEZYK_CHUNJO = 127,
    JEZYK_JINNO = 128,
    POLIMORFIA = 129,
    JEZDZIECTWO = 130,
    PRZYWOLAJ_KONIA = 131,
    AUTO_ATAK = 132,
    WIEDZA_O_POTWORACH = 133,
    POGROMCA = 134,
    CIECIE_Z_SIODLA = 137,
    STAPNIECIE_KONIA = 138,
    FALA_MOCY = 139,
    GRAD_STRZAL = 140,
    SMOCZE_OCZY = 151,
    KREW_BOGA_SMOKOW = 152,
    BLOGOSLAWIENSTWO_BOGA_SMOKOW = 153,
    SWIETA_ZBROJA = 154,
    AKCELERACJA = 155,
    FURIA_BOGA_SMOKOW = 156,
    MAGICZNA_POMOC = 157,
    RIPOSTA_WIR_MIECZA = 221,
    RIPOSTA_ZASADZKA = 222,
    RIPOSTA_UDERZENIE_PALCEM = 223,
    RIPOSTA_STRZELAJACY_SMOK = 224,
    RIPOSTA_DUCHOWE_UDERZENIE = 225,
    RIPOSTA_OGNISTA_STRZALA = 226,
    RIPOSTA_MROCZNE_UDERZENIE = 227,
    RIPOSTA_PRZYWOLANIE_BLYSKAWICY = 228,
    RIPOSTA_WILCZY_DECH = 229,
    DOPALACZ_WIR_MIECZA = 236,
    DOPALACZ_ZASADZKA = 237,
    DOPALACZ_UDERZENIE_PALCEM = 238,
    DOPALACZ_STRZELAJACY_SMOK = 239,
    DOPALACZ_DUCHOWE_UDERZENIE = 240,
    DOPALACZ_OGNISTA_STRZALA = 241,
    DOPALACZ_MROCZNE_UDERZENIE = 242,
    DOPALACZ_PRZYWOLANIE_BLYSKAWICY = 243,
    DOPALACZ_WILCZY_DECH = 244,
    COUNT
};

enum
{
    SKILL_ATTRIBUTE_NEED_TARGET = (1 << 0),
    SKILL_ATTRIBUTE_TOGGLE = (1 << 1),
    SKILL_ATTRIBUTE_WEAPON_LIMITATION = (1 << 2),
    SKILL_ATTRIBUTE_MELEE_ATTACK = (1 << 3),
    SKILL_ATTRIBUTE_USE_HP = (1 << 4),
    SKILL_ATTRIBUTE_CAN_CHANGE_DIRECTION = (1 << 5),
    SKILL_ATTRIBUTE_STANDING_SKILL = (1 << 6),
    SKILL_ATTRIBUTE_ONLY_FOR_ALLIANCE = (1 << 7),
    SKILL_ATTRIBUTE_CAN_USE_FOR_ME = (1 << 8),
    SKILL_ATTRIBUTE_NEED_CORPSE = (1 << 9),
    SKILL_ATTRIBUTE_FAN_RANGE = (1 << 10),
    SKILL_ATTRIBUTE_CAN_USE_IF_NOT_ENOUGH = (1 << 11),
    SKILL_ATTRIBUTE_NEED_EMPTY_BOTTLE = (1 << 12),
    SKILL_ATTRIBUTE_NEED_POISON_BOTTLE = (1 << 13),
    SKILL_ATTRIBUTE_ATTACK_SKILL = (1 << 14),
    SKILL_ATTRIBUTE_TIME_INCREASE_SKILL = (1 << 15), 
    SKILL_ATTRIBUTE_CHARGE_ATTACK = (1 << 16),
    SKILL_ATTRIBUTE_PASSIVE = (1 << 17),
    SKILL_ATTRIBUTE_CANNOT_LEVEL_UP = (1 << 18),
    SKILL_ATTRIBUTE_ONLY_FOR_GUILD_WAR = (1 << 19),
    SKILL_ATTRIBUTE_MOVING_SKILL = (1 << 20),
    SKILL_ATTRIBUTE_HORSE_SKILL = (1 << 21),
    SKILL_ATTRIBUTE_CIRCLE_RANGE = (1 << 22),
    SKILL_ATTRIBUTE_SEARCH_TARGET = (1 << 23),
};

typedef struct SGradeData
{
    std::string strName;
    void* pImage;
    WORD wMotionIndex;
} TGradeData;

typedef struct SAffectData
{
    std::string strAffectDescription;
    std::string strAffectMinFormula;
    std::string strAffectMaxFormula;
} TAffectData;
typedef struct SAffectDataNew
{
    std::string strPointType;
    std::string strPointPoly;
} TAffectDataNew;
typedef struct SRequireStatData
{
    BYTE byPoint;
    BYTE byLevel;
} TRequireStatData;

struct SkillData
{
    static DWORD MELEE_SKILL_TARGET_RANGE;

    // Variable
    BYTE byType;
    DWORD dwSkillIndex;
    BYTE byMaxLevel;
    BYTE byLevelUpPoint;
    BYTE byLevelLimit;
    BOOL bNoMotion;

    std::string strName;
    std::string strIconFileName;
    std::string strDescription;
    std::string strMotionScriptFileName;

    std::vector<std::string> ConditionDataVector;
    std::vector<TAffectData> AffectDataVector;
    std::vector<TAffectDataNew> AffectDataNewVector;
    std::vector<TRequireStatData> RequireStatDataVector;
    std::string strCoolTimeFormula;
    std::string strTargetCountFormula;
    std::string strMotionLoopCountFormula;
    std::string strNeedSPFormula;
    std::string strContinuationSPFormula;
    std::string strDuration;

    DWORD dwSkillAttribute;
    DWORD dwNeedWeapon;
    DWORD dwTargetRange;
    WORD wMotionIndex;
    WORD wMotionIndexForMe; 

    BOOL isRequirement;
    std::string strRequireSkillName;
    BYTE byRequireSkillLevel;

    TGradeData GradeData[4];

    void* pImage;

    bool CanSelfBuff() const;
    bool CanTargetBuff() const;
    bool IsAttackSkill() const;
    const LPDIRECT3DTEXTURE8 GetTexture() const;
};