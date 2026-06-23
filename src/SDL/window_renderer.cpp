#include "SDL/window_renderer.hpp"

#include <iostream>

window_renderer::window_renderer(const std::string& title, int width, int height, SDL_WindowFlags flags)
{
    if (!SDL_CreateWindowAndRenderer(title.c_str(), width, height, flags, &m_window, &m_renderer))
        std::cerr << "Could not create window and renderer: " << SDL_GetError() << std::endl;
}

window_renderer::~window_renderer()
{
    close();
}

window_renderer::window_renderer(window_renderer&& other) noexcept
{
    this->m_window = other.m_window;
    this->m_renderer = other.m_renderer;
    other.m_window = nullptr;
    other.m_renderer = nullptr;
}

window_renderer& window_renderer::operator=(window_renderer&& other) noexcept
{
    if (this == &other) return *this;

    this->m_window = other.m_window;
    this->m_renderer = other.m_renderer;
    other.m_window = nullptr;
    other.m_renderer = nullptr;

    return *this;
}

void window_renderer::close()
{
    if (m_renderer)
        SDL_DestroyRenderer(m_renderer);
    if (m_window)
        SDL_DestroyWindow(m_window);
}
