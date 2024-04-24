#include "pch.h"
#include "RouteCreator.h"

#define PRINTF_RECORD(fmt, ...) G::console->WriteInfo("Record %s(): " fmt, __func__, ##__VA_ARGS__);
#define PRINTF_ACTION(fmt, ...) G::console->WriteInfo("Action %s(): " fmt, __func__, ##__VA_ARGS__);

static bool recording = false, was_recording = false;
static bool playing = false, tp_insead_of_walk = false, reverse = false;
static bool has_opened_quest = false;

RouteCreator::~RouteCreator()
{
}

void RouteCreator::Init()
{

}

void RouteCreator::OnMenuFrame()
{
   
    ImGui::CustomCheckbox("Record", &recording);

    if (recording && !was_recording)
    {
        was_recording = true;
        StartRecording();
    }
    else if (!recording && was_recording)
    {
        was_recording = false;
        StopRecording();
    }

   
    ImGui::CustomCheckbox("Play", &playing);
    ImGui::CustomCheckbox("Play reverse", &reverse);
    ImGui::CustomCheckbox("TP insead of walk", &tp_insead_of_walk);
}

void RouteCreator::PreIteration(Player* local)
{
    m_local = local;
    m_local->GetPixelPosition(&m_local_pos);
    m_cur_map_name = G::game->GetBackground()->GetCurrentMapName();

    if (recording && m_local_pos.Distance(m_local_last_pos) >= 500.f) 
    {
        RecordData rd;
        m_local_last_pos = m_local_pos;
        rd.m_pos = m_local_pos;
        rd.m_action = RecordMainAction::GO_TO;
        m_records.push_back(rd);
    }
}

void RouteCreator::OnIteration(Entity* entity)
{
    return;
}

void RouteCreator::PostIteration()
{
    if (playing)
    {
        Play(reverse);
    }
}

void RouteCreator::OnRenderWorld()
{
    if (m_records.size() < 1)
        return;

    size_t i = 0;
    std::string map = G::game->GetBackground()->GetCurrentMapName();
    for (const auto& r : m_records)
    {
        if (i + 1 >= m_records.size())
            break;

        if (m_local_pos.Distance(r.m_pos) <= 20000.f)
        {
            Vector p1 = r.m_pos;
            Vector p2 = m_records[i + 1].m_pos;

            p1.z += 15.f;
            p2.z += 15.f;

            DX8::screen->RenderLine3D(p1, p2, Color::FireRed());
            DX8::screen->RenderCircle3D(p1, 50.f, 16, Color::ColdBlue(), true, true);
        }

        i++;
    }
}

void RouteCreator::OnRenderScreen()
{
    static BYTE alpha = 255;
    static bool flip = true;
    if (!flip)
    {
        alpha += 15;
        if (alpha >= 255)
            flip = true;
    }
    else
    {
        alpha -= 15;
        if (alpha <= 0)
            flip = false;
    }
    alpha = std::clamp(alpha, BYTE(0), BYTE(255));

    ImGui::PushFont(DX8::Font::verdana_32);
    if (recording)
    {
        DX8::screen->DrawCircleFilled({ 50.f, 50.f }, 16.f, 24, Color::Red(alpha));
        DX8::screen->DrawCircle({ 50.f, 50.f }, 16.f, 24, Color::Black(alpha));
        DX8::screen->DrawString({ 50.f + 16.f + 10.f, 50.f }, (DWORD)EStrFlag::CENTER_Y, Color::Red(), "Recording route");
    }
    else if (playing)
    {
        DX8::screen->DrawTriangleFilled(
            { 
                Vector2D(50.f - 16.f, 50.f - 16.f),
                Vector2D(50.f - 16.f, 50.f - 16.f),//TODO
            }, Color::Red(alpha));
        DX8::screen->DrawCircle({ 50.f, 50.f }, 16.f, 24, Color::Black(alpha));
        DX8::screen->DrawString({ 50.f + 16.f + 10.f, 50.f }, (DWORD)EStrFlag::CENTER_Y, Color::Red(), "Recording route");
    }
    ImGui::PopFont();
}

bool RouteCreator::IsValid(Entity* entity)
{
    return false;
}

