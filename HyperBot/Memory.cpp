#include "pch.h"
#include "Memory.h"
#define     EQN_EPS			 1e-9
#define	    IsZero(x)		 ((x) > -EQN_EPS && (x) < EQN_EPS)
#define		INRANGE(x,a,b)   (x >= a && x <= b)
#define		GET_BYTE( x )    (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))
#define		GET_BITS( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))


Memory::Memory()
{
	sig_total = 0; sig_found = 0;
	Clock timer;

	G::console->SetColor(Console::TextColor::YELLOW);
	G::console->WriteInfo("Mapping signatures...\n");

	timer.Begin();
	
	bool success = MapSignatures();

	timer.End();

	G::console->SetColor(Console::TextColor::YELLOW);
	G::console->WriteInfo("Found %d/%d(%d%%) addresses from signatures in %.1fs\n",
		sig_found, sig_total, (sig_found * 100) / sig_total, timer.GetElapsed());

	//Found at least 1 invalid function address
	if (!success)
	{
		for (const auto& fail : sig_failed)
			G::console->WriteError("Signature for %s doesn't match!\n", fail.c_str());
	}
}

DWORD Memory::FindSignature(const char* signature, int extra_opcode, int relative, const char* module)
{
	MODULEINFO modInfo;
	auto mod = GetModuleHandleA(module);
	GetModuleInformation(GetCurrentProcess(), mod, &modInfo, sizeof(MODULEINFO));
	DWORD startAddress = (DWORD)mod;
	DWORD endAddress = (startAddress + (DWORD)modInfo.SizeOfImage);
	const char* pat = signature;
	DWORD firstMatch = 0;

	for (DWORD pCur = startAddress; pCur < endAddress; pCur++) {
		if (!*pat)
		{
			return firstMatch;
		}
		if (*(BYTE*)pat == ('\?') || *(BYTE*)pCur == GET_BYTE(pat)) {
			if (!firstMatch) firstMatch = pCur;
			if (!pat[2]) //success
			{
				if (relative && extra_opcode)
					return (*(DWORD*)(firstMatch + extra_opcode) + firstMatch + relative);
				return firstMatch;
			}
			if (*(PWORD)pat == ('\?\?') || *(PBYTE)pat != ('\?')) pat += 3;
			else pat += 2;
		}
		else {
			pat = signature;
			firstMatch = 0;
		}
	}
	return 0x0;
}

std::vector<DWORD> Memory::FindSignatures(const char* signature, const char* module)
{
	MODULEINFO modInfo;
	auto mod = GetModuleHandleA(module);
	GetModuleInformation(GetCurrentProcess(), mod, &modInfo, sizeof(MODULEINFO));
	DWORD startAddress = (DWORD)mod;
	DWORD endAddress = (startAddress + (DWORD)modInfo.SizeOfImage);
	const char* pat = signature;
	DWORD firstMatch = 0;
	std::vector<DWORD> result;

	for (DWORD pCur = startAddress; pCur < endAddress; pCur++) {
		if (!*pat)
			return result;
		if (*(BYTE*)pat == ('\?') || *(BYTE*)pCur == GET_BYTE(pat)) {
			if (!firstMatch) firstMatch = pCur;
			if (!pat[2]) //success
			{
				result.push_back(firstMatch);
				pat = signature;
				pCur = firstMatch + 0x1;
			}
			if (*(PWORD)pat == ('\?\?') || *(PBYTE)pat != ('\?')) pat += 3;
			else pat += 2;
		}
		else {
			pat = signature;
			firstMatch = 0;
		}
	}
	return result;
}

DWORD Memory::GetAddress(const std::string& name)
{
	try
	{
		return address.at(name);
	}
	catch (std::out_of_range& e)
	{
		G::console->WriteError("%s('%s') - failed: %s!\n", __FUNCTION__, name.c_str(), e.what());
		throw MessageBoxA(0, e.what(), "HyperBOT", MB_OK);
	}
	return 0;
}

