#include "CPU.h"
#include "Platform.h"
#include <chrono>
#include <iostream>


int main(int argc, char** argv) {
	if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

	int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]);
	char const* romFilename = argv[3];

	Platform platform("CHIP-8 Emulator", CPU::VID_WIDTH * videoScale, CPU::VID_HEIGHT * videoScale, CPU::VID_WIDTH, CPU::VID_HEIGHT);

	CPU chip8;
	chip8.LoadROM(romFilename);

	int videoPitch = sizeof(chip8.videoOutput[0]) * CPU::VID_WIDTH;
	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit) {
		quit = platform.ProcessInput(chip8.keyInput);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;

			chip8.Cycle();

			platform.Update(chip8.videoOutput, videoPitch);
		}
	}

	return 0;
}
