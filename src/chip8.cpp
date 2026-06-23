#include "chip8.hpp"

#include <fstream>
#include <iostream>

#include "SDL3/SDL_oldnames.h"


chip8::chip8(const std::filesystem::path& path_to_rom, window_renderer* renderer)
    : renderer(renderer)
{
    // std::ios::ate makes the cursor move to the end of the file
    std::ifstream file{path_to_rom, std::ios::binary | std::ios::ate};
    if (!file)
        std::cerr << "Could not open file for reading." << std::endl;

    // Get actual file size
    std::streamsize size = file.tellg();

    // Move the pointer back to the beginning
    file.seekg(0, std::ios::beg);

    // Write into memory starting from address 200
    file.read(reinterpret_cast<char*>(memory + STARTING_POINT), size);

    if (file.bad())
        std::cerr << "Failed to read the file." << std::endl;

    // Init surface
    surface = SDL_CreateSurface(64,32, SDL_PIXELFORMAT_ARGB8888);
    texture = SDL_CreateTextureFromSurface(renderer->get_renderer(), surface);

    std::cout << "Initialized Chip8." << std::endl;
}

void chip8::update(float delta_time)
{
    uint8_t first_byte = memory[program_counter];
    uint8_t second_byte = memory[program_counter + 1];
    uint16_t combined_byte = (first_byte << 8) | second_byte;

    uint8_t last_byte = (combined_byte & 0xF000) >> 12;
    uint16_t nnn = combined_byte & 0x0FFF;
    uint16_t nn = combined_byte & 0x00FF;
    uint16_t n = combined_byte & 0x000F;
    uint8_t x = (combined_byte & 0x0F00) >> 8;
    uint8_t y = (combined_byte & 0x00F0) >> 4;
    uint8_t xy = (combined_byte & 0x0FF0) >> 4;

    program_counter += 2;

    switch (last_byte)
    {
    case 0x0:
        switch (combined_byte)
        {
        case 0x00E0:
            // Clear Screen
            for (int i = 0; i < 64 * 32; ++i) screen[i] = 0;
            break;
        case 0x00EE:
            // Return
            program_counter = stack[--stack_pointer];
            break;
        }
        break;
    case 0x1:
        // Jump
        program_counter = nnn;
        break;
    case 0x2:
        // Call
        stack[stack_pointer++] = program_counter;
        program_counter = nnn;
        break;
    case 0x6:
        registry[x] = nn;
        break;
    case 0x7:
        registry[x] += nn;
        break;
    case 0xA:
        index_register = nnn;
        break;
    case 0xD:
        // Draw
        {
            // Wrap x and y position for a sprite
            uint8_t x_coord = registry[x] % 64;
            uint8_t y_coord = registry[y] % 32;

            registry[0xF] = 0;

            for (int row = 0; row < n; ++row)
            {
                uint8_t sprite_data = memory[index_register + row];

                for (int i = 0; i < 8; ++i)
                {
                    bool is_sprite_bit_set = ((sprite_data >> (7 - i)) & 1) == 1;

                    uint8_t target_x = x_coord + i;
                    uint8_t target_y = y_coord + row;

                    uint8_t& current_screen_coord = screen[target_x + target_y * 64];

                    if (is_sprite_bit_set && current_screen_coord)
                    {
                        current_screen_coord = 0;
                        registry[0xF] = 1;
                    }
                    else if (is_sprite_bit_set && !current_screen_coord)
                    {
                        current_screen_coord = 1;
                    }

                    if (x_coord > 64) break;
                }

                if (y_coord > 32) break;
            }
        }
        break;
    }
}

void chip8::render()
{
    SDL_Renderer* sdl_renderer = renderer->get_renderer();
    auto* pixels = static_cast<uint32_t*>(surface->pixels);

    SDL_SetRenderDrawColor(sdl_renderer, 0,0,0,255);
    SDL_RenderClear(sdl_renderer);

    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            uint32_t color = (screen[x + y * 64] == 1) ? 0xFFFFFFFF : 0xFF000000;
            pixels[x + y * 64] = color;
        }
    }

    texture = SDL_CreateTextureFromSurface(sdl_renderer, surface);
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    SDL_SetRenderDrawColor(sdl_renderer, 255,255,255,255);
    SDL_RenderTexture(sdl_renderer, texture, nullptr, nullptr);

    SDL_RenderPresent(sdl_renderer);

    SDL_DestroyTexture(texture);
}
