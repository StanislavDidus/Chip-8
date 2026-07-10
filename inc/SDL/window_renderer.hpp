#pragma once
#include <string>

#include "SDL3/SDL_render.h"

class window_renderer
{
public:
    window_renderer() = default;
    window_renderer(const std::string& title, int width, int height, SDL_WindowFlags flags);
    ~window_renderer();

    window_renderer(const window_renderer& other) = delete;
    window_renderer& operator=(const window_renderer& other) = delete;
    window_renderer(window_renderer&& other) noexcept;
    window_renderer& operator=(window_renderer&& other) noexcept;

    void toggle_fullscreen();

    void close();

    template <typename Self>
    auto&& get_window(this Self&& self)
    {
        return self.m_window;
    }

    template <typename Self>
    auto&& get_renderer(this Self&& self)
    {
        return self.m_renderer;
    }

private:
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;

    bool m_fullscreen = false;
};
