#pragma once

class AutoItemPickup : public ICheatBase<std::pair<VID, GroundItemData*>>
{
public:
	virtual ~AutoItemPickup() {};
	virtual void Init() override;
	virtual void OnMenuFrame() override;
	virtual void PreIteration(Player* local) override;
	virtual void OnIteration(std::pair<VID, GroundItemData*>* item) override;
	virtual void PostIteration() override;
	virtual void OnRenderWorld() override;
	virtual void OnRenderScreen() override;
	virtual bool IsValid(std::pair<VID, GroundItemData*>* item) override;
	
private:
	struct Target
	{
		Target() : m_distance(-1.0f), m_target(nullptr), m_vid(0) {}
		Target(GroundItemData* target, bool hit_send, const Vector& pos, const Vector& bound_box_min, const Vector& bound_box_max, float dist)
			: m_target(target), m_position(pos), m_bound_box_min(bound_box_min), m_bound_box_max(bound_box_max), m_distance(dist), m_vid(0) {}

		VID						m_vid;
		GroundItemData*			m_target;
		Vector					m_position;
		Vector					m_bound_box_min;
		Vector					m_bound_box_max;
		float					m_distance;
	};

	Player*						m_local;
	Vector						m_local_pos;
	std::vector<Target>			m_ap_targets;				//Pickup target items
	GameMainInstance::ItemMap	m_items_whitelist;			//Map of item whitelist [vid, ItemData*]
	std::vector<std::string>	m_items_whitelist_display;	//List of item whitelist names
	std::vector<std::string>	m_items_display_filtered;	//List of item names

	float						m_ap_distance;
	float						m_ap_speed;
	bool						m_is_ap_ready;
	bool						m_is_ranged;
	float						m_ap_time;

	bool PickUpTarget(const Target& target);
	void AddNewTarget(std::pair<VID, GroundItemData*>* item);
	
	void LoadItemsWhitelistFile();
	void WriteItemsWhitelistFile();
	void DumpItemsWhitelist();
};

