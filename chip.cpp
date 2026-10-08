#include<cstdint>
#include "chip.hpp"
#include <fstream>
#include <random>
#include <chrono>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONT_SIZE = 80;
const unsigned int FONT_START_ADDRESS = 0x50;

// each character is 5 bytes. 
uint8_t fontset[FONT_SIZE] =
{
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


// actual constructors of the Chip8 class
// random number generator to account for this one function that loads a random number into a register
Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    for (int i =0; i < FONT_SIZE ; ++i){
        memory[FONT_START_ADDRESS+i] = fontset[i];
    }
    randByte = std::uniform_int_distribution<uint8_t>(0,255U);
}



void Chip8::LoadROM(char const* filename){
    // RAII - Resource acquisition is initialization
    // The file is autoclosed once it goes out of scope
    std::ifstream file(filename,std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        std::streampos size = file.tellg(); // gives the position of the read pointer / position
        char* buffer = new char[size]; // using the read position, you set the size of the buffer
        file.seekg(0,std::ios::beg);
        file.read(buffer,size);
        file.close();

        for (long i =0;i <size;++i){
            memory[START_ADDRESS + i] = buffer[i];
        }

        delete[] buffer;

    }
}
void Chip8::OP_00E0(){
    // Clear the screen i.e. set all to 0
    memset(video,0,sizeof(video));
}
