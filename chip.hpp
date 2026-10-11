#pragma once
#include <cstdint>
#include<random>

const unsigned int VIDEO_HEIGHT = 32;
const unsigned int VIDEO_WIDTH = 64;
class Chip8
{
    public:
        Chip8();
        void Cycle();
        void LoadROM(char const* filename);
        uint32_t video[VIDEO_HEIGHT*VIDEO_WIDTH]{};
        uint8_t keypad[16]{};

    private:
        void Table0();
        void Table8();
        void TableE();
        void TableF();


        uint8_t registers[16]{}; // unsigned integer of length 8 bits
        uint8_t memory[4096]{};
        uint16_t index{};
        uint16_t pc{};
        uint16_t stack[16]{};
        uint8_t sp{};
        uint8_t delayTimer{};
        uint8_t soundTimer{};
        uint16_t opcode;

        std::default_random_engine randGen;
        std::uniform_int_distribution<uint8_t> randByte;

        // Do nothing
        void OP_NULL();

        // Actual instruction
        void OP_00E0();

        // RET
        void OP_00EE();

        // JP address
        void OP_1nnn();

        // CALL address
        void OP_2nnn();

        // SE Vx, byte
        void OP_3xkk();

        // SNE Vx, byte
        void OP_4xkk();

        // SE Vx, Vy
        void OP_5xy0();

        // LD Vx, byte
        void OP_6xkk();

        // ADD Vx, byte
        void OP_7xkk();

        // LD Vx, Vy
        void OP_8xy0();

        // OR Vx, Vy
        void OP_8xy1();

        // AND Vx, Vy
        void OP_8xy2();

        // XOR Vx, Vy
        void OP_8xy3();

        // ADD Vx, Vy
        void OP_8xy4();

        // SUB Vx, Vy
        void OP_8xy5();

        // SHR Vx
        void OP_8xy6();

        // SUBN Vx, Vy
        void OP_8xy7();

        // SHL Vx
        void OP_8xyE();

        // SNE Vx, Vy
        void OP_9xy0();

        // LD I, address
        void OP_Annn();

        // JP V0, address
        void OP_Bnnn();

        // RND Vx, byte
        void OP_Cxkk();

        // DRW Vx, Vy, height
        void OP_Dxyn();

        // SKP Vx
        void OP_Ex9E();

        // SKNP Vx
        void OP_ExA1();

        // LD Vx, DT
        void OP_Fx07();

        // LD Vx, K
        void OP_Fx0A();

        // LD DT, Vx
        void OP_Fx15();

        // LD ST, Vx
        void OP_Fx18();

        // ADD I, Vx
        void OP_Fx1E();

        // LD F, Vx
        void OP_Fx29();

        // LD B, Vx
        void OP_Fx33();

        // LD [I], Vx
        void OP_Fx55();

        // LD Vx, [I]
        void OP_Fx65();

        
        std::default_random_engine randGen;
        std::uniform_int_distribution<uint8_t> randByte;
        
        
        //Chip8 Function table
        typedef void (Chip8::*Chip8Func)();
        // Pointer to a chip8 member function that takes no arguements and returns void
        Chip8Func table[0xF + 1];
        Chip8Func table0[0xE + 1];
        Chip8Func table8[0xE +1];
        Chip8Func tableE[0xE + 1];
        Chip8Func tableF[0x65 +1];

};