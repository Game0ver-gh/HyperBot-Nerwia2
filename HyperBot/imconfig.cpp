#include "pch.h"
#include "imconfig.h"
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#include <time.h>
#include <vector>
#include <array>

#pragma warning(disable : 6237)
#pragma warning(disable : 4099)

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui_internal.h"

// System includes
#include <ctype.h>      // toupper
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>     // intptr_t
#else
#include <stdint.h>     // intptr_t
#endif

#ifdef _DEBUG
#define DRAW_BORDER(bb) window->DrawList->AddRect(bb.Min, bb.Max, ImColor(255, 0, 212, 128)) //pink
#else
#define DRAW_BORDER(bb)
#endif


//-------------------------------------------------------------------------
// Warnings
//-------------------------------------------------------------------------

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4127)     // condition expression is constant
#pragma warning (disable: 4996)     // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#if defined(_MSC_VER) && _MSC_VER >= 1922 // MSVC 2019 16.2 or later
#pragma warning (disable: 5054)     // operator '|': deprecated between enumerations of different types
#endif
#pragma warning (disable: 26451)    // [Static Analyzer] Arithmetic overflow : Using operator 'xxx' on a 4 byte value and then casting the result to a 8 byte value. Cast the value to the wider type before calling operator 'xxx' to avoid overflow(io.2).
#pragma warning (disable: 26812)    // [Static Analyzer] The enum type 'xxx' is unscoped. Prefer 'enum class' over 'enum' (Enum.3).
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         // warning: unknown warning group 'xxx'                      // not all warnings are known by all Clang versions and they tend to be rename-happy.. so ignoring warnings triggers new warnings on some configuration. Great!
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                // warning: unknown warning group 'xxx'
#pragma clang diagnostic ignored "-Wold-style-cast"                 // warning: use of old-style cast                            // yes, they are more terse.
#pragma clang diagnostic ignored "-Wfloat-equal"                    // warning: comparing floating point with == or != is unsafe // storing and comparing against same constants (typically 0.0f) is ok.
#pragma clang diagnostic ignored "-Wformat-nonliteral"              // warning: format string is not a string literal            // passing non-literal to vsnformat(). yes, user passing incorrect format strings can crash the code.
#pragma clang diagnostic ignored "-Wsign-conversion"                // warning: implicit conversion changes signedness
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"  // warning: zero as null pointer constant                    // some standard header variations use #define NULL 0
#pragma clang diagnostic ignored "-Wdouble-promotion"               // warning: implicit conversion from 'float' to 'double' when passing argument to function  // using printf() is a misery with this as C++ va_arg ellipsis changes float to double.
#pragma clang diagnostic ignored "-Wenum-enum-conversion"           // warning: bitwise operation between different enumeration types ('XXXFlags_' and 'XXXFlagsPrivate_')
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"// warning: bitwise operation between different enumeration types ('XXXFlags_' and 'XXXFlagsPrivate_') is deprecated
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"  // warning: implicit conversion from 'xxx' to 'float' may lose precision
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpragmas"                          // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wformat-nonliteral"                // warning: format not a string literal, format string not checked
#pragma GCC diagnostic ignored "-Wclass-memaccess"                  // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#pragma GCC diagnostic ignored "-Wdeprecated-enum-enum-conversion"  // warning: bitwise operation between different enumeration types ('XXXFlags_' and 'XXXFlagsPrivate_') is deprecated
#endif

