#pragma once

class GameInformer : public ICheatBase<Entity>
{
public:
	virtual ~GameInformer() {};
	virtual void Init() override;
	virtual void OnMenuFrame() override;
	virtual void PreIteration(Player* local) override;
	virtual void OnIteration(Entity* entity) override;
	virtual void PostIteration() override;
	virtual void OnRenderWorld() override;
	virtual void OnRenderScreen() override;
	virtual bool IsValid(Entity* entity) override;

	bool IsLocalSpectated();
	
private:
	struct MeetPlayersData
	{
		std::string m_name;
		int m_level;
		bool m_is_gm;
		int m_empire;
		RaceType m_race;
		bool operator<(const MeetPlayersData& other) const
		{
			return m_name < other.m_name;
		}
	};

	Player*						m_local;
	bool						m_is_spectated;
	std::vector<Entity*>		m_gms;
	std::vector<Entity*>		m_mobs;
	std::vector<Entity*>		m_players;
	std::vector<Entity*>		m_metins;
	std::set<MeetPlayersData>	m_all_meet_players;

	void ShowLegacyInfoBox();
};