bool RouteCreator::OnSend(BYTE* packet, int length)
{
    if (!packet || length <= 1 || !recording)
        return true;

    int packet_id = *packet;
    static bool is_chat_packet = false;
    static BYTE chat_type = EChatType::CHAT_TYPE_COMMAND;

    switch (packet_id)
    {
    case HEADER_CG_CHAT:
    {
        //Chat packet is spleet into 2 packets, first is the header, second is the message
        TPacketCGChat* p = (TPacketCGChat*)packet;
        chat_type = p->type;
        is_chat_packet = true;
        break;
    }
    case HEADER_CG_ON_CLICK:
    {
        TPacketCGOnClick* p = (TPacketCGOnClick*)packet;
        Entity* ent = G::game->GetEntity(p->vid);
        OnClickNPC(!ent ? 0 : ent->GetVirtualNumber());
        break;
    }
    case HEADER_CG_ITEM_USE:
    {
        TPacketCGItemUse* p = (TPacketCGItemUse*)packet;
        auto* item_data = m_local->GetItemData(p->pos.cell);
        auto item = G::game->GetItem(item_data->GetName(), false);
		OnUseItem(item.first);
		break;
    }
    case HEADER_CG_ITEM_USE_TO_ITEM:
    {
		TPacketCGItemUseToItem* p = (TPacketCGItemUseToItem*)packet;
        auto* src_item_data = m_local->GetItemData(p->source_pos.cell);
        auto src_item = G::game->GetItem(src_item_data->GetName(), false);
        auto* dst_item_data = m_local->GetItemData(p->target_pos.cell);
        auto dst_item = G::game->GetItem(dst_item_data->GetName(), false);
        OnUseItemToAnotherItem(src_item.first, dst_item.first);
        break;
    }
    case HEADER_CG_ITEM_PICKUP:
    {
        TPacketCGItemPickUp* p = (TPacketCGItemPickUp*)packet;
        auto vnum = G::game->GetGroundItemMapPtr()->find(p->vid)->second->GetVirtualNumber();
		OnPickupItem(vnum);
		break;
    }
    case HEADER_CG_GIVE_ITEM:
    {
        TPacketCGGiveItem* p = (TPacketCGGiveItem*)packet;
		VNUM vnum = G::game->GetEntity(p->dwTargetVID)->GetVirtualNumber();
        auto* item_data = m_local->GetItemData(p->ItemPos.cell);
        auto item = G::game->GetItem(item_data->GetName(), false);
		OnGiveItem(p->byItemCount, item.first, vnum);
		break;
    }
    case HEADER_CG_REFINE:
    {
        TPacketCGRefine* p = (TPacketCGRefine*)packet;
        OnRefine(p->pos, p->type);
        break;
    }
    case HEADER_CG_SCRIPT_BUTTON:
    {
        TPacketCGScriptButton* p = (TPacketCGScriptButton*)packet;
		OnScriptButton(p->idx);
		break;
    }
    case HEADER_CG_QUEST_CONFIRM:
    {
        TPacketCGQuestConfirm* p = (TPacketCGQuestConfirm*)packet;
        OnQuestConfirm(p->answer, p->requestPID);
        break;
    }
    case HEADER_CG_QUEST_INPUT_STRING:
    {
		TPacketCGQuestInputString* p = (TPacketCGQuestInputString*)packet;
		OnQuestInputString(p->szString);
		break;
	}
    case HEADER_CG_SCRIPT_ANSWER:
    {
        TPacketCGScriptAnswer* p = (TPacketCGScriptAnswer*)packet;
        OnScriptAnswer(p->answer);
		break;
	}
    case HEADER_CG_SCRIPT_SELECT_ITEM:
    {
        TPacketCGScriptSelectItem* p = (TPacketCGScriptSelectItem*)packet;
        OnScriptSelectItem(p->selection);
        break;
    }
    default:
    {
        //Chat packet message is handled here
        if (is_chat_packet)
        {
            is_chat_packet = false;
			OnSendChatMsg((char*)packet, chat_type);
            chat_type = EChatType::CHAT_TYPE_COMMAND;
        }
    }
    }

    return true;
}

bool RouteCreator::OnRecv(BYTE* packet, int length)
{
    if (playing && packet)
    {
        if (*packet == HEADER_GC_WARP && length == 15)
        {
            //DROP HEADER_GC_SCRIPT
            PRINTF_ACTION("Warp\n");
        }

        //TODO rest
        else if (*packet == HEADER_GC_SCRIPT && length == 6)
        {
            //has_opened_quest = false;
            PRINTF_ACTION("Quest opened\n");
        }
    }

    return true;
}

bool RouteCreator::ShouldOpenScriptWindow()
{
    return !playing;
}

void RouteCreator::StartRecording()
{
    m_local->GetPixelPosition(&m_local_last_pos);

    RecordData rd;
    m_local_last_pos = m_local_pos;
    rd.m_pos = m_local_pos;
    rd.m_action = RecordMainAction::START;
    m_records.push_back(rd);
}