static const char* PatchFormatStringFloatToInt(const char* fmt)
{
    if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
        return "%d";
    const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
    const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
    if (fmt_end > fmt_start && fmt_end[-1] == 'f')
    {
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        if (fmt_start == fmt && fmt_end[0] == 0)
            return "%d";
        const char* tmp_format;
        ImFormatStringToTempBuffer(&tmp_format, NULL, "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
        return tmp_format;
#else
        IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
    }
    return fmt;
}

class Timer
{
public:
    explicit Timer() : last_time(GetCurTime()) {}
    static float GetCurTime() { return static_cast<float>(clock()) / 1000.f; }
    void Update() { last_time = GetCurTime(); }
    float GetTimeSinceUpdate() const { return GetCurTime() - last_time; }

private:
    float last_time;
};

enum ImGuiAnimState_
{
    TRANSITION_RIGHT,
    TRANSITION_LEFT,
    LEFT_STOP,
    RIGHT_STOP,
    UNDEFINED
};



#include <string>
std::string str[] =
{
    "TRANSITION_RIGHT",
    "TRANSITION_LEFT",
    "LEFT_STOP",
    "RIGHT_STOP",
    "UNDEFINED"
};

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis(0.0, 1.0);

struct CustomWidget
{
    CustomWidget() :
        color_button_state(false), animation_trigger(false), id(0),
        anim_state(ImGuiAnimState_::UNDEFINED), slider_thumb_pos(0.0f),
        picker_size(0.f, 0.f), picker_color(1,1,1,1){}
    Timer timer;
    ImVec4 picker_color;
    bool color_button_state;
    bool animation_trigger;
    float slider_thumb_pos;
    ImVec2 picker_size;
    Timer picker_timer;
    size_t id;
    ImGuiAnimState_ anim_state;
};

static CustomSettings settings;



const std::string& ImGui::GetCustomColorName(int ImGuiCustomCol_)
{
    static std::string color_names[] =
    {
        "ChildBg",
        "Border",
        "FrameEnabled",
        "FrameDisabled",
        "CheckboxBorderDisabled",
        "CheckboxBorderEnabled",
        "CheckboxThumbDisabled",
        "CheckboxThumbEnabled",
        "Text",
        "TextDisabled",
        "TextActive",
        "SliderThumbSmall",
        "SliderThumbBig",
        "SliderThumbLarge",
        "RailBorderDisabled",
        "RailBorderEnabled",
	};
	return color_names[ImGuiCustomCol_];
}

CustomSettings* ImGui::GetCustomSettings()
{
    return &settings;
}

void CustomSettings::UpdateColors()
{
    if (night_mode)
    {
        colors[ImGuiCustomCol_Border] = ImColor(255, 255, 255);
        colors[ImGuiCustomCol_ChildBg] = ImColor(28, 28, 28);
        colors[ImGuiCustomCol_Text] = ImColor(255, 255, 255);
        colors[ImGuiCustomCol_TextActive] = ImColor(255, 255, 255);
        colors[ImGuiCustomCol_TextDisabled] = ImColor(255, 255, 255);
        colors[ImGuiCustomCol_FrameEnabled] = ImColor(96, 205, 255);
        colors[ImGuiCustomCol_FrameDisabled] = ImColor(255, 255, 255, 139);

        //Slider
        colors[ImGuiCustomCol_RailBorderEnabled] = ImColor(96, 205, 255);
        colors[ImGuiCustomCol_RailBorderDisabled] = ImColor(255, 255, 255, 154);
        colors[ImGuiCustomCol_SliderThumbBig] = ImColor(69, 69, 69);
        colors[ImGuiCustomCol_SliderThumbSmall] = ImColor(96, 205, 255);
        colors[ImGuiCustomCol_SliderThumbLarge] = ImColor(69, 69, 69, 154);

        //Checkbox
        colors[ImGuiCustomCol_CheckboxBorderDisabled] = ImColor(255, 255, 255, 154);
        colors[ImGuiCustomCol_CheckboxBorderEnabled] = ImColor(96, 205, 255);
        colors[ImGuiCustomCol_CheckboxThumbDisabled] = ImColor(255, 255, 255, 200);
        colors[ImGuiCustomCol_CheckboxThumbEnabled] = ImColor(0, 0, 0);
    }
    else
    {
        colors[ImGuiCustomCol_Border] = ImColor(0, 0, 0);
        colors[ImGuiCustomCol_ChildBg] = ImColor(250, 250, 250);
        colors[ImGuiCustomCol_Text] = ImColor(0, 0, 0);
        colors[ImGuiCustomCol_TextActive] = ImColor(0, 0, 0);
        colors[ImGuiCustomCol_TextDisabled] = ImColor(0, 0, 0);
        colors[ImGuiCustomCol_FrameEnabled] = ImColor(0, 95, 184);
        colors[ImGuiCustomCol_FrameDisabled] = ImColor(0.6f, 0.6f, 0.6f);

        //Slider
        colors[ImGuiCustomCol_RailBorderEnabled] = ImColor(0, 95, 184);
        colors[ImGuiCustomCol_RailBorderDisabled] = ImColor(0.6f, 0.6f, 0.6f);
        colors[ImGuiCustomCol_SliderThumbBig] = ImColor(255, 255, 255);
        colors[ImGuiCustomCol_SliderThumbSmall] = ImColor(0, 95, 184);
        colors[ImGuiCustomCol_SliderThumbLarge] = ImColor(69, 69, 69, 154);

        //Checkbox
        colors[ImGuiCustomCol_CheckboxBorderDisabled] = ImColor(0.4f, 0.4f, 0.4f);
        colors[ImGuiCustomCol_CheckboxBorderEnabled] = ImColor(0, 95, 184);
        colors[ImGuiCustomCol_CheckboxThumbDisabled] = ImColor(0.4f, 0.4f, 0.4f);
        colors[ImGuiCustomCol_CheckboxThumbEnabled] = ImColor(255, 255, 255);
    }
}

CustomSettings::CustomSettings()
{
    night_mode = true;
    animation_scale = 0.5f;
    easing_func = GetEasingFunction(easing_functions::EaseOutExpo);

    colors = new ImVec4[ImGuiCustomCol_COUNT];

    UpdateColors();
}

CustomSettings::~CustomSettings()
{
    delete[] colors;
}

namespace detail
{
    std::array<CustomWidget, 256> widgets;
    size_t widget_count = 0;

    void _SetupTransition(const bool& v, ImGuiAnimState_* anim_state, float pixel_per_sec, float pixel_move_dist)
    {
        if (v)
        {
            if (pixel_per_sec > 0.1f) *anim_state = ImGuiAnimState_::TRANSITION_RIGHT;
            if (int(pixel_per_sec * 10.f) == int(pixel_move_dist * 10.f)) *anim_state = ImGuiAnimState_::RIGHT_STOP;
        }
        else
        {
            if (pixel_per_sec > 0.1f) *anim_state = ImGuiAnimState_::TRANSITION_LEFT;
            if (int(pixel_per_sec * 10.f) == int(pixel_move_dist * 10.f)) *anim_state = ImGuiAnimState_::LEFT_STOP;
        }
    }

    float _CalcPixelTransitionLinear(float pixel_dist, float anim_start_time, float anim_time = settings.animation_scale)
    {
        float dist = pixel_dist; if (dist < 0.f) dist = 0.f;
        const float elapsed_time = std::clamp(anim_start_time, 0.0f, anim_time);
        float pixels_per_sec = (elapsed_time * dist) / anim_time;
        return std::clamp(pixels_per_sec, 0.0f, dist);
    }

    float _CalcPixelTransition(float pixel_dist, float anim_start_time, float epsilon = 0.1f, float func_bound = 8.f, float anim_time = settings.animation_scale)
    {
        float dist = pixel_dist; if (dist < 0.f) dist = 0.f;
        const float elapsed_time = std::clamp(anim_start_time, 0.0f, anim_time);
        float pixels_per_sec = (elapsed_time * dist) / anim_time;
        return std::clamp(float(settings.easing_func(pixels_per_sec / (dist - func_bound))) * (dist + epsilon), 0.0f, dist);
    }

    ImVec4 _CalcColorTransition(const ImColor& from, const ImColor& to, float anim_start_time, float anim_time = settings.animation_scale)
    {
        ImVec4 color_per_sec{ 0.0f, 0.0f, 0.0f, 0.0f };
        for (size_t i = 0; i < 4; i++)
        {
            if (to.Value[i] != from.Value[i])
            {
                auto dist = to.Value[i] - from.Value[i];
                color_per_sec[i] = _CalcPixelTransition(std::abs(dist), anim_start_time, 0.01f, 0.0f, anim_time);
                if (dist < 0.f) color_per_sec[i] *= -1.f;
                color_per_sec[i] = std::clamp(color_per_sec[i], -1.0f, 1.0f);
            }
        }
        return color_per_sec;
    }

    void _TransitColor(const ImGuiAnimState_& anim_state, float anim_time, const bool& v, const ImVec4& active_col, const ImVec4& deactive_col, ImVec4* anim_col_out)
    {
        if (v) *anim_col_out = deactive_col;
        else *anim_col_out = active_col;

        auto trans_color = _CalcColorTransition(deactive_col, active_col, anim_time);

        switch (anim_state)
        {
        case ImGuiAnimState_::TRANSITION_RIGHT:
        {
            *anim_col_out += trans_color;
            break;
        }

        case ImGuiAnimState_::TRANSITION_LEFT:
        {
            *anim_col_out -= trans_color;
            break;
        }

        case ImGuiAnimState_::RIGHT_STOP:
        {
            *anim_col_out = active_col;
            break;
        }

        case ImGuiAnimState_::LEFT_STOP:
        {
            *anim_col_out = deactive_col;
            break;
        }
        }
    }

    void _TransitColor(const bool& v, float anim_time, const ImVec4& active_col, const ImVec4& deactive_col, ImVec4* anim_col_out)
    {
        auto trans_color = _CalcColorTransition(deactive_col, active_col, anim_time);

        if (v) *anim_col_out += trans_color;
        else *anim_col_out -= trans_color;

        if (v && anim_time >= settings.animation_scale)
            *anim_col_out = deactive_col;
        else if (!v && anim_time >= settings.animation_scale)
            *anim_col_out = active_col;

        anim_col_out->Clamp(0.0f, 1.0f);
    }

    void _DebugRect(const ImVec2& min, const ImVec2& max, const ImColor& color, bool filled = false)
    {
        if (filled) ImGui::GetForegroundDrawList()->AddRectFilled(min, max, color);
        else ImGui::GetForegroundDrawList()->AddRect(min, max, color);
    }

    template <typename ...Args> void _DebugText(const ImVec2& pos, const char* format, Args... arg)
    {
        static char buff[512];
        sprintf_s(buff, format, arg...);
        ImGui::GetForegroundDrawList()->AddText(pos, ImColor(255, 255, 255), buff);
    }

    template <typename ...Args> void _DebugText(const ImVec2& pos, const ImColor& color, const char* format, Args... arg)
    {
        static char buff[512];
        sprintf_s(buff, format, arg...);
        ImGui::GetForegroundDrawList()->AddText(pos, color, buff);
    }

    void _DebugInfo(const CustomWidget& wdg)
    {
        _DebugText({ 200, 280 }, "transition: %s", str[wdg.anim_state].c_str());
        _DebugText({ 200, 300 }, "anim_trigger = %s", (wdg.animation_trigger ? "true" : "false"));
        _DebugText({ 200, 320 }, "color button = %s", (wdg.color_button_state ? "true" : "false"));
        _DebugText({ 200, 340 }, "time since update = %.2f", wdg.timer.GetTimeSinceUpdate());
    }

    template<typename ...TArgs>
    void _DebugInfo(const ImVec2& pos, const char* fmt, TArgs... args)
    {
        char buf[512];
        sprintf_s(buf, fmt, args);
        ImGui::GetForegroundDrawList()->AddText(pos, ImColor(255, 255, 255), buf);
    }

    //returns pressed
    bool _DrawCheckbox(ImGuiWindow* window, const ImRect& bb, const char* label, const ImVec2& label_size, bool* v, CustomWidget& cur_widget)
    {
        auto id = ImGui::GetItemID();

        bool hovered, held, hovering_whole_frame = false;
        bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

        auto& style = ImGui::GetStyle();
        auto& cfg = *ImGui::GetCustomSettings();

        if (pressed)
        {
            *v = !(*v);
            cur_widget.timer.Update();
            ImGui::MarkItemEdited(id);
        }

        
        const ImRect check_bb(bb.Min + ImVec2(0.f, 2.f), bb.Max - ImVec2(0.f, 2.f));
        ImGui::RenderNavHighlight(bb, id);

        ImVec4 text_col = cfg.colors[ImGuiCustomCol_Text]; text_col.w = ImGui::GetStyle().Alpha;
        
		ImVec4 check_enabled_col = cfg.colors[ImGuiCustomCol_CheckboxThumbEnabled]; check_enabled_col.w = ImGui::GetStyle().Alpha;
		ImVec4 check_disabled_col = cfg.colors[ImGuiCustomCol_CheckboxThumbDisabled]; check_disabled_col.w = ImGui::GetStyle().Alpha;
		ImVec4 border_disabled_col = cfg.colors[ImGuiCustomCol_CheckboxBorderDisabled]; border_disabled_col.w = ImGui::GetStyle().Alpha;
		ImVec4 border_enabled_col = cfg.colors[ImGuiCustomCol_CheckboxBorderEnabled]; border_enabled_col.w = ImGui::GetStyle().Alpha;
		ImVec4 frame_bg_disabled_col = cfg.colors[ImGuiCustomCol_FrameDisabled]; frame_bg_disabled_col.w = ImGui::GetStyle().Alpha;
		ImVec4 frame_bg_enabled_col = cfg.colors[ImGuiCustomCol_FrameEnabled]; frame_bg_enabled_col.w = ImGui::GetStyle().Alpha;

        ImVec4 switch_animated_col, text_animated_col, frame_bg_animated_col, border_animated_col;

        const auto frame_h_2 = check_bb.GetSize().y / 2.f;
        const auto r = frame_h_2 - 2.f;

        auto pixel_dist = (check_bb.Max.x - r - 2.f) - (check_bb.Min.x + r + 2.f);
        float pixels_per_sec = _CalcPixelTransition(pixel_dist, cur_widget.timer.GetTimeSinceUpdate());

        ImVec2 check_pos = { (!(*v) ? check_bb.Max.x - pixels_per_sec - r - 2.f : check_bb.Min.x + pixels_per_sec + r + 2.f), check_bb.Min.y + frame_h_2 };
        ImVec2 label_pos = ImVec2(window->DC.CursorPos.x + ImGui::GetStyle().FramePadding.x, bb.Min.y + ImGui::GetStyle().FramePadding.y - 1.f);
        ImRect frame_bb =
        { label_pos.x - style.ItemInnerSpacing.x, label_pos.y - style.ItemInnerSpacing.y ,
            ImGui::GetItemRectMax().x + style.ItemInnerSpacing.x, ImGui::GetItemRectMax().y };

        _SetupTransition(*v, &cur_widget.anim_state, pixels_per_sec, pixel_dist);

        const auto& updt_time = cur_widget.timer.GetTimeSinceUpdate();
        _TransitColor(cur_widget.anim_state, updt_time, *v, check_enabled_col, check_disabled_col, &switch_animated_col);
        _TransitColor(cur_widget.anim_state, updt_time, *v, text_col, check_disabled_col, &text_animated_col);
        _TransitColor(cur_widget.anim_state, updt_time, *v, frame_bg_enabled_col, frame_bg_disabled_col, &frame_bg_animated_col);
        _TransitColor(cur_widget.anim_state, updt_time, *v, border_enabled_col, border_disabled_col, &border_animated_col);

        //Highlight
        if (ImGui::IsMouseHoveringRect(frame_bb.Min + 1.f, check_bb.Max + 3.f))
            window->DrawList->AddRectFilled(frame_bb.Min + 1.f, check_bb.Max + 3.f, ImColor(1.f, 1.f, 1.f, ImGui::GetStyle().Alpha * 0.1f), 24.f);

        //frame border
        window->DrawList->AddRectFilled(check_bb.Min - 1.f, check_bb.Max + 1.f, ImColor(border_animated_col), 24.f);

        //frame bg
        window->DrawList->AddRectFilled(check_bb.Min, check_bb.Max, ImColor(frame_bg_animated_col), 24.f);

        //thumb
        window->DrawList->AddCircleFilled(check_pos, r, ImColor(switch_animated_col));
        window->DrawList->AddCircle(check_pos, r, ImColor(0.f, 0.f, 0.f, ImGui::GetStyle().Alpha));

        //thumb
        window->DrawList->AddCircleFilled(check_pos, r, ImColor(switch_animated_col));

        //label
        if (label_size.x > 0.0f) window->DrawList->AddText(label_pos, ImColor(text_animated_col), label);

        return pressed;
    }
}

void ImGui::CustomAnimationTriggerSet(bool menu_visible)
{
    static bool was_visible = false;

    if (menu_visible && !was_visible)
    {
        was_visible = true;
        for (auto& w : detail::widgets)
        {
            //TODO ?
        }
    }
    else if (!menu_visible && was_visible)
    {
        was_visible = false;
    }
}

ImU32 ImGui::GetCustomColorU32(int idx, float alpha_mul)
{
    auto* settings = ImGui::GetCustomSettings();
    auto color = settings->colors[idx];
    color.w = ImGui::GetStyle().Alpha * alpha_mul;
    return ColorConvertFloat4ToU32(color);
}
    
bool ImGui::CustomBegin(const char* label, int* option, const std::vector<void*>& tab_icons, ImGuiWindowFlag_ flags, void* custom_bg, void* custom_logo)
{
    if (custom_logo)
        flags |= ImGuiWindowFlags_NoTitleBar;

    auto result = ImGui::Begin(label, nullptr, flags);
    ImGuiWindow* window = FindWindowByName(label);
    if (!window) return result;

    const auto& alpha = ImGui::GetStyle().Alpha;


    if (custom_bg)
    {
        window->DrawList->AddImage(
            custom_bg, ImGui::GetWindowPos() + ImVec2(0, 100),
            ImGui::GetWindowPos() + ImGui::GetWindowSize() + ImVec2(10.f, 10.f),
            ImVec2(0, 0), ImVec2(1, 1), ImColor(0.7f, 0.7f, 0.7f, alpha));
    }

    window->DrawList->AddRectFilled(
        window->Pos, window->Pos + ImVec2(window->SizeFull.x, 100.f),
        ImColor(0, 0, 0, 255));

    window->DrawList->AddRectFilledMultiColor(
        window->Pos, window->Pos + ImVec2(window->SizeFull.x, 100.f),
        ImColor(0.3f, 0.3f, 0.3f, alpha), ImColor(0.3f, 0.3f, 0.3f, alpha),
        ImColor(0.1f, 0.1f, 0.1f, alpha * 0.5f), ImColor(0.1f, 0.1f, 0.1f, alpha * 0.5f));

    window->DrawList->AddRectFilledMultiColor(
        { window->Pos.x, window->Pos.y + 100.f - 3.f }, { window->Pos.x + window->SizeFull.x / 2.f, window->Pos.y + 100.f },
        Color::ColdBlue(0), Color::ColdBlue(alpha * 255),
        Color::ColdBlue(alpha * 255), Color::ColdBlue(0));

    window->DrawList->AddRectFilledMultiColor(
        { window->Pos.x + window->SizeFull.x / 2.f, window->Pos.y + 100.f - 3.f }, { window->Pos.x + window->SizeFull.x, window->Pos.y + 100.f },
        Color::ColdBlue(alpha * 255), Color::ColdBlue(0),
        Color::ColdBlue(0), Color::ColdBlue(alpha * 255));

    if (custom_logo)
    {
        ImGui::Dummy(ImVec2(220, 100));
        window->DrawList->AddImage(
            custom_logo, ImGui::GetWindowPos() + ImVec2(10.f, 10.f),
            ImGui::GetWindowPos() + ImVec2(10.f, 10.f) + ImVec2(220.f, 80.f),
            ImVec2(0, 0), ImVec2(1, 1), ImColor(1.f, 1.f, 1.f, ImGui::GetStyle().Alpha));
    }


    ImVec2 y_backup = ImGui::GetCursorPos();
    int i = 0;

    ImGui::SetCursorPos(ImVec2(ImGui::GetStyle().WindowPadding.x, 100.f));
    ImGui::BeginChild("##tabbar", {0.f, 0.f}, false);

    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, 0);

    for (auto* tab : tab_icons)
    {
        char id[0xF]{};
        sprintf_s(id, "%d", i);

		ImGui::PushStyleColor(ImGuiCol_Button, i == *option ? 
            Color::FireRed(ImGui::GetStyle().Alpha * 255).GetU32() : 
            Color::White(ImGui::GetStyle().Alpha * 255 * 0.2f).GetU32());

        ImGui::SetCursorPosX(50.f - 26.f - ImGui::GetStyle().ItemSpacing.x);
        if (ImGui::ImageButton(id, tab, ImVec2(52, 52), ImVec2(0,0), ImVec2(1,1), ImVec4(0,0,0,0), ImVec4(0,0,0,ImGui::GetStyle().Alpha)))
            *option = i;

        i++;
    }
    ImGui::PopStyleColor(2 + tab_icons.size());
    ImGui::EndChild();
	ImGui::SetCursorPos(y_backup);
    return result;
}

