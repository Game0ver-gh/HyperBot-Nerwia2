#include "pch.h"
#undef max
#undef min



HyperGUI::HyperGUI()
{
    m_easing_in = GetEasingFunction(easing_functions::EaseOutExpo);
	m_easing_out = GetEasingFunction(easing_functions::EaseInExpo);
	m_anim_speed = 0.4f;
	m_window_pos = { 20.f, 80.f };
    m_window_size = { 0, 0 };
    m_window_max_size = { 600, 600 };
    m_cur_option = 0;
}


bool HyperGUI::Slide(float& value, int min, int max, float sec_duration, int id, double(*easingFunction)(double))
{
	if (static_cast<size_t>(id) >= m_animation_step.size())
        m_animation_step.resize(id + 1);
    
	const float steps = 60.f * sec_duration;
	auto& t = m_animation_step[id];
    t.second = t.first++ / steps;
	t.second = std::clamp(t.second, 0.f, steps);
    
	auto easing = easingFunction(t.second);
    if (easing >= 1.0f || t.first >= steps)
        return true;
    
    value = easing * (max - min) + min;
    return false;
}

void HyperGUI::ClearAnimBuffer(const std::vector<size_t>& ids)
{
    for (const auto& id : ids)
    {
		if (id >= m_animation_step.size())
			continue;
        
        m_animation_step[id] = { 0, 0.f };
    }
}

void HyperGUI::SetInitialPosition()
{
    static bool init_pos = true;
    if (init_pos)
    {
        ImGui::SetNextWindowPos(m_window_pos);
        ImGui::SetNextWindowSize(m_window_size);
        init_pos = false;
    }
}

void HyperGUI::RenderDebug()
{
    static float f = 0.0f;
    static bool demo = false;
    static std::array<bool, 256> send_filter;
    static std::array<bool, 256> recv_filter;
    static std::array<std::pair<DWORD, std::string>, 256> send_list;
    static std::array<std::pair<DWORD, std::string>, 256> recv_list;
    static bool init = true;
    if (init)
    {
        init = false;
        for (int i = 0; i < UCHAR_MAX + 1; ++i)
        {
            send_filter[i] = true;
            recv_filter[i] = true;
            send_list[i] = { i, F::packet_sniffer->GetHeaderName(i, PACKET_TO_SERVER) };
            recv_list[i] = { i, F::packet_sniffer->GetHeaderName(i, PACKET_TO_CLIENT) };
        }
        std::sort(recv_list.begin(), recv_list.end(), [](const auto& a, const auto& b) {
			return a.second < b.second;
		    });

        std::sort(send_list.begin(), send_list.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;
            });
    }

    ImGui::CustomCheckbox("[DBG] Info box", &G::cfg[EBool::GUI_SHOW_INFO_BOX]);
    ImGui::CustomCheckbox("[DBG] Print raw packets", &G::cfg[EBool::DBG_PRINT_PACKETS]);

    ImGui::SetNextItemWidth(ImGui::GetWindowSize().x / 3 - 25.f);
    if (ImGui::BeginCombo("##PACKET_FILTER_SEND", "Filter send packet:"))
    {
        for (size_t i = 0; i < send_filter.size(); i++)
        {
            if (ImGui::Selectable(send_list[i].second.c_str(), &send_filter[send_list[i].first], ImGuiSelectableFlags_DontClosePopups))
                F::packet_sniffer->SetSendFilter(send_filter);
        }
        ImGui::EndCombo();
    } ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetWindowSize().x / 3 - 25.f);
    if (ImGui::BeginCombo("##PACKET_FILTER_RECV", "Filter recv packet:"))
    {
        for (size_t i = 0; i < recv_filter.size(); i++)
        {
			if (ImGui::Selectable(recv_list[i].second.c_str(), &recv_filter[recv_list[i].first], ImGuiSelectableFlags_DontClosePopups))
				F::packet_sniffer->SetRecvFilter(recv_filter);
		}
		ImGui::EndCombo();
    }ImGui::SameLine();
    if (ImGui::CustomButton("Clear filters"))
    {
		for (auto& b : send_filter)
			b = false;
		for (auto& b : recv_filter)
			b = false;
        F::packet_sniffer->SetRecvFilter(recv_filter);
        F::packet_sniffer->SetSendFilter(send_filter);
	}


    ImGui::CustomSliderFloat("[DBG] Test slider", &f, 0.f, 100.f, "%.2f");
    ImGui::CustomCheckbox("[DBG] Show demo", &demo);
    if (demo) ImGui::ShowDemoWindow();

    if (ImGui::CustomButton("Save config")) G::cfg.WriteConfigToFile();
    if (ImGui::CustomButton("Load config")) G::cfg.LoadConfigFromFile(); 
    if (ImGui::CustomButton("Reset config")) G::cfg.RegenerateConfigFile();

    static bool show_color_editor = false;
    ImGui::CustomCheckbox("[DBG] Color edit", &show_color_editor);
    if (show_color_editor)
    {
        m_window_size = { 900, 600 };
        m_window_max_size = { 900, 600 };
        static unsigned int copied_colors[ImGuiCustomCol_COUNT];
        static auto make_copy = true;
        if (make_copy)
        {
			auto* setts = ImGui::GetCustomSettings();
			for (int i = 0; i < ImGuiCustomCol_COUNT; ++i)
                copied_colors[i] = ImGui::GetCustomColorU32((ImGuiCustomCol_)i);
			make_copy = false;
		}

        auto* setts = ImGui::GetCustomSettings();
		for (int i = 0; i < ImGuiCustomCol_COUNT; ++i)
		{
			ImGui::ColorEdit3(ImGui::GetCustomColorName(i).c_str(), &setts->colors[i].x);
            if (ImGui::GetCustomColorU32(i) != copied_colors[i])
            {
                ImGui::SameLine();
                if (ImGui::Button("Reset"))
                {
                    auto u_col = ImGui::ColorConvertU32ToFloat4(copied_colors[i]);
                    setts->colors[i] = u_col;
				}
            }
		}
    }

    if (ImGui::Button("Copy to clipboard"))
    {
        auto* setts = ImGui::GetCustomSettings();
        ImGui::LogToClipboard();
        ImGui::LogText("ImVec4* colors = ImGui::GetCustomSettings()->colors;\n");
        for (int i = 0; i < ImGuiCustomCol_COUNT; ++i)
        {
            ImGui::LogText("colors[ImGuiCustomCol_%s] = ImVec4(%.2ff, %.2ff, %.2ff, 1.00ff);\n",
                ImGui::GetCustomColorName((ImGuiCustomCol_)i).c_str(),
                setts->colors[i].x, setts->colors[i].y, setts->colors[i].z);
        }
        ImGui::LogFinish();
    }
}

