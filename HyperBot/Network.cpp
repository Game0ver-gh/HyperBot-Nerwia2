#include "pch.h"
#include "Network.h"
#define CHECK_PACKET_SENT if (!ShouldSend()) return false; //Prevent packet spamming

Clock                   m_packet_timer;
static constexpr float	m_packet_delay = 1.5f;
static size_t           m_packets_per_tick = 0;

bool Network::SendRefinePacket(BYTE pos, BYTE type)
{
    static const auto fnAddr = G::memory->GetAddress("SendRefinePacket");
    static const auto netSendFn = reinterpret_cast<bool(__thiscall*)(void*, BYTE, BYTE)>(fnAddr);
    return netSendFn(this, pos, type);
}

bool Network::Recv(int size, void* pBuffer)
{
    static const auto fnAddr = G::memory->GetAddress("Recv");
    static const auto RecvFn = reinterpret_cast<bool(__thiscall*)(void*, int, void*)>(fnAddr);
    return RecvFn(this, size, pBuffer);
}

int Network::__Warp(const Vector& dst)
{
    static const auto fnAddr = G::memory->GetAddress("__Warp");
    static const auto RecvFn = reinterpret_cast<bool(__thiscall*)(void*, float, float)>(fnAddr);
    return RecvFn(this, dst.x, dst.y);
}

bool Network::IsConnected()
{
    return *(bool*)(this + 0x48);
}

bool Network::ConnectGameServer(UINT charSlot)
{
    static const auto fnAddr = G::memory->GetAddress("ConnectGameServer");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*, UINT)>(fnAddr);
    return call(this, charSlot);
}

void Network::SetLoginInfo(const char* login, const char* password)
{
    static const auto fnAddr = G::memory->GetAddress("SetLoginInfo");
    static const auto call = reinterpret_cast<UINT(__thiscall*)(void*, const char*, const char*)>(fnAddr);
    static const auto call2 = reinterpret_cast<UINT(__thiscall*)(void*, const char*, const char*)>(fnAddr - 0x5740); // + ???
    if (call && call2)
    {
        call(this, login, password);
        call2(this, login, password);
    }
}

void Network::SelectAnswer(int index, BYTE answer)
{
    static const auto fnAddr = G::memory->GetAddress("SelectAnswer");
    static const auto call = reinterpret_cast<void(__stdcall*)(int, BYTE)>(fnAddr);
    call(index, answer);
}

size_t Network::GetPacketsPerSec()
{
    static Clock timer;
    static size_t packets_per_sec = 0, last_updated = 0;
    if (timer.GetElapsedSinceUpdate() <= 1)
    {
        packets_per_sec += m_packets_per_tick;
	}
    else
    {
        last_updated = packets_per_sec;
        packets_per_sec = 0;
        timer.Update();
    }
	
    return last_updated;
}

size_t Network::GetPacketsCount()
{
    return m_packets_per_tick;
}

size_t Network::GetAvailablePackets()
{
    return size_t(std::abs(G::cfg[EInt::M_MAX_PACKET_PER_TICK] - (int)GetPacketsCount()));
}

size_t Network::GetMaxPacketsCount()
{
    return G::cfg[EInt::M_MAX_PACKET_PER_TICK];
}

void Network::UpdatePacketSent()
{
    m_packets_per_tick = NULL;
}

Entity* Network::GetTargetEntity()
{
	return *(Entity**)(this + 0x1E6C);
}

bool Network::Send(int len, const void* buf)
{
    static const auto fnAddr = G::memory->GetAddress("Send");
	static const auto call = reinterpret_cast<bool(__thiscall*)(void*, int, const char*)>(fnAddr);
	return call(this, len, (const char*)buf);
}

void Network::NotifyShowMap()
{
    m_packet_timer.Update();
}

bool Network::ShouldSend()
{
    if (m_packet_timer.GetElapsedSinceUpdate() <= m_packet_delay)
		return false;

    if (int(m_packets_per_tick) > G::cfg[EInt::M_MAX_PACKET_PER_TICK])
        return false; 

    m_packets_per_tick++;

    return true;
}

bool Network::SendChatPacket(const char* msg, EChatType type)
{
    static const auto fnAddr = G::memory->GetAddress("SendChatPacket");
    static const auto SendChatPacket = reinterpret_cast<bool(__thiscall*)(void*, const char*, BYTE)>(fnAddr);
    return SendChatPacket(this, msg, (BYTE)type);
}

