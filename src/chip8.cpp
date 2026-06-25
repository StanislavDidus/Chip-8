#include "chip8.hpp"

#include <fstream>
#include <iostream>
#include <cstring>

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

    init_keys();
    init_font();

    std::cout << "Initialized Chip8." << std::endl;
}

void chip8::init_keys()
{
    keymap[SDL_SCANCODE_1] = 0x1;
    keymap[SDL_SCANCODE_2] = 0x2;
    keymap[SDL_SCANCODE_3] = 0x3;
    keymap[SDL_SCANCODE_4] = 0xC;
    keymap[SDL_SCANCODE_Q] = 0x4;
    keymap[SDL_SCANCODE_W] = 0x5;
    keymap[SDL_SCANCODE_E] = 0x6;
    keymap[SDL_SCANCODE_R] = 0xD;
    keymap[SDL_SCANCODE_A] = 0x7;
    keymap[SDL_SCANCODE_S] = 0x8;
    keymap[SDL_SCANCODE_D] = 0x9;
    keymap[SDL_SCANCODE_F] = 0xE;
    keymap[SDL_SCANCODE_Z] = 0xA;
    keymap[SDL_SCANCODE_X] = 0x0;
    keymap[SDL_SCANCODE_C] = 0xB;
    keymap[SDL_SCANCODE_V] = 0xF;

    std::cout << "Initialized key map." << std::endl;
}

void chip8::init_font()
{
    uint8_t characters[5 * 16] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    memcpy(memory + FONT_MEMORY_LOCATION, characters, 5 * 16 * sizeof(uint8_t));

    std::cout << "Initialized font." << std::endl;
}

void chip8::key_pressed(SDL_Scancode scancode)
{
    if (keymap.contains(scancode))
        keys.set(keymap[scancode], true);
}

void chip8::key_released(SDL_Scancode scancode)
{
    if (keymap.contains(scancode))
        keys.set(keymap[scancode], false);
}

void chip8::update()
{
    for (int i = 0; i < INSTRUCTION_PER_FRAME; ++i)
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
            default:
                    std::cerr << "Unknown instruction was met: " << opcode << std::endl;
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
        case 0xB:
            OP_BNNN();
            break;
        case 0xC:
            OP_CXNN();
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
                    OP_8XY0();
                    break;
                case 0x1:
                    OP_8XY1();
                    break;
                case 0x2:
                    OP_8XY2();
                    break;
                case 0x3:
                    OP_8XY3();
                    break;
                case 0x4:
                    OP_8XY4();
                    break;
                case 0x5:
                    OP_8XY5();
                    break;
                case 0x6:
                    OP_8XY6();
                    break;
                case 0x7:
                    OP_8XY7();
                    break;
                case 0xE:
                    OP_8XYE();
                    break;
                default:
                    std::cerr << "Unknown instruction was met: " << opcode << std::endl;
                }
            }
            break;
        case 0xE:
            {
                uint8_t nn = opcode & 0x00FF;
                switch (nn)
                {
                case 0x9E:
                    OP_EX9E();
                    break;
                case 0xA1:
                    OP_EXA1();
                    break;
                default:
                    std::cerr << "Unknown instruction was met: " << opcode << std::endl;
                }
            }
            break;
        case 0xF:
            {
                uint8_t nn = opcode & 0x00FF;
                switch (nn)
                {
                case 0x07:
                    OP_FX07();
                    break;
                case 0x15:
                    OP_FX15();
                    break;
                case 0x18:
                    OP_FX18();
                    break;
                case 0x1E:
                    OP_FX1E();
                    break;
                case 0x0A:
                    OP_FX0A();
                    break;
                case 0x29:
                    OP_FX29();
                    break;
                case 0x33:
                    OP_FX33();
                    break;
                case 0x55:
                    OP_FX55();
                    break;
                case 0x65:
                    OP_FX65();
                    break;
                default:
                    std::cerr << "Unknown instruction was met: " << opcode << std::endl;
                }
            }
            break;
        default:
            std::cerr << "Unknown instruction was met: " << opcode << std::endl;
            break;
        }
    }

    // Update timers
    if (delay_timer > 0)
        --delay_timer;
    if (sound_timer > 0)
    {
        audio_play.play_sound();
        --sound_timer;
    }
}

void chip8::render(window_renderer& renderer)
{
    SDL_Renderer* sdl_renderer = renderer.get_renderer();
    uint32_t pixels[64 * 32];

    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            uint32_t color = (screen[x + y * 64] == 1) ? 0xFFFFFFFF : 0xFF000000;
            pixels[x + y * 64] = color;
        }
    }

    SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(uint32_t));

    //SDL_SetRenderDrawColor(sdl_renderer, 0,0,0,255);
    SDL_RenderClear(sdl_renderer);

    //SDL_SetRenderDrawColor(sdl_renderer, 255,255,255,255);
    SDL_RenderTexture(sdl_renderer, texture, nullptr, nullptr);

    SDL_RenderPresent(sdl_renderer);
}

