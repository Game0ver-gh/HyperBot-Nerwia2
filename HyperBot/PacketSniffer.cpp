#include "pch.h"
#include "PacketSniffer.h"

static bool was_prev_packet_chat_packet = false;
static int prev_packet = 0;

void PacketSniffer::PrintASCII(const BYTE* buff, int len, int to)
{
	std::string output = was_prev_packet_chat_packet ? "" : "\n";
	char ascii[17];
	int i, j;
	ascii[16] = '\0';
	for (i = 0; i < len; ++i) 
	{
		char buf[64]{};
		sprintf_s(buf, sizeof(buf), "%02X ", ((unsigned char*)buff)[i]);
		output += buf;
		if (((unsigned char*)buff)[i] >= ' ' && ((unsigned char*)buff)[i] <= '~') 
			ascii[i % 16] = ((unsigned char*)buff)[i];
		else 
			ascii[i % 16] = '.';

		if ((i + 1) % 8 == 0 || i + 1 == len) 
		{
			output += " ";
			if ((i + 1) % 16 == 0) 
			{
				sprintf_s(buf, sizeof(buf), "|  %s \n", ascii);
				output += buf;
			}
			else if (i + 1 == len) 
			{
				ascii[(i + 1) % 16] = '\0';

				if ((i + 1) % 16 <= 8) 
					output += " ";
				
				for (j = (i + 1) % 16; j < 16; ++j) 
					output += "   ";
				
				sprintf_s(buf, sizeof(buf), "|  %s \n", ascii);
				output += buf;
			}
		}
	}

	printf(output.c_str());
}

void PacketSniffer::DumpPacket(const char* buff, int len, int to)
{
	bool drop = false;
	if (to == PACKET_TO_CLIENT)
	{
		if (!m_recv_filter[*buff])
			drop = true;

		if (!m_recv_filter[HEADER_GC_CHAT] && was_prev_packet_chat_packet)
			drop = true;
	}

	if (to == PACKET_TO_SERVER)
	{
		if (!m_send_filter[*buff])
			drop = true;

		if (!m_send_filter[HEADER_CG_CHAT] && was_prev_packet_chat_packet)
			drop = true;
	}

	if (!was_prev_packet_chat_packet && !drop)
	{
		Console::MultiColor format =
		{
			{ "\n[", Console::TextColor::YELLOW },
			{ (to == PACKET_TO_SERVER ? "Send " : "Recv "), (to == PACKET_TO_SERVER ? Console::TextColor::GREEN : Console::TextColor::RED) },
			{ (to == PACKET_TO_SERVER ? "-> " : "<- "), Console::TextColor::YELLOW },
			{ GetHeaderName(*buff, to), Console::TextColor::CYAN },
			{ "]", Console::TextColor::YELLOW }
		};
		G::console->WriteMultiColor(format);
	}
	
	if (!drop)
		PrintASCII((BYTE*)buff, len, to);

	was_prev_packet_chat_packet = (to == PACKET_TO_SERVER ? HEADER_CG_CHAT : HEADER_GC_CHAT) == int(*buff);
}

PacketSniffer::PacketSniffer()
{
	m_send_filter.fill(true);
	m_recv_filter.fill(true);
}

bool PacketSniffer::OnRecv(int len, const char* buff)
{
	if (len <= 0 || !buff)
		return false;

	if (G::cfg[EBool::DBG_PRINT_PACKETS])
		DumpPacket(buff, len, PACKET_TO_CLIENT);

	return true;
}

bool PacketSniffer::OnSend(int len, const char* buff)
{
	if (len <= 0 || !buff)
		return false;

	if (G::cfg[EBool::DBG_PRINT_PACKETS])
		DumpPacket(buff, len, PACKET_TO_SERVER);

	return true;
}

void PacketSniffer::SetSendFilter(const std::array<bool, 256>& packet_ids)
{
	m_send_filter = packet_ids;
}

