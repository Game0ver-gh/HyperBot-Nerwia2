#pragma once

class HyperGUI
{
public:
	HyperGUI();
	bool Render();

private:
	int m_cur_option;
	std::vector<std::pair<int, float>> m_animation_step;
	ImVec2 m_window_size;
	ImVec2 m_window_pos;
	ImVec2 m_window_max_size;
	float m_anim_speed;
	easingFunction m_easing_in;
	easingFunction m_easing_out;

	void AnimateMainMenu(bool should_open);
	bool Slide(float& value, int min, int max, float sec_duration, int id, double(*easingFunc)(double));
	void ClearAnimBuffer(const std::vector<size_t>& ids);
	void SetInitialPosition();
	void RenderDebug();
};