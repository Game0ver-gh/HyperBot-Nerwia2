#include "pch.h"
#include "Background.h"

MapInfo* Background::GlobalPosToMapInfo(DWORD dwGlobalX, DWORD dwGlobalY)
{
    typedef MapInfo* (__thiscall* MapInfo)(void*, int, int);
    static auto call = reinterpret_cast<MapInfo>(G::memory->GetAddress("GlobalPosToMapInfo"));
    if (call) return call((uintptr_t*)this, dwGlobalX, dwGlobalY);
    return nullptr;
}

const char* Background::GetCurrentMapName()
{
    DWORD* result; // eax@1

    result = (DWORD*)((uintptr_t)this + 0x14F8);
    if (*(DWORD*)((uintptr_t)this + 0x150C) >= 0x10u)
        result = (DWORD*)*result;
    return (const char*)result;
}

void Background::GlobalToLocalPosition(LONG& x, LONG& y)
{
    x -= *(DWORD*)(this + 0x560);
    y -= *(DWORD*)(this + 0x561);
}

void Background::LocalToGlobalPosition(LONG& x, LONG& y)
{
    x += *(DWORD*)(this + 0x560);
    y += *(DWORD*)(this + 0x561);
}

float Background::GetTerrainHeight(const Vector& pos)
{
	static auto addr = G::memory->GetAddress("GetTerrainHeight");
	static auto call = reinterpret_cast<float(__thiscall*)(void*, float, float)>(addr);
	return call(this, pos.x, pos.y);
}

bool Background::IsBlockedCoord(const Vector& pos)
{
    int tmp[2] = { static_cast<int>(std::ceilf(pos.x)), static_cast<int>(std::ceilf(pos.y)) };
    return IsBlockedCoord(tmp[0], tmp[1]);
}

bool Background::IsBlockedCoord(int x, int y)
{
    typedef bool(__stdcall* IsBlock)(int, int);
	static auto call = reinterpret_cast<IsBlock>(G::memory->GetAddress("IsBlockedCoord"));
    if (call) return call(x, y);
    return true;
}

bool Background::IsAttrOn(int x, int y, BYTE flag)
{
    int v4; // ebx@1
    DWORD* v5; // esi@1
    int v6; // edi@2
    char v7; // ST04_1@5
    bool result; // al@7
    DWORD v9; // [sp+Ch] [bp-8h]@6
    int v10; // [sp+10h] [bp-4h]@5

    auto addr = *(uintptr_t*)0x27E55A4;
    auto _this = *(uintptr_t*)addr;
    //auto pTerrain = *(uintptr_t*)addr2;

    v4 = x;
    v5 = (DWORD*)_this;
    if (x >= 0
        && (v6 = y, y >= 0)
        && x <= 25600 * *(WORD*)(_this + 112)
        && y <= 25600 * *(WORD*)(_this + 114)
        && (v10 = (unsigned __int16)(((signed int)((unsigned __int64)(1374389535 * x) >> 32) >> 13)
            + ((unsigned int)((unsigned __int64)(1374389535 * x) >> 32) >> 31)),
            v7 = (signed __int16)y / 25600,
            y = (signed __int16)y / 25600,
            _GetTerrainNumFromCoord(
                _this,
                ((signed int)((unsigned __int64)(1374389535 * x) >> 32) >> 13)
                + ((unsigned int)((unsigned __int64)(1374389535 * x) >> 32) >> 31),
                v7,
                (unsigned __int8*)&x))
        && _GetTerrainPointer(v5, x, &v9))
    {
        auto _this2 = (BYTE*)v9;

        result = _IsAttrOn(
            _this2,
            (v4 - 25600 * (unsigned __int16)v10) / 100,
            (v6 - 25600 * (unsigned __int16)y) / 100,
            flag);
    }
    else
    {
        result = 0;
    }
    return result;
}

bool Background::GetTerrainIndexFromWorldPos(const Vector& pos, BYTE* terrainIdx)
{
    int ix = int(pos.x / 100), iy = int(pos.y / 100);

    WORD wTerrainNumX = ix / (25600);
    WORD wTerrainNumY = iy / (25600);

    auto CTerrain = 0x0;
    if (auto addr = *(uintptr_t*)0x27E55A4; addr)
        if (auto _this = *(uintptr_t*)addr; _this)
            CTerrain = _this;

    return _GetTerrainNumFromCoord(CTerrain, wTerrainNumX, wTerrainNumY, terrainIdx);
}