void RouteCreator::StopRecording()
{
    RecordData rd;
    rd.m_pos = m_local_last_pos;
    rd.m_action = RecordMainAction::END;
    m_records.push_back(rd);
}

void RouteCreator::Play(bool reverse)
{
    if (m_records.empty())
        return;

    if (!reverse)
    {
        const auto& r = m_records.front();
        if (PerformAction(r))
            m_records.pop_front();
        else
        {
            //TODO: perform side action
        }
    }
    else
    {
        const auto& r = m_records.back();
        if (PerformAction(r))
            m_records.pop_back();
        else
        {
            //TODO: perform side action
        }
    }
}

void RouteCreator::OnClickNPC(VNUM npc_vnum)
{
    RecordData rd;
    rd.m_npc_vnum = npc_vnum;
    rd.m_pos = m_local_pos;
    rd.m_action = RecordMainAction::CLICK_NPC; 
    m_records.push_back(rd);
    PRINTF_RECORD("NPC Clicked: %d\n", npc_vnum);
}

void RouteCreator::OnGiveItem(BYTE item_count, VNUM item_vnum, VNUM npc_vnum)
{
    RecordData rd; 
    rd.m_npc_vnum = npc_vnum;
    rd.m_pos = m_local_pos;
    rd.m_item_count = item_count;
    rd.m_item_vnum = item_vnum;
    rd.m_action = RecordMainAction::GIVE_ITEM;
    m_records.push_back(rd);
    PRINTF_RECORD("Gave %d items of vnum %d to NPC %d\n", (int)item_count, item_vnum, npc_vnum);
}

void RouteCreator::OnUseItem(VNUM item_vnum)
{
    RecordData rd;
    rd.m_pos = m_local_pos;
    rd.m_item_vnum = item_vnum;
    rd.m_action = RecordMainAction::USE_ITEM;
    m_records.push_back(rd);
    PRINTF_RECORD("Used item %d\n", item_vnum);
}

void RouteCreator::OnUseItemToAnotherItem(VNUM item_vnum, VNUM another_item_vnum)
{
    RecordData rd;
    rd.m_pos = m_local_pos;
    rd.m_item_vnum = item_vnum;
    rd.m_another_item_vnum = another_item_vnum;
    rd.m_action = RecordMainAction::USE_ITEM_TO_ANOTHER_ITEM;
    m_records.push_back(rd);
    PRINTF_RECORD("Used item %d on item %d\n", item_vnum, another_item_vnum);
}

void RouteCreator::OnSendChatMsg(const std::string& msg, BYTE type)
{
    RecordData rd;
    rd.m_pos = m_local_pos;
    rd.m_chat = msg;
    rd.m_chat_type = type;
    rd.m_action = RecordMainAction::SEND_CHAT_MSG;
    m_records.push_back(rd);
    PRINTF_RECORD("Sent chat message: [%d]%s\n", (int)type, msg.c_str());
}

void RouteCreator::OnRefine(size_t slot, BYTE type)
{
    RecordData rd;
    rd.m_pos = m_local_pos;
    rd.m_item_refine_slot = slot;
    rd.m_item_refine_type = type;
    rd.m_action = RecordMainAction::REFINE;
    m_records.push_back(rd);
    PRINTF_RECORD("Refined item in slot %d with type %d\n", (int)slot, (int)type);
}

void RouteCreator::OnScriptButton(size_t button_id)
{
	PRINTF_RECORD("Clicked script button %d\n", (int)button_id);
}

void RouteCreator::OnQuestConfirm(BYTE answer, DWORD request_pid)
{
    PRINTF_RECORD("Confirmed quest with answer: %d, request_pid: %d\n", (int)answer, request_pid);
}

void RouteCreator::OnScriptSelectItem(DWORD selection)
{
    PRINTF_RECORD("Selected item: %d\n", (int)selection);
}

void RouteCreator::OnQuestInputString(const std::string& str)
{
    PRINTF_RECORD("Quest input string: %s\n", str.c_str());
}

void RouteCreator::OnScriptAnswer(BYTE answer)
{
    RecordData rd;
    rd.m_pos = m_local_pos;
    rd.m_answer_button = answer;
    rd.m_action = RecordMainAction::ANSWER_QUEST;
    m_records.push_back(rd);
    PRINTF_RECORD("Answered quest with answer: %d\n", (int)answer);
}

