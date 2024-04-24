#pragma once

class WaitHack : public ICheatBase<Entity>
{
public:
	virtual ~WaitHack();
	virtual void Init() override;
	virtual void OnMenuFrame() override;
	virtual void PreIteration(Player* local) override;
	virtual void OnIteration(Entity* entity) override;
	virtual void PostIteration() override;
	virtual void OnRenderWorld() override;
	virtual void OnRenderScreen() override;
	virtual bool IsValid(Entity* entity) override;
	
	struct Target
	{
		Target() : m_distance(-1.0f), m_target(nullptr) {}
		Target(Entity* target, bool hit_send, const Vector& pos, const Vector& bound_box_min, const Vector& bound_box_max, float dist)
			: m_target(target), m_position(pos), m_bound_box_min(bound_box_min), m_bound_box_max(bound_box_max), m_distance(dist) {}
		
		Entity*			m_target;
		Vector			m_position;
		Vector			m_bound_box_min;
		Vector			m_bound_box_max;
		float			m_distance;
	};

	const std::vector<Target>& GetTargetList();

private:
	Player*				m_local;
	Vector				m_local_pos;
	std::vector<Target> m_hit_targets;
	
	float				m_wh_distance;
	float				m_attack_speed;
	bool				m_is_attack_ready;
	bool				m_is_ranged;
	float				m_attack_time;
	
	bool AttackTarget(const Target& target);
	void AddNewTarget(Entity* entity);
};

