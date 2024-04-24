#pragma once

class MiscHacks : public ICheatBase<Entity>
{
public:
	virtual ~MiscHacks() {};
	virtual void Init() override;
	virtual void OnMenuFrame() override;
	virtual void PreIteration(Player* local) override;
	virtual void OnIteration(Entity* entity) override;
	virtual void PostIteration() override;
	virtual void OnRenderWorld() override;
	virtual void OnRenderScreen() override;
	virtual bool IsValid(Entity* entity) override;

private:
	Player* m_local;
	Vector m_local_pos;
	//std::array<LPDIRECT3DTEXTURE8, 8> m_skill_textures;
	std::vector<SkillData*> m_skills;

	void RunSpeedBoost();
	void UnlockCamerZoom();
	void DisableFog();
	void AutoUsePassiveSkills();
};

