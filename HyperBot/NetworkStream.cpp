#include "pch.h"


bool __fastcall HookManager::Send(void* pThis, void* edx, int len, char* buff)
{
	auto* _this = (DWORD*)(pThis);
	int m_sendBufSize = _this[0xC];
	int m_sendBufInputPos = _this[0xD];
	int sendBufRestSize = m_sendBufSize - m_sendBufInputPos;

	if ((len + 1) > sendBufRestSize)
		return false;

	memcpy((void*)(_this[0xB] + m_sendBufInputPos), buff, len);
	_this[0xD] += len;

	if (!F::route_creator->OnSend((BYTE*)buff, len))
		return false;

	if (!F::packet_sniffer->OnSend(len, buff)) //should drop packet?
		return false;

	return true;
}

bool __fastcall HookManager::Recv(void* pThis, void* edx, int len, char* buff)
{
	auto* _this = (DWORD*)(pThis);
	if (_this[9] - _this[10] < len)
		return false;

	memcpy(buff, (const void*)(_this[7] + _this[10]), len);
	_this[10] += len;

	if (!F::route_creator->OnRecv((BYTE*)buff, len))
		return false;

	if (!F::packet_sniffer->OnRecv(len, buff)) //should drop packet?
		return false;
	
	return true;
}

bool __fastcall HookManager::RecvDamageInfoPacket(void* pThis, void* edx)
{
	if (!G::cfg[EBool::WH_ENABLED] || F::wait_hack->GetTargetList().empty())
		return HookManager::oRecvDamageInfoPacket(pThis);

	TPacketGCDamageInfo DamageInfoPacket;

	auto* network = G::game->GetNetworkMgr();

	if (!network->Recv(sizeof(TPacketGCDamageInfo), &DamageInfoPacket))
		return false;

	static auto fn_addr = G::memory->GetAddress("AddDamageEffect");
	if (!fn_addr)
		return false;

	static auto AddDamageEffect = reinterpret_cast
		<void(__thiscall*)(void* inst_base, DWORD dmg, BYTE flag, BOOL self, BOOL target)>
		(fn_addr);
	
	if (DamageInfoPacket.damage <= 0)
		return true;

	auto* local = G::game->GetLocalPlayer();
	DWORD* _this = (DWORD*)pThis;

	if (G::cfg[EBool::WH_ENABLED] && !F::wait_hack->GetTargetList().empty())
	{
		static auto ptr = G::memory->GetPointer("CPythonSystem");
		if (ptr) ptr[0x25CF] = true; //Wish I remember what this does...

		for (const auto& target : F::wait_hack->GetTargetList())
		{
			//Add damage effect to every wait hack target
			//I think its very cool to see dmg numbers on targets which are being attacked
			AddDamageEffect(target.m_target, DamageInfoPacket.damage, DamageInfoPacket.flag, false, true);
		}
	}

	return true;
}