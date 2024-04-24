#pragma once
#define PACKET_TO_SERVER 0
#define PACKET_TO_CLIENT 1

class PacketSniffer
{
public:
	PacketSniffer();
	bool OnRecv(int len, const char* buff);
	bool OnSend(int len, const char* buff);

	void SetSendFilter(const std::array<bool, 256>& packet_ids);
	void SetRecvFilter(const std::array<bool, 256>& packet_ids);

	const char* GetHeaderName(BYTE header, int to);

private:
	void DumpPacket(const char* buff, int len, int to);
	void PrintASCII(const BYTE* buff, int len, int to);

	std::array<bool, 256> m_send_filter;
	std::array<bool, 256> m_recv_filter;
};