DWORD* Memory::GetPointer(const std::string& name)
{
	try
	{
		return pointer.at(name);
	}
	catch (std::out_of_range& e)
	{
		G::console->WriteError("%s('%s') - failed: %s!\n", __FUNCTION__, name.c_str());
		throw MessageBoxA(0, e.what(), "HyperBOT", MB_OK);
	}
	return nullptr;
}

bool Memory::MapSignatures()
{
	//Values / patches
	address["max_camera_dist"] = *(DWORD*)(Memory::FindSignature("F3 0F 11 0D ? ? ? ? 5D") + 0x4);
	address["min_fog_dist"] = *(DWORD*)(Memory::FindSignature("51 F3 0F 10 15 ? ? ? ?") + 0x5);

	//Interfaces
	pointer["CCameraManager"] = *(DWORD**)(Memory::FindSignature("8B 0D ? ? ? ? E8 ? ? ? ? 8B 4D 08") + 0x2);
	pointer["CStateManager"] = (DWORD*)(Memory::FindSignature("8B 0D ? ? ? ? E8 ? ? ? ? 84 C0 75 41") + 0x2);
	pointer["CPythonPlayer"] = *(DWORD**)(Memory::FindSignature("8B 0D ? ? ? ? 8B 35 ? ? ? ? 83 C1 04 8B 01") + 0x2);
	pointer["CPythonCharManager"] = *(DWORD**)(Memory::FindSignature("8B 0D ? ? ? ? FF 75 BA 83 C1 04") + 0x2);
	pointer["CPythonStateManager"] = *(DWORD**)(Memory::FindSignature("8B 0D ?? ?? ?? ?? 6A 00 6A 07 E8 ?? ?? ?? ?? 8B 0D ?? ?? ?? ?? 6A 00 6A 1C") + 0x2);
	pointer["CItemManager"] = (DWORD*)(Memory::FindSignature("8B 0D ? ? ? ? 6A 02 6A 01 6A 00 E8 ? ? ? ? 8B 0D ? ? ? ? 6A 01 6A 04 6A 00 E8 ? ? ? ? 8B 0D ? ? ? ? 6A 00 6A 07 E8 ? ? ? ? 8B 0D ? ? ? ? 6A 00 6A 1C") + 0x2);
	pointer["CPythonItem"] = *(DWORD**)(Memory::FindSignature("8B 0D ? ? ? ? 83 C0 64") + 0x2);
	pointer["CPythonBackground"] = *(DWORD**)(Memory::FindSignature("8B 0D ? ? ? ? 6A 02 83 EC 08") + 0x2);
	pointer["CPythonMinimap"] = *(DWORD**)(Memory::FindSignature("8B 0D ? ? ? ? 83 C4 10 D9 5D A0") + 0x2);
	pointer["CPythonNetworkStream"] = *(DWORD**)(Memory::FindSignature("8B 0D ? ? ? ? E8 ? ? ? ? 5D C2 08 00") + 0x2);
	pointer["CPythonNonPlayer"] = *(DWORD**)(Memory::FindSignature("A1 ? ? ? ? 8B 9D") + 0x1);
	pointer["CFlyingManager"] = *(DWORD**)(Memory::FindSignature("A1 ? ? ? ? 8B 48 1C") + 0x1);
	pointer["CPythonSkill"] = *(DWORD**)(Memory::FindSignature("8B 0D ? ? ? ? 50 E8 ? ? ? ? 85 C0 74 29") + 0x2);
	pointer["CPythonSystem"] = *(DWORD**)(Memory::FindSignature("8B 0D ? ? ? ? E8 ? ? ? ? 84 C0 74 56") + 0x2);

	pointer["GetResourcePointer"] = **(DWORD***)(Memory::FindSignature("8B 0D ? ? ? ? 8D 85 ? ? ? ? 50 E8 ? ? ? ? 8B F0") + 0x2);

	//Functions
	address["__ShowMapName"] = Memory::FindSignature("E8 ? ? ? ? 8B 8B ? ? ? ? 56", 1, 5);
	address["GetBowRange"] = Memory::FindSignature("55 8B EC 51 C7 45 ? ? ? ? ? E8 ? ? ? ? 84 C0 74 21 8B 0D ? ? ? ?");
	address["IsBowMode"] = Memory::FindSignature("E8 ? ? ? ? 85 C0 74 19 53", 1, 5);
	address["GetTerrainHeight"] = Memory::FindSignature("55 8B EC 83 EC 4C A1 ? ? ? ? 33 C5 89 45 FC 83 B9 ? ? ? ? ? 0F 85 ? ? ? ? 0F 28 05 ? ? ? ? 33 C9 0F 11 45 B4 C7 45 ? ? ? ? ?");
	address["GlobalPosToMapInfo"] = Memory::FindSignature("55 8B EC 56 FF 75 0C 8B B1");
	address["IsBlockedCoord"] = Memory::FindSignature("55 8B EC 8B 0D ? ? ? ? 6A 01");
	address["UIRender"] = Memory::FindSignature("57 8B F9 80 7F 55 00 0F 84 ? ? ? ? 8B 07 FF 50 04 83 3D ? ? ? ? ? 74 7A");
	address["RenderGame"] = Memory::FindSignature("55 8B EC 83 EC 44 80 3D ? ? ? ? ?");
	address["UpdateGame"] = Memory::FindSignature("55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 83 EC 48 A1 ? ? ? ? 33 C5 89 45 F0 53 56 57 50 8D 45 F4 64 A3 ? ? ? ? 8B D9 E8 ? ? ? ?");
	address["IncreaseExternalForce"] = Memory::FindSignature("55 8B EC 83 EC 20 A1 ? ? ? ? 33 C5 89 45 FC F3 0F 10 55");
	address["BlockMovement"] = Memory::FindSignature("8B 81 ? ? ? ? 85 C0 74 02");
	address["TestActorCollision"] = Memory::FindSignature("55 8B EC 83 EC 20 53 8B 5D 08 57");
	address["UpdateKeyboard"] = Memory::FindSignature("57 8B F9 8B 0D ? ? ? ? 85 C9");
	address["ProcessTerrainCollision"] = Memory::FindSignature("55 8B EC 83 E4 C0");
	address["Recv"] = Memory::FindSignature("55 8B EC 56 57 FF 75 0C 8B 7D 08");
	address["__Warp"] = Memory::FindSignature("55 8B EC 83 EC 0C 53 8B 1D");
	address["SelectAnswer"] = Memory::FindSignature("55 8B EC FF 75 08 E8 ? ? ? ? 84 C0 74 0E");
	address["GetPixelPosition"] = Memory::FindSignature("55 8B EC 81 C1 ? ? ? ? E8 ? ? ? ? 8B 4D 08");
	address["GetBoundBox2D"] = Memory::FindSignature("55 8B EC 81 C1 ? ? ? ? 5D E9 ? ? ? ? CC 55 8B EC 83 E4 F8");
	address["IsGameMaster"] = Memory::FindSignature("6A 00 83 C1 50");
	address["IsAttacking"] = Memory::FindSignature("56 8B F1 E8 ? ? ? ? 85 C0 74 07 B8");
	address["LookAtDestPixelPosition"] = Memory::FindSignature("55 8B EC 8B 45 08 83 EC 08 81 C1");
	address["MoveToDestPixelPosition"] = Memory::FindSignature("55 8B EC 56 8B F1 E8 ? ? ? ? 85 C0 74 07 32 C0 5E 5D C2 08 00");
	address["AttackToDestInstance"] = Memory::FindSignature("55 8B EC 83 EC 10 A1 ? ? ? ? 33 C5 89 45 FC 56 8D 45 F0 8B F1 8B 4D 08 50 E8 ? ? ? ? 8D 45 F0 8B CE 50 E8 ? ? ? ? 8B 4D FC B0 01");
	address["SetAttackKey"] = Memory::FindSignature("55 8B EC 53 8A 5D 08 56 8B F1");
	address["IsSkillCoolTime"] = Memory::FindSignature("55 8B EC FF 75 08 E8 ? ? ? ? 33 C9");
	address["IsSkillActive"] = Memory::FindSignature("E8 ? ? ? ? 85 C0 74 1E 8B 85");
	address["UseSkill"] = Memory::FindSignature("55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 81 EC ? ? ? ? A1 ? ? ? ? 33 C5 89 45 F0 56 57 50 8D 45 F4 64 A3 ? ? ? ? 8B F9 89 BD ? ? ? ? 8B 75 08 89 B5");
	address["StopWalking"] = Memory::FindSignature("55 8B EC 56 8B F1 C6 86 ? ? ? ? ? E8 ? ? ? ? 85 C0");
	address["StartWalking"] = Memory::FindSignature("56 8B F1 8D 8E ? ? ? ? E8 ? ? ? ? 6A 11");
	address["GetPickingPosition"] = Memory::FindSignature("55 8B EC 8B 0D ? ? ? ? 56 8B 75 08");
	address["SCRIPT_SetPixelPosition"] = Memory::FindSignature("55 8B EC 83 EC 10 A1 ? ? ? ? 33 C5 89 45 FC F3 0F 10 45 ? F3 0F 10 4D");
	address["GlobalPosToMapInfo"] = Memory::FindSignature("55 8B EC 56 FF 75 0C 8B B1");
	address["IsPositionBlocked"] = Memory::FindSignature("55 8B EC 8B 0D ? ? ? ? 6A 01");
	address["GetResourcePointer"] = Memory::FindSignature("55 8B EC 83 EC 60 A1 ? ? ? ? 33 C5 89 45 FC 8B 45 08 53");
	address["ReloadResource"] = Memory::FindSignature("55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 81 EC ? ? ? ? A1 ? ? ? ? 33 C5 89 45 F0 53 56 57 50 8D 45 F4 64 A3 ? ? ? ? 8B F9 E8");
	address["SetViewDistanceSet"] = Memory::FindSignature("55 8B EC 83 EC 0C 83 B9");
	address["RenderBox3D"] = Memory::FindSignature("55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 33 C5 89 45 FC 80 3D ? ? ? ? ? 0F 85");
	address["IsWalking"] = Memory::FindSignature("E8 ? ? ? ? 33 C9 83 F8 02");
	address["AddDamageEffect"] = Memory::FindSignature("55 8B EC 83 EC 10 56 8B F1 8B 0D ? ? ? ? ");

	//Packet functions
	address["Recv"] = Memory::FindSignature("E8 ? ? ? ? 84 C0 75 10 5E", 1, 5); //skip 0xE8 (call) then + 0x5 opcodes to get relative addr
	address["Send"] = Memory::FindSignature("55 8B EC 56 8B F1 57 8B 7D 08 8B 56 30 8B 4E 34 2B D1 8D 47 01 3B C2 7E 08");
	address["SendAttackPacket"] = Memory::FindSignature("55 8B EC 83 EC 30 A1 ? ? ? ? 33 C5 89 45 FC 56 8B F1 E8 ? ? ? ? 84 C0 75 13 B0 01 5E 8B 4D FC 33 CD E8 ? ? ? ? 8B E5 5D C2 08 00 8A 45 08");
	address["SendOnClickPacket"] = Memory::FindSignature("55 8B EC 83 EC 28 A1 ? ? ? ? 33 C5 89 45 FC 8B 45 08 56 89 45 D9 8B F1 8D 45 D8 C6 45 D8 1A");
	address["SendScriptAnswerPacket"] = Memory::FindSignature("55 8B EC 83 EC 2C A1 ? ? ? ? 33 C5 89 45 FC 8A 45 08 56 88 45 D5 8B F1 8D 45 D4 C6 45 D4 1D");
	address["SendGiveItemPacket"] = Memory::FindSignature("55 8B EC 83 EC 0C 8B 45 08 89 45 F5");
	address["SendPickUpItemPacket"] = Memory::FindSignature("55 8B EC 83 EC 28 A1 ? ? ? ? 33 C5 89 45 FC 56 8B F1 E8 ? ? ? ? 84 C0 75 13 B0 01 5E 8B 4D FC 33 CD E8 ? ? ? ? 8B E5 5D C2 04 00 8B 45 08");
	address["SendShootPacket"] = Memory::FindSignature("55 8B EC 83 EC 20 A1 ? ? ? ? 33 C5 89 45 FC 8A 45 08");
	address["SendTargetPacket"] = Memory::FindSignature("55 8B EC 83 EC 28 A1 ? ? ? ? 33 C5 89 45 FC 8B 45 08 56 89 45 D9 8B F1 8D 45 D8 C6 45 D8 3D");
	address["SendAddFlyTargetingPacket"] = Memory::FindSignature("55 8B EC 83 EC 34 A1 ? ? ? ? 33 C5 89 45 FC 8B 45 08 56 8B F1 89 45 ED 8B 4D 0C C6 45 EC 35");
	address["SendFlyTargetingPacket"] = Memory::FindSignature("55 8B EC 83 EC 34 A1 ? ? ? ? 33 C5 89 45 FC 8B 45 08 56 8B F1 89 45 ED 8B 4D 0C C6 45 EC 33");
	address["SendScriptButtonPacket"] = Memory::FindSignature("55 8B EC 83 EC 30 A1 ? ? ? ? 33 C5 89 45 FC 8B 45 08 56 89 45 D1");
	address["SendRefinePacket"] = Memory::FindSignature("55 8B EC 51 8A 45 08 88 45 FD");
	address["SendUseSkillPacket"] = Memory::FindSignature("55 8B EC 83 EC 50 A1 ? ? ? ? 33 C5 89 45 FC 8B 45 08 89 45 F1");
	address["SendChatPacket"] = Memory::FindSignature("55 8B EC 56 8B 75 08 8B D6 57 8B F9");
	address["SendSyncPositionPacket"] = Memory::FindSignature("55 8B EC 83 EC 50 A1 ? ? ? ? 33 C5 89 45 FC 8B 45 08 89 45 F0");
	address["SendCharacterStatePacket"] = Memory::FindSignature("55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 33 C5 89 45 FC 56 57 8B 7D 08 8B F1 E8");
	address["SendItemUsePacket"] = Memory::FindSignature("55 8B EC 83 EC 28 A1 ? ? ? ? 33 C5 89 45 FC 56 8B F1 E8 ? ? ? ? 84 C0 74 4D");
	address["SendItemUseToItemPacket"] = Memory::FindSignature("55 8B EC 83 EC 30 A1 ? ? ? ? 33 C5 89 45 FC 56 8B F1 E8 ? ? ? ? 84 C0 75 13 B0 01 5E 8B 4D FC 33 CD E8 ? ? ? ? 8B E5 5D C2 08 00 66 8B 45 08");
	address["RecvDamageInfoPacket"] = Memory::FindSignature("55 8B EC 83 EC 30 A1 ? ? ? ? 33 C5 89 45 FC 57 8D 45 D0 8B F9 50 6A 0A");
	address["OnScriptEventStart"] = Memory::FindSignature("55 8B EC 56 FF 75 0C 8B F1 FF 75 08 68 ? ? ? ? E8 ? ? ? ? 50 68 ? ? ? ? FF B6 ? ? ? ? E8 ? ? ? ? 83 C4 18 5E 5D C2 08 00");

	for (const auto& [name, addr] : address) { if (addr == 0x0) sig_failed.push_back(name); sig_total++; }
	for (const auto& [name, ptr] : pointer) { if (ptr == nullptr) sig_failed.push_back(name); sig_total++; }
	sig_found = sig_total - sig_failed.size();

	return sig_failed.empty();
}