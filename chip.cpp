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
    pc = START_ADDRESS;
    for (int i =0; i < FONT_SIZE ; ++i){
        memory[FONT_START_ADDRESS+i] = fontset[i];
    }
    randByte = std::uniform_int_distribution<uint8_t>(0,255U);

    // function pointer table
    table[0x0] = &Chip8::Table0;
	table[0x1] = &Chip8::OP_1nnn;
	table[0x2] = &Chip8::OP_2nnn;
	table[0x3] = &Chip8::OP_3xkk;
	table[0x4] = &Chip8::OP_4xkk;
	table[0x5] = &Chip8::OP_5xy0;
	table[0x6] = &Chip8::OP_6xkk;
	table[0x7] = &Chip8::OP_7xkk;
	table[0x8] = &Chip8::Table8;
	table[0x9] = &Chip8::OP_9xy0;
	table[0xA] = &Chip8::OP_Annn;
	table[0xB] = &Chip8::OP_Bnnn;
	table[0xC] = &Chip8::OP_Cxkk;
	table[0xD] = &Chip8::OP_Dxyn;
	table[0xE] = &Chip8::TableE;
	table[0xF] = &Chip8::TableF;


    for (size_t i = 0; i <= 0xE; i++)
        {
            table0[i] = &Chip8::OP_NULL;
            table8[i] = &Chip8::OP_NULL;
            tableE[i] = &Chip8::OP_NULL;
        }
    
    table0[0x0] = &Chip8::OP_00E0;
	table0[0xE] = &Chip8::OP_00EE;

	table8[0x0] = &Chip8::OP_8xy0;
	table8[0x1] = &Chip8::OP_8xy1;
	table8[0x2] = &Chip8::OP_8xy2;
	table8[0x3] = &Chip8::OP_8xy3;
	table8[0x4] = &Chip8::OP_8xy4;
	table8[0x5] = &Chip8::OP_8xy5;
	table8[0x6] = &Chip8::OP_8xy6;
	table8[0x7] = &Chip8::OP_8xy7;
	table8[0xE] = &Chip8::OP_8xyE;

	tableE[0x1] = &Chip8::OP_ExA1;
	tableE[0xE] = &Chip8::OP_Ex9E;

	for (size_t i = 0; i <= 0x65; i++)
	{
		tableF[i] = &Chip8::OP_NULL;
	}

	tableF[0x07] = &Chip8::OP_Fx07;
	tableF[0x0A] = &Chip8::OP_Fx0A;
	tableF[0x15] = &Chip8::OP_Fx15;
	tableF[0x18] = &Chip8::OP_Fx18;
	tableF[0x1E] = &Chip8::OP_Fx1E;
	tableF[0x29] = &Chip8::OP_Fx29;
	tableF[0x33] = &Chip8::OP_Fx33;
	tableF[0x55] = &Chip8::OP_Fx55;
	tableF[0x65] = &Chip8::OP_Fx65;
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

void Chip8::Cycle(){
    // fetch opcode ( move it 8bits, then logical or)
    opcode = ((memory[pc] << 8u) | memory[pc+1]);
    pc += 2;
    //execute, think of this like "self" in python
    // Take bitwise and of the opcode and move it right 12 bits
    // so 0xA123 & 0xF000 = 0xA000 , then 0xA000 >> 12 = 0xA
    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();

    if (delayTimer > 0)
    {
        --delayTimer;
    } 
    if (soundTimer > 0 )
    {
        --soundTimer;
    }

}
// table functions
void Chip8::Table0(){
    ((*this).*(table0[opcode & 0x000Fu]))();
}

void Chip8::Table8(){
    ((*this).*(table8[opcode & 0x000Fu]))();

}

void Chip8::TableE(){
    ((*this).*tableE[opcode & 0x000Fu])();
}

void Chip8::TableF(){
    ((*this).*tableF[opcode & 0x00FFu])();
}

void Chip8::OP_NULL()
{
    // Does nothing
}

