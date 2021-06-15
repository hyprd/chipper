#pragma once
#include <stdint.h>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "SDL2/SDL.h"

class Chip8 {
    public: 
        Chip8();
        ~Chip8();
        void Initialise(char* name);
        void LoadFontsetToMemory();
        void LoadROM(char* filename);
        void Cycle();
        void ExecuteInstruction();

        bool drawNeeded;
        uint8_t display[2048];
        uint8_t keypad[16];
        uint8_t keymap[16] = {  
            SDLK_1, SDLK_2, SDLK_3, SDLK_4,
            SDLK_q, SDLK_w, SDLK_e, SDLK_r,
            SDLK_a, SDLK_s, SDLK_d, SDLK_f,
            SDLK_z, SDLK_x, SDLK_c, SDLK_v 
        };

    private:
        uint16_t pc;
        uint16_t sp;
        uint16_t I; 
        uint16_t stack[16];
        uint8_t memory[4096];
        uint8_t delayTimer;
        uint8_t soundTimer;
        uint8_t V[16];
};