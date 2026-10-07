#include "chip.hpp"
#include <iostream>


int main()
{
    std::cout << "Starting Chip8" << std::endl;

    Chip8 chip8; // declare chip8
    chip8.LoadROM("/opt/chip8/PONG"); // load in the rom file,passing it statically now
}