void Chip8::OP_00E0(){
    // Clear the display
    // memset sets the first sizeof(video) bytes of memory pointer by video to 0
    memset(video,0,sizeof(video));
}
// sp is the stack pointer, it tells where in the 16 levels of stack the most recent value is placed
//PC = program counter, which is a register that holds the address of the next instruction to execute.
void Chip8::OP_00EE(){
    //Return from a subroutine
    --sp; // just an unsigned integer
    pc = stack[sp]; // pc goes to where sp is poiting

}

void Chip8::OP_1nnn(){
    // Jump to location 1nnn 
    // This is a plain jump, not a function call, so stack pointer is not touched here
    uint16_t address = opcode & 0x0FFFu;
    pc = address;
}

void Chip8::OP_2nnn(){
    //Call subroutine at nnn
    uint16_t address = opcode & 0x0FFFu;
    stack[sp] = pc;
    ++sp;
    pc = address;
}

void Chip8::OP_3xkk(){
    // skip instruction if Vx = kk
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    
    if (registers[Vx] == byte)
    {
        pc += 2;
    }

}

void Chip8::OP_4xkk(){
    // skip instruction if Vx != kk
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    if (registers[Vx] != byte)
    {
        pc += 2;
    }
}

void Chip8::OP_5xy0(){
    // Skip instruction if vx = vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    
    if (registers[Vx] == registers[Vy])
    {
        pc += 2;
    }
}

void Chip8::OP_6xkk(){
    // Set Vx = kk
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    registers[Vx] = byte;
}

void Chip8::OP_7xkk(){
    // Set Vx = Vx + kk
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    registers[Vx] += byte;
}

void Chip8::OP_8xy0(){
    // Set Vx = Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] = registers[Vy];
}

void Chip8::OP_8xy1(){
    // Set Vx = Vx OR VY
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] = registers[Vx] | registers[Vy];
}

void Chip8::OP_8xy2(){
    // Set Vx = Vx & VY
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] = registers[Vx] & registers[Vy];
}

void Chip8::OP_8xy3(){
    // Set Vx = Vx XOR VY
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] = registers[Vx] ^ registers[Vy];
}

void Chip8::OP_8xy4(){
    //  Add Vx and Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint16_t sum = registers[Vx] + registers[Vy];
    // Sum of two 8 bits is kept in a 16 bit so that the carry
    // Storing it in 16 bits allows you to check if the sum is greater than 255

    if (sum > 255)
    {
        registers[0xF] = 1;
    }
    else{
        registers[0xF] = 0;
    }
    registers[Vx] = sum & 0xFFu;
}


void Chip8::OP_8xy5(){
    // Sub Vx and Vy
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    if (registers[Vx] > registers[Vy])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }
    registers[Vx] -= registers[Vy];
}

void Chip8::OP_8xy6(){
    // Vx = Vx SHR 1, SHR = Shift Right Logical
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    // Save LSB in VF
    registers[0xF] = (registers[Vx] & 0x1u);
    registers[Vx] = registers[Vx] >> 1;
}

void Chip8::OP_8xy7(){
    // Set Vx = Vy - Vy, set VF = Not borrow i.e. flag it
    // Weirdly the same instruction at 8xy5, just that not its Vy-Vx
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    if (registers[Vx] > registers[Vy])
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }
    registers[Vx] = registers[Vy]- registers[Vx];

}

void Chip8::OP_8xyE(){
    // Vx = Vx SHL 1, SHL = shift left
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    // Save the MSB in VF
    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
    registers[Vx] <<= 1;
}

void Chip8::OP_9xy0(){
    // Skip next instruction if Vx != Vy
    // Similar to 4xkk
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    if (registers[Vx] != registers[Vy])
    {
        pc += 2;
    }

}

void Chip8::OP_Annn(){
    // Set I = nnn
    uint16_t address = opcode & 0x0FFFu;
	index = address;
}

void Chip8::OP_Bnnn()
{
    // Jump to location nnn + V0
    uint16_t address = opcode & 0x0FFFu;
    pc = registers[0] + address;
}