bool Network::SendAttackPacket(VID vid, UINT uMotion)
{
    CHECK_PACKET_SENT;
    static const auto fnAddr = G::memory->GetAddress("SendAttackPacket");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*, char, int)>(fnAddr);
    return call(this, uMotion, vid);
}

bool Network::SendPickUpItemPacket(VID vid)
{
    CHECK_PACKET_SENT;
    static const auto fnAddr = G::memory->GetAddress("SendPickUpItemPacket");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*, int)>(fnAddr);
    return call(this, vid);
}

bool Network::__SendHack(int encrypted_msg)
{
    //TODO (It is not used in Nerwia2)
    return false;
}

bool Network::SendAddFlyTargetingPacket(VID dwTargetVID, const Vector& kPPosTarget)
{
    CHECK_PACKET_SENT;
    static const auto fnAddr = G::memory->GetAddress("SendAddFlyTargetingPacket");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*, DWORD, const Vector&)>(fnAddr);
    return call(this, dwTargetVID, kPPosTarget);
}

bool Network::SendFlyTargetingPacket(VID dwTargetVID, const Vector& kPPosTarget)
{
    CHECK_PACKET_SENT;
    static const auto fnAddr = G::memory->GetAddress("SendFlyTargetingPacket");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*, DWORD, const Vector&)>(fnAddr);
    return call(this, dwTargetVID, kPPosTarget);
}

bool Network::SendSyncPositionPacket(VID dwVictimVID, DWORD dwVictimX, DWORD dwVictimY)
{
    CHECK_PACKET_SENT;
    static const auto fnAddr = G::memory->GetAddress("SendSyncPositionPacket");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*, DWORD, DWORD, DWORD)>(fnAddr);
    return call(this, dwVictimVID, dwVictimX, dwVictimY);
}

bool Network::SendOnClickPacket(VID vid)
{
    CHECK_PACKET_SENT;
    static const auto fnAddr = G::memory->GetAddress("SendOnClickPacket");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*, DWORD)>(fnAddr);
    return call(this, vid);
}

bool Network::SendScriptAnswerPacket(BYTE answer)
{
    CHECK_PACKET_SENT;
    static const auto fnAddr = G::memory->GetAddress("SendScriptAnswerPacket");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*, BYTE)>(fnAddr);
    return call(this, answer);
}

bool Network::SendGiveItemPacket(VID vid, TItemPos itemPos, BYTE count)
{
    CHECK_PACKET_SENT;
    static const auto fnAddr = G::memory->GetAddress("SendGiveItemPacket");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*, DWORD, TItemPos, BYTE)>(fnAddr);
    return call(this, vid, itemPos, count);
}

bool Network::SendItemUsePacket(TItemPos itemPos)
{
    CHECK_PACKET_SENT;
    static const auto fnAddr = G::memory->GetAddress("SendItemUsePacket");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*, TItemPos)>(fnAddr);
    return call(this, itemPos);
}

bool Network::SendItemUseToItemPacket(TItemPos source_pos, TItemPos target_pos)
{
    CHECK_PACKET_SENT;
	static const auto fnAddr = G::memory->GetAddress("SendItemUseToItemPacket");
	static const auto call = reinterpret_cast<bool(__thiscall*)(void*, TItemPos, TItemPos)>(fnAddr);
	return call(this, source_pos, target_pos);
}

bool Network::SendShootPacket(UINT uiSkill)
{
    CHECK_PACKET_SENT;
    static const auto fnAddr = G::memory->GetAddress("SendShootPacket");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*, UINT)>(fnAddr);
    return call(this, uiSkill);
}

bool Network::SendUseSkillPacket(DWORD dwSkillIndex, VID dwTargetVID)
{
    CHECK_PACKET_SENT;
    static const auto fnAddr = G::memory->GetAddress("SendUseSkillPacket");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*, DWORD, DWORD)>(fnAddr);
    return call(this, dwSkillIndex, dwTargetVID);
}

bool Network::SendTargetPacket(VID vid)
{
    CHECK_PACKET_SENT;
    static const auto fnAddr = G::memory->GetAddress("SendTargetPacket");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*, DWORD)>(fnAddr);
    return call(this, vid);
}

bool Network::SendCharacterStatePacket(const Vector& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg)
{
    CHECK_PACKET_SENT;
    static const auto fnAddr = G::memory->GetAddress("SendCharacterStatePacket");
    static const auto call = reinterpret_cast<bool(__thiscall*)(void*, const Vector&, float, UINT, UINT)>(fnAddr);
    return call(this, c_rkPPosDst, fDstRot, eFunc, uArg);
}
