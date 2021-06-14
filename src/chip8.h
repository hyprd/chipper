#include <stdint.h>
#include <cstring>
#include <iostream>

class Chip8 {
    public: 
        Chip8();
        ~Chip8();
        void Initialise(char* name);
        void LoadFontsetToMemory();
        void LoadROM(char* filename);

        uint8_t display[2048];
        uint8_t keypad[16];
    private:
        uint16_t pc;
        uint16_t indexRegister; // aka "I"
        uint16_t stack[16];
        uint8_t memory[4096];
        uint8_t delayTimer;
        uint8_t soundTimer;
        uint8_t variableRegister[16]; // aka "V"
};