bool ImGui::CustomBeginChild(const char* label, bool border)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* parent_window = g.CurrentWindow;
    auto backup = g.Style.ChildRounding;
    g.Style.ChildRounding = 6.f;

    ImGuiWindowFlags flags = 0;
    ImVec2 size_arg = { 0.f, 0.f };

    flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;
    flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);  // Inherit the NoMove flag

    // Size
    const ImVec2 content_avail = GetContentRegionAvail();
    ImVec2 size = ImFloor(size_arg);
    const int auto_fit_axises = ((size.x == 0.0f) ? (1 << ImGuiAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << ImGuiAxis_Y) : 0x00);
    if (size.x <= 0.0f)
        size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too much issues)
    if (size.y <= 0.0f)
        size.y = ImMax(content_avail.y + size.y, 4.0f);
    SetNextWindowSize(size);

    auto id = parent_window->GetID(label);

    // Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(ImGuiID id) with a stable value.
    const char* temp_window_name;
    if (label)
        ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%s_%08X", parent_window->Name, label, id);
    else
        ImFormatStringToTempBuffer(&temp_window_name, NULL, "%s/%08X", parent_window->Name, id);

    PushStyleColor(ImGuiCol_ChildBg, GetCustomColorU32(ImGuiCustomCol_ChildBg));

    const float backup_border_size = g.Style.ChildBorderSize;
    if (!border)
        g.Style.ChildBorderSize = 0.0f;
    bool ret = Begin(temp_window_name, NULL, flags);
    g.Style.ChildBorderSize = backup_border_size;

    ImGuiWindow* child_window = g.CurrentWindow;
    child_window->ChildId = id;
    child_window->AutoFitChildAxises = (ImS8)auto_fit_axises;

    const ImVec2 text_pos = child_window->Pos + g.Style.ItemSpacing;

    const auto label_size = CalcTextSize(label);
    const auto frame_bb =
        ImRect(
            child_window->Pos,
            ImVec2(child_window->Pos.x + child_window->InnerRect.GetSize().x,
                text_pos.y + label_size.y + g.Style.ItemSpacing.y + 2 + g.Style.ItemSpacing.x));

    ItemSize(frame_bb);
    if (!ItemAdd(frame_bb, id))
    {
        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
        return false;
    }

    ImGui::GetForegroundDrawList()->AddRectFilled(frame_bb.Min, frame_bb.Max, GetCustomColorU32(ImGuiCustomCol_ChildBg));
    ImGui::GetForegroundDrawList()->AddText(text_pos, GetCustomColorU32(ImGuiCustomCol_Text), label);
    ImGui::GetForegroundDrawList()->AddRectFilled(
        { text_pos.x, text_pos.y + label_size.y + g.Style.ItemSpacing.y },
        { child_window->Pos.x + child_window->InnerRect.GetSize().x - g.Style.ItemSpacing.x, text_pos.y + label_size.y + g.Style.ItemSpacing.y + 2 },
        GetCustomColorU32(ImGuiCustomCol_ChildBg), 100.f);

    // Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
    // While this is not really documented/defined, it seems that the expected thing to do.
    if (child_window->BeginCount == 1)
        parent_window->DC.CursorPos = child_window->Pos;


    // Process navigation-in immediately so NavInit can run on first frame
    if (g.NavActivateId == id && !(flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayersActiveMask != 0 || child_window->DC.NavHasScroll))
    {
        FocusWindow(child_window);
        NavInitWindow(child_window, false);
        SetActiveID(id + 1, child_window); // Steal ActiveId with another arbitrary id so that key-press won't activate child item
        g.ActiveIdSource = ImGuiInputSource_Nav;
    }

    g.Style.ChildRounding = backup;
    PopStyleColor();

    return ret;
}

