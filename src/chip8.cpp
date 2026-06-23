#include "chip8.hpp"

#include <fstream>
#include <iostream>

#include "SDL3/SDL_oldnames.h"


chip8::chip8(const std::filesystem::path& path_to_rom, window_renderer* renderer)
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

    // Init texture that we will draw to
    surface = SDL_CreateSurface(64,32, SDL_PIXELFORMAT_ARGB8888);
    if (!surface) std::cerr << "Could not create surface: " << SDL_GetError() << std::endl;
    texture = SDL_CreateTextureFromSurface(renderer->get_renderer(), surface);
    if (!texture) std::cerr << "Could not create texture: " << SDL_GetError() << std::endl;
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    std::cout << "Initialized Chip8." << std::endl;
}

void chip8::update(float delta_time)
{
    uint8_t first_byte = memory[program_counter];
    uint8_t second_byte = memory[program_counter + 1];
    opcode = (first_byte << 8) | second_byte;

    uint8_t last_byte = (opcode & 0xF000) >> 12;

    program_counter += 2;

    switch (last_byte)
    {
    case 0x0:
        switch (opcode)
        {
        case 0x00E0:
            // Clear Screen
            OP_00E0();
            break;
        case 0x00EE:
            // Return
            OP_00EE();
            break;
        }
        break;
    case 0x1:
        // Jump
        OP_1NNN();
        break;
    case 0x2:
        // Call
        OP_2NNN();
        break;
    case 0x3:
        // Skip if registry value and a given value are equal
        OP_3XNN();
        break;
    case 0x4:
        // Skip if registry value and a given value are not equal
        OP_4XNN();
        break;
    case 0x5:
        // Skip if two values are equal
        OP_5XY0();
        break;
    case 0x6:
        // Set register
        OP_6XNN();
        break;
    case 0x7:
        // Add to register
        OP_7XNN();
        break;
    case 0x9:
        // Skip if two values are not equal
        OP_9XY0();
        break;
    case 0xA:
        // Set index register
        OP_ANNN();
        break;
    case 0xD:
        // Draw
        OP_DXYN();
        break;
    // Logical and Arithmetic instructions
    case 0x8:
        {
            uint8_t n = opcode & 0x000F;
            switch (n)
            {
            case 0x0:
                // Set

                break;
            }
        }
        break;
    default:
        std::cerr << "Unknown instruction was met." << std::endl;
        break;
    }
}

void chip8::render(window_renderer& renderer)
{
    SDL_Renderer* sdl_renderer = renderer.get_renderer();
    uint32_t pixels[64 * 32];

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

    SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(uint32_t));

    SDL_SetRenderDrawColor(sdl_renderer, 255,255,255,255);
    SDL_RenderTexture(sdl_renderer, texture, nullptr, nullptr);

    SDL_RenderPresent(sdl_renderer);
}

