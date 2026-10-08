#include "chip.hpp"
#include "platform.hpp"
#include <iostream>


int main()
{
    std::cout << "Starting Chip8" << std::endl;
    std::cout << "Loading renderer" << std::endl;

    Platform Platform("CHIP-8 Emulator",VIDEO_WIDTH*10,VIDEO_HEIGHT*10,VIDEO_HEIGHT,VIDEO_WIDTH);

    Chip8 chip8; // declare chip8
    chip8.LoadROM("/opt/chip8/PONG"); // load in the rom file,passing it statically now

    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

    while (!quit){
        quit = Platform.ProcessInput(chip8.keypad);
    }
}