bool ImGui::CustomButton(const char* label)
{
    return Button(label);
}

bool ImGui::CustomCheckbox(const char* label, bool* v)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = CalcTextSize(label, NULL, true);

    const float square_sz = GetFrameHeight();
    const ImVec2 pos = window->DC.CursorPos;
    const ImVec2 bb_size(square_sz * 2.5f / 2.f + style.ItemInnerSpacing.x * 2.f, label_size.y + style.FramePadding.y * 2.0f - 1.f);
    ImVec2 min = { pos.x + window->InnerRect.GetSize().x - (style.FramePadding.x * 1.f) * 4.0f - bb_size.x, pos.y };
    if (auto clamp = pos.x + style.ItemInnerSpacing.x * 2.f + label_size.x + g.Style.FramePadding.x * 8.f; min.x < clamp)
        min.x = clamp;
    const ImRect total_bb(min, min + bb_size);

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id))
    {
        IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
        return false;
    }

    detail::widget_count++;

    if (detail::widgets.empty() || detail::widget_count - 1 >= detail::widgets.size())
        return false;

    auto& cur_widget = detail::widgets[detail::widget_count - 1];
    cur_widget.id = detail::widget_count - 1;

    auto pressed = detail::_DrawCheckbox(window, total_bb, label, label_size, v, cur_widget);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
    return pressed;
}