void Chip8::OP_Cxkk(){
    // Vx = some random byte and kk
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = randByte(randGen) & byte;
}

void Chip8::OP_Dxyn(){
    // A sprite is 8 pixels wide
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	// Wrap if going beyond screen boundaries
	uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

	registers[0xF] = 0;

	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[index + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

void Chip8::OP_Ex9E(){
    // Skip next instruction if key with Vx value has been pressed
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];

	if (keypad[key])
	{
		pc += 2;
	}
}

void Chip8::OP_ExA1(){
    // Skip next instruction if key with Vx value has not been pressed
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];

	if (!keypad[key])
	{
		pc += 2;
	}
}

void Chip8::OP_Fx07(){
    // Set Vx = delay timer value
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	registers[Vx] = delayTimer;

}

void Chip8::OP_Fx0A()
{
    // Wait for a key press and then store the value of the key in Vx
    // When value is not detected, decrement pc by 2, which is basically like running the same insstruction 
    // again.
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (keypad[0])
	{
		registers[Vx] = 0;
	}
	else if (keypad[1])
	{
		registers[Vx] = 1;
	}
	else if (keypad[2])
	{
		registers[Vx] = 2;
	}
	else if (keypad[3])
	{
		registers[Vx] = 3;
	}
	else if (keypad[4])
	{
		registers[Vx] = 4;
	}
	else if (keypad[5])
	{
		registers[Vx] = 5;
	}
	else if (keypad[6])
	{
		registers[Vx] = 6;
	}
	else if (keypad[7])
	{
		registers[Vx] = 7;
	}
	else if (keypad[8])
	{
		registers[Vx] = 8;
	}
	else if (keypad[9])
	{
		registers[Vx] = 9;
	}
	else if (keypad[10])
	{
		registers[Vx] = 10;
	}
	else if (keypad[11])
	{
		registers[Vx] = 11;
	}
	else if (keypad[12])
	{
		registers[Vx] = 12;
	}
	else if (keypad[13])
	{
		registers[Vx] = 13;
	}
	else if (keypad[14])
	{
		registers[Vx] = 14;
	}
	else if (keypad[15])
	{
		registers[Vx] = 15;
	}
	else
	{
		pc -= 2;
	}
}

void Chip8::OP_Fx15(){
    // Set delay Timer = Vx
    uint8_t Vx = (opcode & 0x0F00u ) >> 8u;
    delayTimer = registers[Vx];
}

void Chip8::OP_Fx18(){
    // Set soundTimer = Vx
    uint8_t Vx = (opcode & 0x0F00u ) >> 8u;
    soundTimer = registers[Vx];
}

void Chip8::OP_Fx1E(){
    // Set I = I + Vx
    uint8_t Vx = (opcode & 0x0F00u ) >> 8u;
    index = index + registers[Vx];
}

void Chip8::OP_Fx29(){
    // Set I = location of sprite for Vx
    // Basically get the address of the first byte taking an offset from the start address
    // Font characters are all 5 bytes
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t digit = registers[Vx];

	index = FONT_START_ADDRESS + (5 * digit);

}

void Chip8::OP_Fx33(){
    // BCD = Binary Coded Decimal representation
    // Store BCD of Vx in I,I+1, I+2
    // places hundreds digit at location in I, tens digit at I+1, ones digit at I+2
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];

	// Ones-place
	memory[index + 2] = value % 10;
	value /= 10;

	// Tens-place
	memory[index + 1] = value % 10;
	value /= 10;

	// Hundreds-place
	memory[index] = value % 10;
}

void Chip8::OP_Fx55(){
    // Store V0 through Vx in location starting I
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    for ( uint8_t i = 0; i <= Vx; ++i)
    {
        memory[index+i] = registers[i];
    }
}

void Chip8::OP_Fx65(){
    // Read registers V0 through Vx starting at I
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    for ( uint8_t i = 0; i <= Vx; ++i)
    {
        registers[i] = memory[index+i];
    }
}