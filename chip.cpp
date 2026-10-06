#include<cstdint>
#include <chip.hpp>
#include <fstream>

const unsigned int START_ADDRESS = 0x200;
void Chip8::LoadROM(char const* filename){
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