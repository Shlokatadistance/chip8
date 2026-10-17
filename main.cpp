#include "chip.hpp"
#include "platform.hpp"
#include <iostream>
#include <chrono>

int main(int argc,char* argv[])
{
    if (argc != 2){
        std::cerr << "Usage Error";
        std::exit(EXIT_FAILURE);
    }
    char const* rom_name = argv[1];
    std::cout << "Starting Chip8" << std::endl;
    std::cout << "Loading renderer" << std::endl;

    Platform Platform("CHIP-8 Emulator",VIDEO_WIDTH*10,VIDEO_HEIGHT*10,VIDEO_HEIGHT,VIDEO_WIDTH);

    Chip8 chip8; // declare chip8
    chip8.LoadROM("/opt/chip8/PONG"); // load in the rom file,passing it statically now

    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
    const int cycleDelay = 1;
	bool quit = false;

    while (!quit){
        quit = Platform.ProcessInput(chip8.keypad);
        auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;

			chip8.Cycle();

			Platform.Update(chip8.video, videoPitch);
		}
    }
}