bool ImGui::CustomCheckbox(const char* label, bool* v, Color* color)
{
    bool checkbox_clicked = CustomCheckbox(label, v);
    bool color_picker_clicked = CustomColorPicker(label, color, true);
    return (checkbox_clicked || color_picker_clicked);
}

bool ImGui::CustomColorPicker(const char* label, Color* v, bool merge_to_checkbox)
{
    if (!merge_to_checkbox)
    {
        return false;
    }

    detail::widget_count++;

    if (detail::widgets.empty() || detail::widget_count - 1 >= detail::widgets.size())
        return false;

    CustomWidget& cur_widget = detail::widgets[detail::widget_count - 1];
    cur_widget.id = detail::widget_count - 1;

    cur_widget.picker_color = ImGui::ColorConvertU32ToFloat4(*v);

    ImVec2 pos = ImGui::GetCursorPos();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
    ImVec2 button_pos = pos + ImVec2(ImGui::GetWindowSize().x - 70.f - ImGui::GetStyle().ItemInnerSpacing.x, -23.f);
    ImGui::SetCursorPos(button_pos);
    std::string button_label = "##";
    bool clicked = ImGui::ColorButton(button_label.append(std::to_string(cur_widget.id)).append(label).c_str(), cur_widget.picker_color, ImGuiColorEditFlags_NoTooltip);
    if (clicked) cur_widget.color_button_state = true;
    ImGui::PopStyleVar();

    if (cur_widget.color_button_state)
    {
        ImGui::SetNextWindowPos(ImGui::GetWindowPos() + button_pos + ImGui::GetItemRectSize() / 2.f);
        ImGui::Begin(std::to_string(cur_widget.id).c_str(), nullptr, 
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar 
            | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar 
            | ImGuiWindowFlags_NoCollapse);
        
        ImGui::ColorPicker4(button_label.append("_picker").c_str(), &cur_widget.picker_color.x,
            ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);

        bool is_focused = ImGui::IsWindowFocused();
        ImGui::End();

        if (!is_focused)
            cur_widget.color_button_state = false;
	}

    ImGui::SetCursorPos(pos);

    *v = Color(ImGui::ColorConvertFloat4ToU32(cur_widget.picker_color));
    return clicked;
}

