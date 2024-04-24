#pragma once
#define EQ_TOTAL_SLOTS_PER_CARD 9 * 5
#define EQ_TOTAL_CARDS 4
#define EQ_TOTAL_SLOTS EQ_TOTAL_SLOTS_PER_CARD * EQ_TOTAL_CARDS

enum EItemType;
enum EItemSubType;
typedef struct SItemPos TItemPos;

class ItemData
{
public:
	const wchar_t*	GetNameW();
	const char*		GetName();
	const char*		GetDescryption();
	const char*		GetIconPath();
	const char*		GetModelPath();
	EItemType		GetType();
	EItemSubType	GetSubType();
	int				GetRefineLevel();
};

class GroundItemData
{
public:
	Vector			GetPixelPosition();
	VNUM			GetVirtualNumber();
	void			GetBoundBox(Vector* min, Vector* max);
	void			GetBoundBoxTransformed(Vector* min, Vector* max);
	float			GetRadius();
	const char*		GetOwnerName();
	float			GetDistanceTo(const Vector& pos);
	bool			CanBePickedUp();

private:
	void*			vtable;
	VID				dwVirtualNumber;
	Vector			v3EndPosition;
	Vector			v3RotationAxis;
	Quaternion		qEnd;
	Vector			v3Center;
	bool			m_bUpdated;
	float			m_fLastLocalTime;
	float			m_fLocalTime;
	float			m_fDelay;
	float			m_fSecondElapsed;
	float			m_fAverageSecondElapsed;
	float			m_fRadius;
	Vector			m_v3Center;	//0x40
	Vector			m_v3Min, m_v3Max;
};

class Item
{
public:
	int				GetItemSlot(DWORD vNum, BYTE storageType);
	int				GetItemSlot(const std::string& name, BYTE storageType);
	ItemData*		GetItemData(WORD slot, BYTE storageType = 1);
	VID GetItemVID		(WORD slot, BYTE storageType = 1);

private:
	VID				__GetItemVID(const TItemPos& item_pos);
};

enum EItemType
{
	ITEM_TYPE_NONE,					//0
	ITEM_TYPE_WEAPON,				//1
	ITEM_TYPE_ARMOR,				//2
	ITEM_TYPE_USE,					//3
	ITEM_TYPE_AUTOUSE,				//4
	ITEM_TYPE_MATERIAL,				//5
	ITEM_TYPE_SPECIAL,				//6 
	ITEM_TYPE_TOOL,					//7
	ITEM_TYPE_LOTTERY,				//8
	ITEM_TYPE_ELK,					//9
	ITEM_TYPE_METIN,				//10
	ITEM_TYPE_CONTAINER,			//11
	ITEM_TYPE_FISH,					//12
	ITEM_TYPE_ROD,					//13
	ITEM_TYPE_RESOURCE,				//14
	ITEM_TYPE_CAMPFIRE,				//15
	ITEM_TYPE_UNIQUE,				//16
	ITEM_TYPE_SKILLBOOK,			//17
	ITEM_TYPE_QUEST,				//18
	ITEM_TYPE_POLYMORPH,			//19
	ITEM_TYPE_TREASURE_BOX,			//20
	ITEM_TYPE_TREASURE_KEY,			//21
	ITEM_TYPE_SKILLFORGET,			//22
	ITEM_TYPE_GIFTBOX,				//23
	ITEM_TYPE_PICK,					//24
	ITEM_TYPE_HAIR,					//25
	ITEM_TYPE_TOTEM,				//26
	ITEM_TYPE_BLEND,				//27
	ITEM_TYPE_COSTUME,				//28
	ITEM_TYPE_DS,					//29
	ITEM_TYPE_SPECIAL_DS,			//30
	ITEM_TYPE_EXTRACT,				//31
	ITEM_TYPE_SECONDARY_COIN,		//32
	ITEM_TYPE_RING,					//33
	ITEM_TYPE_BELT,					//34 

	ITEM_TYPE_MAX_NUM,
};

#pragma pack (push, 1)
#define WORD_MAX 0xffff

typedef struct SItemPos
{
	BYTE window_type;
	WORD cell;
	SItemPos()
	{
		window_type = 1;//INVENTORY; 4 = MALL
		cell = WORD_MAX;
	}
	SItemPos(BYTE _window_type, WORD _cell)
	{
		window_type = _window_type;
		cell = _cell;
	}

	bool operator==(const struct SItemPos& rhs) const
	{
		return (window_type == rhs.window_type) && (cell == rhs.cell);
	}

	bool operator<(const struct SItemPos& rhs) const
	{
		return (window_type < rhs.window_type) || ((window_type == rhs.window_type) && (cell < rhs.cell));
	}
} TItemPos;
#pragma pack(pop)