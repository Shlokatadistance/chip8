#pragma once
#include <cstdint>
class Chip8
{
    public:
        Chip8();
        void LoadROM(char const* filename);
    private:
        uint8_t registers[16]{}; // unsigned integer of length 8 bits
        uint8_t memory[4096]{};
        uint16_t index{};
        uint16_t pc{};
        uint16_t stack[16]{};
        uint8_t sp{};
        uint8_t delayTimer{};
        uint8_t soundTimer{};
        uint8_t keypad[16]{};
        uint32_t video[64*32]{};
        uint16_t opcode;
};