void RenderPlayers()
{

}

bool HyperGUI::Render()
{
	if (auto* network = G::game->GetNetworkMgr(); !network || !network->IsConnected())
        return false;

    SetInitialPosition();
    
    AnimateMainMenu(DX8::is_gui_active);

    DX8::is_hovering_gui = ImGui::IsMouseHoveringRect(
        m_window_pos, { m_window_pos.x + m_window_size.x, m_window_pos.y + m_window_size.y }, false);

    //ImGui::CustomAnimationTriggerSet(m_window_size.x > 50.f && m_window_size.y > 50.f);
    
    if (m_window_size.x > 0.f && m_window_size.y > 0.f)
    {
        ImGui::SetNextWindowSize(m_window_size);
        ImGui::CustomBegin("HyperBot", &m_cur_option, 
            {
                DX8::Icon::sword,
                DX8::Icon::pickup,
                DX8::Icon::route,
                DX8::Icon::misc,
                DX8::Icon::players,
                DX8::Icon::dbg,
                DX8::Icon::exit
            },
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse,
            DX8::Icon::bg_black, DX8::Icon::logo4);
        {
            m_window_pos = ImGui::GetWindowPos();
			ImGui::SetCursorPosX(100.f); ImGui::SetCursorPosY(100.f);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0.7f));
			ImGui::BeginChild("##tabs", { 0, 0 }, true);
			{
                switch (m_cur_option)
                {
                    case 0: F::wait_hack->OnMenuFrame(); break;
                    case 1: F::auto_pickup->OnMenuFrame(); break;
                    case 2: F::route_creator->OnMenuFrame();  break;
                    case 3: F::misc_hacks->OnMenuFrame(); break;
                    case 4: F::game_informer->OnMenuFrame(); break;
                    case 5: RenderDebug(); break;
				    case 6: F::should_exit = true; break;
                }
                
			}
            ImGui::EndChild();
            ImGui::PopStyleColor();
        }
        ImGui::CustomEnd();
    }
    return DX8::is_gui_active;
}

void HyperGUI::AnimateMainMenu(bool should_open)
{
    static float s_x = 0, s_y = 0, s_a = 0;
    static bool is_oppening = false, is_closing = false;
    auto& alpha = ImGui::GetStyle().Alpha;

	if (m_window_size.x <= 0.f && m_window_size.y <= 0.f)
        alpha = 0.0f;
    
    if (should_open || (is_oppening))
    {
        is_oppening = true;
        bool x = Slide(m_window_size.x, 0, m_window_max_size.x, m_anim_speed, 0, m_easing_in);
        bool y = Slide(m_window_size.y, 0, m_window_max_size.y, m_anim_speed, 1, m_easing_in);
		bool a = Slide(alpha, 0.f, 1.f, m_anim_speed, 2, m_easing_in);

        if (x && y && a)
        {
            s_x = s_y = s_a = 0.f;
            is_oppening = false;
            ClearAnimBuffer({ 3, 4, 5 });
        }
        else if (!should_open && is_oppening)
        {
            s_x = s_y = s_a = 0.f;
            is_oppening = false;
            ClearAnimBuffer({ 3, 4, 5 });
        }
    }
    
    if (!should_open || (is_closing))
    {
		is_closing = true;
        bool x = Slide(s_x, 0, m_window_max_size.x, m_anim_speed, 3, m_easing_out);
        bool y = Slide(s_y, 0, m_window_max_size.y, m_anim_speed, 4, m_easing_out);
		bool a = Slide(s_a, 0.f, 1.f, m_anim_speed * 3.f/4.f, 5, m_easing_out);
        alpha -= s_a;
        m_window_size.x -= s_x;
        m_window_size.y -= s_y;
        if (x && y)
        {
            s_x = s_y = 0.f;
            is_closing = false;
            ClearAnimBuffer({ 0, 1, 2 });
        }
		else if (should_open && is_closing)
		{
            s_x = s_y = 0.f;
			is_closing = false;
			ClearAnimBuffer({ 0, 1, 2 });
		}
    }

    m_window_size.x = std::clamp(m_window_size.x, 0.f, m_window_max_size.x);
    m_window_size.y = std::clamp(m_window_size.y, 0.f, m_window_max_size.y);
    alpha = std::clamp(alpha, 0.0f, 1.0f);
}
