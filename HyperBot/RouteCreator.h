#pragma once

class RouteCreator : public ICheatBase<Entity>
{
public:
	virtual ~RouteCreator();
	virtual void Init() override;
	virtual void OnMenuFrame() override;
	virtual void PreIteration(Player* local) override;
	virtual void OnIteration(Entity* entity) override;
	virtual void PostIteration() override;
	virtual void OnRenderWorld() override;
	virtual void OnRenderScreen() override;
	virtual bool IsValid(Entity* entity) override;

	enum class RecordMainAction
	{
		START,							
		NONE,							
		GO_TO,							
		CLICK_NPC,
		GIVE_ITEM,
		USE_ITEM,
		USE_ITEM_TO_ANOTHER_ITEM,
		SEND_CHAT_MSG,
		REFINE,
		ANSWER_QUEST,
		PICKUP_ITEM,
		END,							
	};

	enum class RecordSideAction
	{
		FIGHT,
		WAIT,
	};

	bool OnSend(BYTE* packet, int length);
	bool OnRecv(BYTE* packet, int length);
	bool ShouldOpenScriptWindow();

private:
	void StartRecording();
	void StopRecording();
	void Play(bool reverse);

	void OnClickNPC(VNUM npc_vnum);
	void OnGiveItem(BYTE item_count, VNUM item_vnum, VNUM npc_vnum);
	void OnUseItem(VNUM item_vnum);
	void OnUseItemToAnotherItem(VNUM item_vnum, VNUM another_item_vnum);
	void OnSendChatMsg(const std::string& msg, BYTE type);
	void OnRefine(size_t slot, BYTE type);
	void OnScriptButton(size_t button_id);
	void OnQuestConfirm(BYTE answer, DWORD request_pid);
	void OnScriptSelectItem(DWORD selection);
	void OnQuestInputString(const std::string& str);
	void OnScriptAnswer(BYTE answer);
	void OnPickupItem(VNUM vnum);

	struct RecordData
	{
		RecordData();
		float				m_time{};
		Vector				m_pos{};
		size_t				m_item_refine_slot{};
		BYTE				m_item_refine_type{};
		VNUM				m_item_pickup_vnum{};
		VNUM				m_npc_vnum{};
		VNUM				m_item_vnum{};
		VNUM				m_another_item_vnum{};
		size_t				m_item_count{};
		size_t				m_answer_button{};
		BYTE				m_chat_type{};
		std::string			m_chat{};
		std::string			m_map_name{};

		RecordMainAction	m_action{};
		RecordMainAction	m_side_action{};	//Need to be set after recording - in case of being unable to pickup item (so try to farm in mean time)
	};

	Player*					m_local;
	Vector					m_local_pos;
	Vector					m_local_last_pos;
	std::string				m_cur_map_name;
	std::deque<RecordData>	m_records;

	bool PerformAction(const RecordData& record);
	bool MoveToDestination(const Vector& dest, float approx = 100.f);
};

