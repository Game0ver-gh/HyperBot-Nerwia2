#pragma once

template <typename EntityIface>
class ICheatBase
{
public:
	virtual ~ICheatBase() {};

	//Called only once when cheat is loaded
	virtual void Init() = 0;

	//Called when specific menu tab is selected
	virtual void OnMenuFrame() = 0;

	//Called before iteration over entities
	virtual void PreIteration(Player* local) = 0;

	//Called for each entity
	virtual void OnIteration(EntityIface* entity) = 0;

	//Called after iteration over entities
	virtual void PostIteration() = 0;

	//Called on each RenderGame() frame
	virtual void OnRenderWorld() = 0;

	//Called on each EndScene() frame
	virtual void OnRenderScreen() = 0;
	
	//Called when filtering entity is needed
	virtual bool IsValid(EntityIface* entity) = 0;
};