bool ImGui::CustomSliderFloat(const char* label, float* v, float v_min, float v_max, const char* format)
{
    float* p_data = v;
    ImGuiSliderFlags flags = 0;
    const auto data_type = ImGuiDataType_Float;
    float* p_min = &v_min;
    float* p_max = &v_max;

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImVec2 frame_min(window->DC.CursorPos.x + window->InnerRect.GetSize().x - window->InnerRect.GetSize().x / 2.f, window->DC.CursorPos.y + style.FramePadding.y);
    const ImVec2 frame_max(window->DC.CursorPos.x + window->InnerRect.GetSize().x - style.FramePadding.x * 4.0f, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2.0f - 1.f);
    const ImRect frame_bb(frame_min, frame_max);
    const ImRect total_bb(window->DC.CursorPos, frame_max);

    const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
        return false;

    detail::widget_count++;

    if (detail::widgets.empty() || detail::widget_count - 1 >= detail::widgets.size())
        return false;

    auto& cur_widget = detail::widgets[detail::widget_count - 1];
    cur_widget.id = detail::widget_count - 1;

    // Default format string when passing NULL
    if (format == NULL)
        format = DataTypeGetInfo(data_type)->PrintFmt;
    else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0)
        format = PatchFormatStringFloatToInt(format);

    const bool hovered = ItemHoverable(frame_bb, id);
    bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
    if (!temp_input_is_active)
    {
        // Tabbing or CTRL-clicking on Slider turns it into an input box
        const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
        const bool clicked = (hovered && g.IO.MouseClicked[0]);
        const bool make_active = (input_requested_by_tabbing || clicked || g.NavActivateId == id || g.NavActivateInputId == id);
        if (make_active && temp_input_allowed)
            if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || g.NavActivateInputId == id)
                temp_input_is_active = true;

        if (make_active && !temp_input_is_active)
        {
            SetActiveID(id, window);
            SetFocusID(id, window);
            FocusWindow(window);
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
        }
    }

    if (temp_input_is_active)
    {
        // Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
        const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
        return TempInputScalar(frame_bb, id, label, data_type, v, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
    }

    // Draw frame
    const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    RenderNavHighlight(frame_bb, id);

    // Slider behavior
    ImRect future_grab_bb;
    const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &future_grab_bb);
    if (value_changed)
    {
        MarkItemEdited(id);
    }
    else
        cur_widget.slider_thumb_pos = future_grab_bb.GetCenter().x;

    if (IsItemActive()) cur_widget.timer.Update();

    const ImVec2 rail_min = { frame_bb.Min.x + future_grab_bb.GetSize().x / 2.0f + 2.0f, frame_bb.GetCenter().y - 2.0f };
    const ImVec2 rail_max = { frame_bb.Max.x - future_grab_bb.GetSize().x / 2.0f - 2.0f, frame_bb.GetCenter().y + 2.0f };

    //Rail 
    window->DrawList->AddRectFilled(rail_min, rail_max, GetCustomColorU32(ImGuiCustomCol_FrameDisabled), 24.f);

    //Render thumb
    if (future_grab_bb.Max.x > future_grab_bb.Min.x)
    {
        const float grab_r = ((label_size.y + style.FramePadding.y * 2.0f - 1.f) / 2.f) - 2.0f;

        auto active_color = (ImColor)GetCustomColorU32(ImGuiCustomCol_FrameEnabled);
        auto deactive_color = active_color;
        deactive_color.Value.w = 0.0f;

        //Glow top
        window->DrawList->AddRectFilledMultiColor(
            { rail_min.x, rail_min.y - 3.f },
            { future_grab_bb.GetCenter().x, frame_bb.GetCenter().y + 2.f },
            deactive_color, deactive_color,
            active_color, active_color);

        //Glow bottom
        window->DrawList->AddRectFilledMultiColor(
            { rail_min.x, rail_min.y },
            { future_grab_bb.GetCenter().x, frame_bb.GetCenter().y + 5.f },
            active_color, active_color,
            deactive_color, deactive_color);

        //Rail
        window->DrawList->AddRectFilled(
            rail_min,
            { future_grab_bb.GetCenter().x, frame_bb.GetCenter().y + 2.f },
            GetCustomColorU32(ImGuiCustomCol_FrameEnabled), 24.f);

        auto grab_center = future_grab_bb.GetCenter();

        //Grab circle
        window->DrawList->AddCircleFilled(grab_center, grab_r + 1.f, GetCustomColorU32(ImGuiCustomCol_SliderThumbLarge, 0.5f));
        window->DrawList->AddCircleFilled(grab_center, grab_r, GetCustomColorU32(ImGuiCustomCol_SliderThumbBig));
        window->DrawList->AddCircleFilled(grab_center, grab_r / 1.75f, GetCustomColorU32(ImGuiCustomCol_SliderThumbSmall));
    }

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
    if (g.LogEnabled)
        LogSetNextTextDecoration("{", "}");

    ImColor text_color = GetCustomColorU32(ImGuiCustomCol_Text);

    //Value
    window->DrawList->AddText(
        { rail_min.x - style.ItemInnerSpacing.x * 4.f - CalcTextSize(value_buf).x, frame_bb.Min.y },
        text_color,
        value_buf);

    //Label
    if (label_size.x > 0.0f)
        window->DrawList->AddText(
            { total_bb.Min.x + style.ItemInnerSpacing.x, frame_bb.Min.y },
            text_color,
            label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return value_changed;
}

