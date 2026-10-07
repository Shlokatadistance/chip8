### Facts
-> 16 8 Bit registers
Labels - V0 to VF
Register can hold any value from 0x00 to 0xFF ( 0 to 255 )
4096 bytes of memory, meaning memory space is from 
0x000 to 0xFFF

Memory Split
0x000 - 0x1FF = Reserved for the interpreter
0x05-0x0A0 = Storage space for the 16 supported characters ( 0 to F )
0x200-0xFFF = ROM instructions

16 characters that the rom expects. Each character is 5 bytes. So a tatal of 80 bytes of fontset.