#pragma once

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

struct application_style
{
    ImFont* main_font = nullptr;
    ImFont* header_font = nullptr;
};

inline ImVec2 get_header_size(application_style& style, const std::string& text)
{
    ImGui::PushFont(style.header_font, 32.0f);
    ImVec2 size = ImGui::CalcTextSize(text.c_str());
    ImGui::PopFont();
    return size;
}

inline void draw_header(application_style& style, const std::string& text, float size = 32.0f)
{
    //ImGui::PopFont();
    ImGui::PushFont(style.header_font, size);
    ImGui::Text("%s", text.c_str());
    ImGui::PopFont();
    //ImGui::PushFont(style.main_font);
}

inline void setup_style()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.FrameRounding = 3.0f;
    style.FrameRounding = 2.3f;

    ImVec4* colors = style.Colors;

    colors[ImGuiCol_Text]                   = ImVec4(0.95f, 0.85f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.35f, 0.52f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.12f, 0.005f, 0.18f, 1.0f);
    colors[ImGuiCol_ChildBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.765f, 0.133f, 0.780f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.769f, 0.345f, 0.698f, 1.0f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.859f, 0.361f, 0.776f, 0.5f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.561f, 0.129f, 0.129f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.345f, 0.135f, 0.412f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.690f, 0.263f, 0.820f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.791f, 0.318f, 0.910f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.3f, 0.2f, 0.450f, 0.650f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.616f, 0.341f, 0.702f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.561f, 0.129f, 0.129f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.628f, 0.133f, 0.780f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.70f, 0.70f, 0.70f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.48f, 0.50f, 0.52f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.73f, 0.60f, 0.15f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.776f, 0.345f, 0.910f , 1.0f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.820f, 0.431f, 0.941f , 1.0f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.885f, 0.510f, 1.0f , 1.0f);
    colors[ImGuiCol_TabDimmed]              = ImVec4(0.561f, 0.259f, 0.651f , 1.0f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.561f, 0.259f, 0.651f , 1.0f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.561f, 0.259f, 0.651f , 1.0f);
}