bool ImGui::CustomSliderInt(const char* label, int* v, int v_min, int v_max, const char* format)
{
    int* p_data = v;
    ImGuiSliderFlags flags = 0;
    const auto data_type = ImGuiDataType_S32;
    int* p_min = &v_min;
    int* p_max = &v_max;

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImVec2 frame_min(window->DC.CursorPos.x + window->InnerRect.GetSize().x - window->InnerRect.GetSize().x / 2.f, window->DC.CursorPos.y + style.FramePadding.y);
    const ImVec2 frame_max(window->DC.CursorPos.x + window->InnerRect.GetSize().x - style.FramePadding.x * 4.0f, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2.0f - 1.f);
    const ImRect frame_bb(frame_min, frame_max);
    const ImRect total_bb(window->DC.CursorPos, frame_max);

    const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
        return false;

    detail::widget_count++;

	if (detail::widgets.empty() || detail::widget_count - 1 >= detail::widgets.size())
        return false;

    auto& cur_widget = detail::widgets[detail::widget_count - 1];
    cur_widget.id = detail::widget_count - 1;

    // Default format string when passing NULL
    if (format == NULL)
        format = DataTypeGetInfo(data_type)->PrintFmt;
    else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0)
        format = PatchFormatStringFloatToInt(format);

    const bool hovered = ItemHoverable(frame_bb, id);
    bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
    if (!temp_input_is_active)
    {
        // Tabbing or CTRL-clicking on Slider turns it into an input box
        const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
        const bool clicked = (hovered && g.IO.MouseClicked[0]);
        const bool make_active = (input_requested_by_tabbing || clicked || g.NavActivateId == id || g.NavActivateInputId == id);
        if (make_active && temp_input_allowed)
            if (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || g.NavActivateInputId == id)
                temp_input_is_active = true;

        if (make_active && !temp_input_is_active)
        {
            SetActiveID(id, window);
            SetFocusID(id, window);
            FocusWindow(window);
            g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
        }
    }

    if (temp_input_is_active)
    {
        // Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
        const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
        return TempInputScalar(frame_bb, id, label, data_type, v, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
    }

    // Draw frame
    const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    RenderNavHighlight(frame_bb, id);

    // Slider behavior
    ImRect future_grab_bb;
    const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &future_grab_bb);
    if (value_changed)
    {
        MarkItemEdited(id);
    }
    else
        cur_widget.slider_thumb_pos = future_grab_bb.GetCenter().x;

    if (IsItemActive()) cur_widget.timer.Update();

    const ImVec2 rail_min = { frame_bb.Min.x + future_grab_bb.GetSize().x / 2.0f + 2.0f, frame_bb.GetCenter().y - 2.0f };
    const ImVec2 rail_max = { frame_bb.Max.x - future_grab_bb.GetSize().x / 2.0f - 2.0f, frame_bb.GetCenter().y + 2.0f };

    //Rail 
    window->DrawList->AddRectFilled(rail_min, rail_max, GetCustomColorU32(ImGuiCustomCol_FrameDisabled), 24.f);

    //Render thumb
    if (future_grab_bb.Max.x > future_grab_bb.Min.x)
    {
        const float grab_r = ((label_size.y + style.FramePadding.y * 2.0f - 1.f) / 2.f) - 2.0f;

        auto active_color = (ImColor)GetCustomColorU32(ImGuiCustomCol_FrameEnabled);
        auto deactive_color = active_color;
        deactive_color.Value.w = 0.0f;

        //Glow top
        window->DrawList->AddRectFilledMultiColor(
            { rail_min.x, rail_min.y - 3.f },
            { future_grab_bb.GetCenter().x, frame_bb.GetCenter().y + 2.f },
            deactive_color, deactive_color,
            active_color, active_color);

        //Glow bottom
        window->DrawList->AddRectFilledMultiColor(
            { rail_min.x, rail_min.y },
            { future_grab_bb.GetCenter().x, frame_bb.GetCenter().y + 5.f },
            active_color, active_color,
            deactive_color, deactive_color);


        window->DrawList->AddRectFilled(
            rail_min,
            { future_grab_bb.GetCenter().x, frame_bb.GetCenter().y + 2.f },
            GetCustomColorU32(ImGuiCustomCol_FrameEnabled), 24.f);

        auto grab_center = future_grab_bb.GetCenter();

        //Grab circle
        window->DrawList->AddCircleFilled(grab_center, grab_r + 1.f, GetCustomColorU32(ImGuiCustomCol_SliderThumbLarge, 0.5f));
        window->DrawList->AddCircleFilled(grab_center, grab_r, GetCustomColorU32(ImGuiCustomCol_SliderThumbBig));
        window->DrawList->AddCircleFilled(grab_center, grab_r / 1.75f, GetCustomColorU32(ImGuiCustomCol_SliderThumbSmall));
    }

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    char value_buf[64];
    const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
    if (g.LogEnabled)
        LogSetNextTextDecoration("{", "}");

    ImColor text_color = GetCustomColorU32(ImGuiCustomCol_Text);

    //Value
    window->DrawList->AddText(
        { rail_min.x - style.ItemInnerSpacing.x * 4.f - CalcTextSize(value_buf).x, frame_bb.Min.y },
        text_color,
        value_buf);

    //Label
    if (label_size.x > 0.0f)
        window->DrawList->AddText(
            { total_bb.Min.x + style.ItemInnerSpacing.x, frame_bb.Min.y },
            text_color,
            label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return value_changed;
}

void ImGui::CustomEnd()
{
    /*int i = 0;
    for (const auto& it : widgets)
    {
        static char buff[0xFF]; sprintf_s(buff, "[%d] %f", i, it.GetElapsedTime());
        ImGui::GetForegroundDrawList()->AddText(ImVec2(20, 20 + 10 * i), ImColor(255, 255, 255), buff);
        i++;
    }*/

    //if (detail::widgets.empty())
    //    detail::widgets.resize(detail::widget_count);
    detail::widget_count = 0;

    ImGui::End();
}

void ImGui::CustomEndChild()
{
    ImGui::EndChild();
}