bool Background::GetCurTerrainIndex(BYTE* terrainIdx)
{
    auto pLocal = G::game->GetLocalPlayer();
    if (!pLocal) return false;

    Vector pos; pLocal->GetPixelPosition(&pos);
    return GetTerrainIndexFromWorldPos(pos, terrainIdx);;
}

IDirect3DTexture8* Background::GetMinimapTerrainTexture(const size_t idx)
{
    if (idx > 9 || idx < 0)
        return nullptr;

    return GetMinimapTerrainArray()[idx];
}

IDirect3DTexture8* Background::GetCurMinimapTerrainTexture()
{
    auto pLocal = G::game->GetLocalPlayer();
    if (!pLocal) return nullptr;

    Vector pos; pLocal->GetPixelPosition(&pos);

    BYTE terrainNum = -1;
    if (!GetTerrainIndexFromWorldPos(pos, &terrainNum))
        return nullptr;

    return GetMinimapTerrainTexture(terrainNum);
}

IDirect3DTexture8** Background::GetMinimapTerrainArray()
{
	static auto addr = G::memory->GetAddress("CPythonMinimap");
    return (IDirect3DTexture8**)(addr + 0x30);
}

CGraphicImageTexture* Background::GetResourcePointer(const char* path)
{
    static auto _this = G::memory->GetPointer("GetResourcePointer");
    typedef void* (__thiscall* tGetResourcePointer)(void*, const char*);
	static auto fun = reinterpret_cast<tGetResourcePointer>(G::memory->GetAddress("GetResourcePointer"));
    return (CGraphicImageTexture*)fun((void*)_this, path);
}

IDirect3DTexture8* Background::GetItemTexture(const char* name)
{
    auto item = G::game->GetItem(name).second;
    if (!item) return nullptr;

    auto imageTex = GetResourcePointer(item->GetIconPath());
    if (imageTex) return imageTex->GetTexture();
    return nullptr;
}

IDirect3DTexture8* Background::GetD3DTexture(const char* name)
{
    auto pRes = G::game->GetBackground()->GetResourcePointer(name);
    if (!pRes) return nullptr;

    if (pRes->GetTexture()) return pRes->GetTexture();

    G::game->GetBackground()->ReloadResource(pRes);
    return pRes->GetTexture();
}

void Background::ReloadResource(void* pResource)
{
    typedef void(__thiscall* tReload)(void*);
    static auto call = reinterpret_cast<tReload>(G::memory->GetAddress("ReloadResource"));
    if (call) call(pResource);
}

void Background::SetViewDistance(int num, float dist)
{
    typedef void(__thiscall* SetViewDist_t)(void*, int, float);
    auto call = reinterpret_cast<SetViewDist_t>(G::memory->GetAddress("SetViewDistance"));
	if (call) call(this, num, dist);
}

bool Background::_GetTerrainNumFromCoord(int thisPtr, WORD x, WORD y, BYTE* outTerrainNum)
{
    unsigned __int8 v4; // dl@1

    v4 = x + 3 * (y - *(BYTE*)(thisPtr + 118)) - *(BYTE*)(thisPtr + 116) + 4;
    *outTerrainNum = v4;
    return v4 <= 9u;
}

bool Background::_GetTerrainPointer(DWORD* thisPtr, BYTE terrainNum, DWORD* OutTerrainPointer)
{
    DWORD* v3; // eax@2
    int v5; // ecx@4

    if (terrainNum >= 9u)
    {
        v3 = OutTerrainPointer;
    LABEL_3:
        *v3 = 0;
        return 0;
    }
    v5 = thisPtr[terrainNum + 16];
    v3 = OutTerrainPointer;
    if (!v5)
        goto LABEL_3;
    *OutTerrainPointer = v5;
    return 1;
}

bool Background::_IsAttrOn(void* thisPtr, WORD x, WORD y, BYTE flag)
{
    if (x >= 256 || y >= 256)
        return false;

    BYTE* _this = (BYTE*)thisPtr;
    bool result = false;
    BYTE terrainFlag = *(&_this[0x100 * y + 0x18E1A] + x);
    if (flag >= 16u)
        result = ((flag ^ terrainFlag) & 0xF0) == 0;
    else
        result = (terrainFlag & flag) != 0;
    return result;
}

IDirect3DTexture8* CGraphicImageTexture::GetTexture()
{
    return texture;
}

uint32_t CGraphicImageTexture::GetWidth()
{
    return width;
}

uint32_t CGraphicImageTexture::GetHeight()
{
	return height;
}

Vector2D CGraphicImageTexture::GetSize()
{
    return Vector2D(width, height);
}

const char* CGraphicImageTexture::GetPath()
{
	return name;
}