void RouteCreator::OnPickupItem(VNUM vnum)
{
    RecordData rd;
    rd.m_pos = m_local_pos;
    rd.m_item_pickup_vnum = vnum; 
    rd.m_action = RecordMainAction::PICKUP_ITEM;
    m_records.push_back(rd);
    PRINTF_RECORD("Picked up item %d\n", vnum);
}

bool RouteCreator::PerformAction(const RecordData& record)
{
    if (m_cur_map_name != record.m_map_name)
        return false;

    switch (record.m_action)
    {
        case RecordMainAction::START:
        {
            PRINTF_ACTION("Starting route\n");
            return true;
        }
        case RecordMainAction::END:
        {
            PRINTF_ACTION("Ending route\n");
			return true;
		}
        case RecordMainAction::GO_TO:
        {
            return MoveToDestination(record.m_pos);
        }
        case RecordMainAction::SEND_CHAT_MSG:
        {
            PRINTF_ACTION("Sending chat msg: [%d]%s\n", (int)record.m_chat_type, record.m_chat.c_str());
            return G::game->GetNetworkMgr()->SendChatPacket(record.m_chat.c_str(), (EChatType)record.m_chat_type);
        }
        case RecordMainAction::ANSWER_QUEST:
        {
            PRINTF_ACTION("Answering quest %d\n", (int)record.m_answer_button);
            return G::game->GetNetworkMgr()->SendScriptAnswerPacket(static_cast<BYTE>(record.m_answer_button));
        }
        case RecordMainAction::CLICK_NPC:
        {
            Entity* npc = G::game->GetEntity(VNUM(record.m_npc_vnum));
            if (!npc) return false;
            PRINTF_ACTION("Clicking NPC %d\n", record.m_npc_vnum);
            return G::game->GetNetworkMgr()->SendOnClickPacket(npc->GetVID());
        }
        case RecordMainAction::GIVE_ITEM:
        {
            int slot = m_local->GetItemSlot(record.m_item_vnum, 1);
            if (slot <= -1) return false;
            PRINTF_ACTION("Giving item %d from slot %d to %d\n", record.m_item_vnum, slot, record.m_npc_vnum);
            return G::game->GetNetworkMgr()->SendGiveItemPacket(record.m_npc_vnum, TItemPos(1, slot), static_cast<BYTE>(record.m_item_count));
        }
        case RecordMainAction::USE_ITEM:
        {
            int slot = m_local->GetItemSlot(record.m_item_vnum, 1);
            if (slot <= -1) return false;
            PRINTF_ACTION("Using item %d from slot %d\n", record.m_item_vnum, slot);
            return G::game->GetNetworkMgr()->SendItemUsePacket(TItemPos(1, slot));
		}
        case RecordMainAction::USE_ITEM_TO_ANOTHER_ITEM:
        {
            int slot = m_local->GetItemSlot(record.m_item_vnum, 1);
			if (slot <= -1) return false;
			int another_slot = m_local->GetItemSlot(record.m_another_item_vnum, 1);
			if (another_slot <= -1) return false;
            PRINTF_ACTION("Using item %d from slot %d to another item %d from slot %d\n", record.m_item_vnum, slot, record.m_another_item_vnum, another_slot);
			return G::game->GetNetworkMgr()->SendItemUseToItemPacket(TItemPos(1, slot), TItemPos(1, another_slot));
        }
        case RecordMainAction::REFINE:
        {
            PRINTF_ACTION("Refining item from slot %d, type %d\n", record.m_item_refine_slot, record.m_item_refine_type);
			return G::game->GetNetworkMgr()->SendRefinePacket(static_cast<BYTE>(record.m_item_refine_slot), record.m_item_refine_type);
		}
        default: //NONE and shiet
        {
			return true;
		}
    }
}

bool RouteCreator::MoveToDestination(const Vector& dest, float approx)
{
    if (tp_insead_of_walk)
    {
        if (m_local->ShortBlinkTo(dest, false, [](const Vector& pos) {}, false, false))
        {
            m_local->SCRIPT_SetPixelPosition(dest);
            PRINTF_ACTION("TP to waypoint\n");
            return true;
        }
        return false;
    }
    else
    {
        if (!m_local->IsCloseToPosition(dest, approx))
        {
			m_local->MoveToDestPixelPosition(dest);
			return false;
		}
        PRINTF_ACTION("Going to waypoint\n");
        return true;
    }
}

RouteCreator::RecordData::RecordData()
{
    m_time = Clock::GetCurTime();
    m_map_name = G::game->GetBackground()->GetCurrentMapName();
    m_action = RecordMainAction::NONE;
    m_side_action = RecordMainAction::NONE;
}