const char* const KeyNames[] = {
    "Unknown",
    "VK_LBUTTON",
    "VK_RBUTTON",
    "VK_CANCEL",
    "VK_MBUTTON",
    "VK_XBUTTON1",
    "VK_XBUTTON2",
    "Unknown",
    "VK_BACK",
    "VK_TAB",
    "Unknown",
    "Unknown",
    "VK_CLEAR",
    "VK_RETURN",
    "Unknown",
    "Unknown",
    "VK_SHIFT",
    "VK_CONTROL",
    "VK_MENU",
    "VK_PAUSE",
    "VK_CAPITAL",
    "VK_KANA",
    "Unknown",
    "VK_JUNJA",
    "VK_FINAL",
    "VK_KANJI",
    "Unknown",
    "VK_ESCAPE",
    "VK_CONVERT",
    "VK_NONCONVERT",
    "VK_ACCEPT",
    "VK_MODECHANGE",
    "VK_SPACE",
    "VK_PRIOR",
    "VK_NEXT",
    "VK_END",
    "VK_HOME",
    "VK_LEFT",
    "VK_UP",
    "VK_RIGHT",
    "VK_DOWN",
    "VK_SELECT",
    "VK_PRINT",
    "VK_EXECUTE",
    "VK_SNAPSHOT",
    "VK_INSERT",
    "VK_DELETE",
    "VK_HELP",
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
    "8",
    "9",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H",
    "I",
    "J",
    "K",
    "L",
    "M",
    "N",
    "O",
    "P",
    "Q",
    "R",
    "S",
    "T",
    "U",
    "V",
    "W",
    "X",
    "Y",
    "Z",
    "VK_LWIN",
    "VK_RWIN",
    "VK_APPS",
    "Unknown",
    "VK_SLEEP",
    "VK_NUMPAD0",
    "VK_NUMPAD1",
    "VK_NUMPAD2",
    "VK_NUMPAD3",
    "VK_NUMPAD4",
    "VK_NUMPAD5",
    "VK_NUMPAD6",
    "VK_NUMPAD7",
    "VK_NUMPAD8",
    "VK_NUMPAD9",
    "VK_MULTIPLY",
    "VK_ADD",
    "VK_SEPARATOR",
    "VK_SUBTRACT",
    "VK_DECIMAL",
    "VK_DIVIDE",
    "VK_F1",
    "VK_F2",
    "VK_F3",
    "VK_F4",
    "VK_F5",
    "VK_F6",
    "VK_F7",
    "VK_F8",
    "VK_F9",
    "VK_F10",
    "VK_F11",
    "VK_F12",
    "VK_F13",
    "VK_F14",
    "VK_F15",
    "VK_F16",
    "VK_F17",
    "VK_F18",
    "VK_F19",
    "VK_F20",
    "VK_F21",
    "VK_F22",
    "VK_F23",
    "VK_F24",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "VK_NUMLOCK",
    "VK_SCROLL",
    "VK_OEM_NEC_EQUAL",
    "VK_OEM_FJ_MASSHOU",
    "VK_OEM_FJ_TOUROKU",
    "VK_OEM_FJ_LOYA",
    "VK_OEM_FJ_ROYA",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "Unknown",
    "VK_LSHIFT",
    "VK_RSHIFT",
    "VK_LCONTROL",
    "VK_RCONTROL",
    "VK_LMENU",
    "VK_RMENU"
};

bool ImGui::CustomHotkey(const char* label, int* key)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    ImGuiIO& io = g.IO;
    const ImGuiStyle& style = g.Style;
    
    const ImGuiID id = window->GetID(label);
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    ImVec2 size = ImGui::CalcItemSize(ImVec2(120.f, ImGui::GetFrameHeight()), ImGui::CalcItemWidth(), label_size.y + style.FramePadding.y * 2.0f);
    const ImRect frame_bb(
        ImVec2(window->DC.CursorPos.x + window->InnerRect.GetSize().x - size.x, window->DC.CursorPos.y), 
        ImVec2(window->DC.CursorPos.x + window->InnerRect.GetSize().x - style.FramePadding.y * 4.0f - style.ItemInnerSpacing.x + 2.f, window->DC.CursorPos.y + size.y));
    const ImRect total_bb(window->DC.CursorPos, frame_bb.Max);

    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id))
        return false;

    const bool hovered = ImGui::ItemHoverable(frame_bb, id);

    if (hovered) {
        ImGui::SetHoveredID(id);
        g.MouseCursor = ImGuiMouseCursor_TextInput;
    }

    const bool user_clicked = hovered && io.MouseClicked[0];

    if (user_clicked) {
        if (g.ActiveId != id) {
            // Start edition
            memset(io.MouseDown, 0, sizeof(io.MouseDown));
            memset(io.KeysDown, 0, sizeof(io.KeysDown));
            *key = 0;
        }
        ImGui::SetActiveID(id, window);
        ImGui::FocusWindow(window);
    }
    else if (io.MouseClicked[0]) {
        // Release focus when we click outside
        if (g.ActiveId == id)
            ImGui::ClearActiveID();
    }

    bool value_changed = false;
    int _key = *key;

    if (g.ActiveId == id) {
        for (auto i = 0; i < 5; i++) {
            if (io.MouseDown[i]) {
                switch (i) {
                case 0:
                    _key = VK_LBUTTON;
                    break;
                case 1:
                    _key = VK_RBUTTON;
                    break;
                case 2:
                    _key = VK_MBUTTON;
                    break;
                case 3:
                    _key = VK_XBUTTON1;
                    break;
                case 4:
                    _key = VK_XBUTTON2;
                    break;
                }
                value_changed = true;
                ImGui::ClearActiveID();
            }
        }
        if (!value_changed) {
            for (auto i = VK_BACK; i <= VK_RMENU; i++) {
                if (io.KeysDown[i]) {
                    _key = i;
                    value_changed = true;
                    ImGui::ClearActiveID();
                }
            }
        }

        if (IsKeyPressedMap(ImGuiKey_Escape)) {
            *key = 0;
            ImGui::ClearActiveID();
        }
        else {
            *key = _key;
        }
    }
     
    // Render
    // Select which buffer we are going to display. When ImGuiInputTextFlags_NoLiveEdit is Set 'buf' might still be the old value. We Set buf to NULL to prevent accidental usage from now on.

    char buf_display[64] = "None";

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 24.f);

    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), true, style.FrameRounding);

    if (*key != 0 && g.ActiveId != id) {

        strcpy_s(buf_display, (*key) < 0 ? "..." : KeyNames[*key]);
    }
    else if (g.ActiveId == id) {
        strcpy_s(buf_display, "<Press a key>");
    }

    const ImRect clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y); // Not using frame_bb.Max because we have adjusted size
    ImVec2 render_pos = frame_bb.Min + style.FramePadding;
    ImGui::RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, buf_display, NULL, NULL, style.ButtonTextAlign, &clip_rect);
    //RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, buf_display, NULL, NULL, GetColorU32(ImGuiCol_Text), style.ButtonTextAlign, &clip_rect);
    //draw_window->DrawList->AddText(g.Font, g.FontSize, render_pos, GetColorU32(ImGuiCol_Text), buf_display, NULL, 0.0f, &clip_rect);

    if (label_size.x > 0)
        ImGui::RenderText(ImVec2(total_bb.Min.x + 4.F, frame_bb.Min.y + style.FramePadding.y), label);

    ImGui::PopStyleVar();

    return value_changed;
}