void PacketSniffer::SetRecvFilter(const std::array<bool, 256>& packet_ids)
{
	m_recv_filter = packet_ids;
}

const char* PacketSniffer::GetHeaderName(BYTE header, int to)
{
	static bool b = false;
	static std::string send_list[UCHAR_MAX + 1];
	static std::string recv_list[UCHAR_MAX + 1];
	if (b == false)
	{
		b = true;
		for (DWORD i = 0; i < UCHAR_MAX + 1; i++)
		{
			char buf[32];
			sprintf_s(buf, "_UNKNOWN_%d", i);
			send_list[i] = buf;
			recv_list[i] = buf;
		}

		//send
		send_list[1] = "HEADER_CG_LOGIN";
		send_list[2] = "HEADER_CG_ATTACK";
		send_list[3] = "HEADER_CG_CHAT";
		send_list[4] = "HEADER_CG_PLAYER_CREATE";
		send_list[5] = "HEADER_CG_PLAYER_DESTROY";
		send_list[6] = "HEADER_CG_PLAYER_SELECT";
		send_list[7] = "HEADER_CG_CHARACTER_MOVE";
		send_list[8] = "HEADER_CG_SYNC_POSITION";
		send_list[9] = "HEADER_CG_DIRECT_ENTER";
		send_list[10] = "HEADER_CG_ENTERGAME";
		send_list[11] = "HEADER_CG_ITEM_USE";
		send_list[12] = "HEADER_CG_ITEM_DROP";
		send_list[13] = "HEADER_CG_ITEM_MOVE";
		send_list[15] = "HEADER_CG_ITEM_PICKUP";
		send_list[16] = "HEADER_CG_QUICKSLOT_ADD";
		send_list[17] = "HEADER_CG_QUICKSLOT_DEL";
		send_list[18] = "HEADER_CG_QUICKSLOT_SWAP";
		send_list[19] = "HEADER_CG_WHISPER";
		send_list[20] = "HEADER_CG_ITEM_DROP2";
		send_list[26] = "HEADER_CG_ON_CLICK";
		send_list[27] = "HEADER_CG_EXCHANGE";
		send_list[28] = "HEADER_CG_CHARACTER_POSITION";
		send_list[29] = "HEADER_CG_SCRIPT_ANSWER";
		send_list[30] = "HEADER_CG_QUEST_INPUT_STRING";
		send_list[31] = "HEADER_CG_QUEST_CONFIRM";
		send_list[41] = "HEADER_CG_PVP";
		send_list[50] = "HEADER_CG_SHOP";
		send_list[51] = "HEADER_CG_FLY_TARGETING";
		send_list[52] = "HEADER_CG_USE_SKILL";
		send_list[53] = "HEADER_CG_ADD_FLY_TARGETING";
		send_list[54] = "HEADER_CG_SHOOT";
		send_list[55] = "HEADER_CG_MYSHOP";
		send_list[60] = "HEADER_CG_ITEM_USE_TO_ITEM";
		send_list[61] = "HEADER_CG_TARGET";
		send_list[65] = "HEADER_CG_WARP";
		send_list[66] = "HEADER_CG_SCRIPT_BUTTON";
		send_list[67] = "HEADER_CG_MESSENGER";
		send_list[69] = "HEADER_CG_MALL_CHECKOUT";
		send_list[70] = "HEADER_CG_SAFEBOX_CHECKIN";
		send_list[71] = "HEADER_CG_SAFEBOX_CHECKOUT";
		send_list[72] = "HEADER_CG_PARTY_INVITE";
		send_list[73] = "HEADER_CG_PARTY_INVITE_ANSWER";
		send_list[74] = "HEADER_CG_PARTY_REMOVE";
		send_list[75] = "HEADER_CG_PARTY_SET_STATE";
		send_list[76] = "HEADER_CG_PARTY_USE_SKILL";
		send_list[77] = "HEADER_CG_SAFEBOX_ITEM_MOVE";
		send_list[78] = "HEADER_CG_PARTY_PARAMETER";
		send_list[80] = "HEADER_CG_GUILD";
		send_list[81] = "HEADER_CG_ANSWER_MAKE_GUILD";
		send_list[82] = "HEADER_CG_FISHING";
		send_list[83] = "HEADER_CG_GIVE_ITEM";
		send_list[90] = "HEADER_CG_EMPIRE";
		send_list[96] = "HEADER_CG_REFINE";
		send_list[100] = "HEADER_CG_MARK_LOGIN";
		send_list[101] = "HEADER_CG_MARK_CRCLIST";
		send_list[102] = "HEADER_CG_MARK_UPLOAD";
		send_list[103] = "HEADER_CG_CRC_REPORT";
		send_list[105] = "HEADER_CG_HACK";
		send_list[106] = "HEADER_CG_CHANGE_NAME";
		send_list[107] = "HEADER_CG_SMS";
		send_list[108] = "HEADER_CG_MATRIX_CARD";
		send_list[109] = "HEADER_CG_LOGIN2";
		send_list[110] = "HEADER_CG_DUNGEON";
		send_list[111] = "HEADER_CG_LOGIN3";
		send_list[112] = "HEADER_CG_GUILD_SYMBOL_UPLOAD";
		send_list[113] = "HEADER_CG_GUILD_SYMBOL_CRC";
		send_list[114] = "HEADER_CG_SCRIPT_SELECT_ITEM";
		send_list[0xfb] = "HEADER_CG_KEY_AGREEMENT";
		send_list[0xfc] = "HEADER_CG_TIME_SYNC";
		send_list[0xfd] = "HEADER_CG_CLIENT_VERSION";
		send_list[0xfe] = "HEADER_CG_PONG";
		send_list[0xff] = "HEADER_CG_HANDSHAKE";

		//recv
		recv_list[1] = "HEADER_GC_CHARACTER_ADD";
		recv_list[2] = "HEADER_GC_CHARACTER_DEL";
		recv_list[3] = "HEADER_GC_CHARACTER_MOVE";
		recv_list[4] = "HEADER_GC_CHAT";
		recv_list[5] = "HEADER_GC_SYNC_POSITION";
		recv_list[6] = "HEADER_GC_LOGIN_SUCCESS3";
		recv_list[7] = "HEADER_GC_LOGIN_FAILURE";
		recv_list[8] = "HEADER_GC_PLAYER_CREATE_SUCCESS";
		recv_list[9] = "HEADER_GC_PLAYER_CREATE_FAILURE";
		recv_list[10] = "HEADER_GC_PLAYER_DELETE_SUCCESS";
		recv_list[11] = "HEADER_GC_PLAYER_DELETE_WRONG_SOCIAL_ID";
		recv_list[13] = "HEADER_GC_STUN";
		recv_list[14] = "HEADER_GC_DEAD";
		recv_list[15] = "HEADER_GC_MAIN_CHARACTER";
		recv_list[16] = "HEADER_GC_PLAYER_POINTS";
		recv_list[17] = "HEADER_GC_PLAYER_POINT_CHANGE";
		recv_list[18] = "HEADER_GC_CHANGE_SPEED";
		recv_list[19] = "HEADER_GC_CHARACTER_UPDATE";
		recv_list[20] = "HEADER_GC_ITEM_SET";
		recv_list[21] = "HEADER_GC_ITEM_SET2";
		recv_list[22] = "HEADER_GC_ITEM_USE";
		recv_list[23] = "HEADER_GC_ITEM_DROP";
		recv_list[25] = "HEADER_GC_ITEM_UPDATE";
		recv_list[26] = "HEADER_GC_ITEM_GROUND_ADD";
		recv_list[27] = "HEADER_GC_ITEM_GROUND_DEL";
		recv_list[28] = "HEADER_GC_QUICKSLOT_ADD";
		recv_list[29] = "HEADER_GC_QUICKSLOT_DEL";
		recv_list[30] = "HEADER_GC_QUICKSLOT_SWAP";
		recv_list[31] = "HEADER_GC_ITEM_OWNERSHIP";
		recv_list[32] = "HEADER_GC_LOGIN_SUCCESS4";
		recv_list[33] = "HEADER_GC_ITEM_UNBIND_TIME";
		recv_list[34] = "HEADER_GC_WHISPER";
		recv_list[35] = "HEADER_GC_ALERT";
		recv_list[36] = "HEADER_GC_MOTION";
		recv_list[38] = "HEADER_GC_SHOP";
		recv_list[39] = "HEADER_GC_SHOP_SIGN";
		recv_list[40] = "HEADER_GC_DUEL_START";
		recv_list[41] = "HEADER_GC_PVP";
		recv_list[42] = "HEADER_GC_EXCHANGE";
		recv_list[43] = "HEADER_GC_CHARACTER_POSITION";
		recv_list[44] = "HEADER_GC_PING";
		recv_list[45] = "HEADER_GC_SCRIPT";
		recv_list[46] = "HEADER_GC_QUEST_CONFIRM";
		recv_list[61] = "HEADER_GC_MOUNT";
		recv_list[62] = "HEADER_GC_OWNERSHIP";
		recv_list[63] = "HEADER_GC_TARGET";
		recv_list[65] = "HEADER_GC_WARP";
		recv_list[69] = "HEADER_GC_ADD_FLY_TARGETING";
		recv_list[70] = "HEADER_GC_CREATE_FLY";
		recv_list[71] = "HEADER_GC_FLY_TARGETING";
		recv_list[72] = "HEADER_GC_SKILL_LEVEL";
		recv_list[73] = "HEADER_GC_SKILL_COOLTIME_END";
		recv_list[74] = "HEADER_GC_MESSENGER";
		recv_list[75] = "HEADER_GC_GUILD";
		recv_list[76] = "HEADER_GC_SKILL_LEVEL_NEW";
		recv_list[77] = "HEADER_GC_PARTY_INVITE";
		recv_list[78] = "HEADER_GC_PARTY_ADD";
		recv_list[79] = "HEADER_GC_PARTY_UPDATE";
		recv_list[80] = "HEADER_GC_PARTY_REMOVE";
		recv_list[81] = "HEADER_GC_QUEST_INFO";
		recv_list[82] = "HEADER_GC_REQUEST_MAKE_GUILD";
		recv_list[83] = "HEADER_GC_PARTY_PARAMETER";
		recv_list[84] = "HEADER_GC_SAFEBOX_MONEY_CHANGE";
		recv_list[85] = "HEADER_GC_SAFEBOX_SET";
		recv_list[86] = "HEADER_GC_SAFEBOX_DEL";
		recv_list[87] = "HEADER_GC_SAFEBOX_WRONG_PASSWORD";
		recv_list[88] = "HEADER_GC_SAFEBOX_SIZE";
		recv_list[89] = "HEADER_GC_FISHING";
		recv_list[90] = "HEADER_GC_EMPIRE";
		recv_list[91] = "HEADER_GC_PARTY_LINK";
		recv_list[92] = "HEADER_GC_PARTY_UNLINK";
		recv_list[95] = "HEADER_GC_REFINE_INFORMATION";
		recv_list[96] = "HEADER_GC_OBSERVER_ADD";
		recv_list[97] = "HEADER_GC_OBSERVER_REMOVE";
		recv_list[98] = "HEADER_GC_OBSERVER_MOVE";
		recv_list[99] = "HEADER_GC_VIEW_EQUIP";
		recv_list[100] = "HEADER_GC_MARK_BLOCK";
		recv_list[101] = "HEADER_GC_MARK_DIFF_DATA";
		recv_list[102] = "HEADER_GC_MARK_IDXLIST";
		recv_list[105] = "HEADER_GC_SLOW_TIMER";
		recv_list[106] = "HEADER_GC_TIME";
		recv_list[107] = "HEADER_GC_CHANGE_NAME";
		recv_list[110] = "HEADER_GC_DUNGEON";
		recv_list[111] = "HEADER_GC_WALK_MODE";
		recv_list[112] = "HEADER_GC_CHANGE_SKILL_GROUP";
		recv_list[113] = "HEADER_GC_MAIN_CHARACTER2_EMPIRE";
		recv_list[114] = "HEADER_GC_SEPCIAL_EFFECT";
		recv_list[115] = "HEADER_GC_NPC_POSITION";
		recv_list[116] = "HEADER_GC_CHINA_MATRIX_CARD";
		recv_list[117] = "HEADER_GC_CHARACTER_UPDATE2";
		recv_list[118] = "HEADER_GC_LOGIN_KEY";
		recv_list[119] = "HEADER_GC_REFINE_INFORMATION_NEW";
		recv_list[120] = "HEADER_GC_CHARACTER_ADD2";
		recv_list[121] = "HEADER_GC_CHANNEL";
		recv_list[122] = "HEADER_GC_MALL_OPEN";
		recv_list[123] = "HEADER_GC_TARGET_UPDATE";
		recv_list[124] = "HEADER_GC_TARGET_DELETE";
		recv_list[125] = "HEADER_GC_TARGET_CREATE_NEW";
		recv_list[126] = "HEADER_GC_AFFECT_ADD";
		recv_list[127] = "HEADER_GC_AFFECT_REMOVE";
		recv_list[128] = "HEADER_GC_MALL_SET";
		recv_list[129] = "HEADER_GC_MALL_DEL";
		recv_list[130] = "HEADER_GC_LAND_LIST";
		recv_list[131] = "HEADER_GC_LOVER_INFO";
		recv_list[132] = "HEADER_GC_LOVE_POINT_UPDATE";
		recv_list[133] = "HEADER_GC_GUILD_SYMBOL_DATA";
		recv_list[134] = "HEADER_GC_DIG_MOTION";
		recv_list[135] = "HEADER_GC_DAMAGE_INFO";
		recv_list[136] = "HEADER_GC_CHAR_ADDITIONAL_INFO";
		recv_list[137] = "HEADER_GC_MAIN_CHARACTER3_BGM";
		recv_list[138] = "HEADER_GC_MAIN_CHARACTER4_BGM_VOL";
		recv_list[150] = "HEADER_GC_AUTH_SUCCESS";
		recv_list[151] = "HEADER_GC_PANAMA_PACK";
		recv_list[152] = "HEADER_GC_HYBRIDCRYPT_KEYS";
		recv_list[153] = "HEADER_GC_HYBRIDCRYPT_SDB";
		recv_list[154] = "HEADER_GC_AUTH_SUCCESS_OPENID";
		recv_list[201] = "HEADER_GC_RUNUP_MATRIX_QUIZ";
		recv_list[202] = "HEADER_GC_NEWCIBN_PASSPOD_REQUEST";
		recv_list[203] = "HEADER_GC_NEWCIBN_PASSPOD_FAILURE";
		recv_list[204] = "HEADER_GC_HS_REQUEST";
		recv_list[205] = "HEADER_GC_XTRAP_CS1_REQUEST";
		recv_list[208] = "HEADER_GC_SPECIFIC_EFFECT";
		recv_list[209] = "HEADER_GC_DRAGON_SOUL_REFINE";
		recv_list[210] = "HEADER_GC_RESPOND_CHANNEL";
		recv_list[0xFA] = "HEADER_GC_KEY_AGREEMENT_COMPLETED";
		recv_list[0xFB] = "HEADER_GC_KEY_AGREEMENT";
		recv_list[0xFC] = "HEADER_GC_HANDSHAKE_OK";
		recv_list[0xFD] = "HEADER_GC_PHASE";
		recv_list[0xFE] = "HEADER_GC_BINDUDP";
		recv_list[0xFF] = "HEADER_GC_HANDSHAKE";

	}

	return (to == PACKET_TO_SERVER ? send_list[header].c_str() : recv_list[header].c_str());
}
