#pragma once
class CGraphicImageTexture;
struct MapInfo;

class Background
{
public:
	MapInfo*				GlobalPosToMapInfo(DWORD dwGlobalX, DWORD dwGlobalY);
	const char*				GetCurrentMapName();
	void					GlobalToLocalPosition(LONG& x, LONG& y);
	void					LocalToGlobalPosition(LONG& x, LONG& y);
	float					GetTerrainHeight(const Vector& pos);
	bool					IsBlockedCoord(const Vector& pos);
	bool					IsBlockedCoord(int x, int y);
	bool					IsAttrOn(int x, int y, BYTE flag);

	bool					GetTerrainIndexFromWorldPos(const Vector& pos, BYTE* terrainIdx);
	bool					GetCurTerrainIndex(BYTE* terrainIdx);
	IDirect3DTexture8*		GetMinimapTerrainTexture(const size_t idx);
	IDirect3DTexture8*		GetCurMinimapTerrainTexture();
	IDirect3DTexture8**		GetMinimapTerrainArray();

	CGraphicImageTexture*	GetResourcePointer(const char* apth);
	IDirect3DTexture8*		GetItemTexture(const char* name);
	IDirect3DTexture8*		GetD3DTexture(const char* name); //with force reload if needed

	void					ReloadResource(void* pResource);

	void					SetViewDistance(int num, float dist);

private:
	bool					_GetTerrainNumFromCoord(int thisPtr, WORD x, WORD y, BYTE* outTerrainNum);
	bool					_GetTerrainPointer(DWORD* thisPtr, BYTE terrainNum, DWORD* OutTerrainPointer);
	bool					_IsAttrOn(void* thisPtr, WORD x, WORD y, BYTE flag);
};

class CGraphicImageTexture
{
public:
	IDirect3DTexture8*		GetTexture();
	uint32_t				GetWidth();
	uint32_t				GetHeight();
	Vector2D				GetSize();
	const char*				GetPath();

private:
	char					_pad[12];
	char*					name;
	char					pad_0010[44];
	IDirect3DTexture8*		texture;
	char					pad_0011[40];
	uint32_t				width;
	uint32_t				height;

};

struct MapInfo
{
	char		name_pad[24];
	DWORD		m_dwBaseX;
	DWORD		m_dwBaseY;
	DWORD		m_dwSizeX;
	DWORD		m_dwSizeY;
	DWORD		m_dwEndX;
	DWORD		m_dwEndY;
};
