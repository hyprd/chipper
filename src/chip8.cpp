#include "chip8.h"

Chip8::Chip8(){
    memset(stack, 0, sizeof(stack));
    memset(V, 0, sizeof(V));
    memset(keypad, 0, sizeof(keypad));
    memset(display, 0, sizeof(display));
    memset(memory, 0, sizeof(memory));
    delayTimer = 0;
    soundTimer = 0;
    I = 0;
    pc = 0;
    sp = 0;
}

Chip8::~Chip8(){}

void Chip8::Initialise(char* name) {
    LoadFontsetToMemory();
    LoadROM(name);
}

void Chip8::LoadROM(char* fname) {
    FILE* f = fopen(fname, "rb");
    fseek(f, 0, SEEK_END);
    unsigned long size = ftell(f);
    rewind(f);
    fread(memory, 1, size, f);
    fclose(f);
}

void Chip8::Cycle() {
    ExecuteInstruction();
}

void Chip8::ExecuteInstruction() {
    uint16_t opcode = static_cast<uint16_t>(memory[pc] << 8 | memory[pc + 1]);
    switch(opcode) {
        case 0x00E0:
            memset(display, 0, sizeof(display));
            drawNeeded = true;
            break;
        case 0x00EE:
            sp -= 1;
            pc = stack[sp];
            break;
        case 0x1000:
            pc = opcode & 0x0FFF;
            break;
        case 0x2000:
            stack[sp] = pc;
            sp += 1;
            pc = opcode & 0x0FFF;
            break;
        case 0x3000:
        {
            uint16_t X = opcode & 0x0F00 >> 8; // shift to lower nibbles
            uint16_t NN = opcode & 0x00FF;
            if(V[X] == NN) pc += 2;
            break;
        }
        case 0x4000:
        {
            uint16_t X = opcode & 0x0F00 >> 8; 
            uint16_t NN = opcode & 0x00FF;
            if(V[X] != NN) pc += 2;
            break;
        }
        case 0x5000:
        {
            uint16_t X = opcode & 0x0F00 >> 8; 
            uint16_t Y = opcode & 0x00F0 >> 4;
            if(V[X] == V[Y]) pc += 2;
            break;
        }
        case 0x6000:
        {
            uint16_t X = opcode & 0x0F00 >> 8;
            uint16_t NN = opcode & 0x00FF;
            V[X] = NN;
            break;
        }
        case 0x7000:
        {
            uint16_t X = opcode & 0x0F00 >> 8;
            uint16_t NN = opcode & 0x00FF;
            V[X] += NN;
            break;
        }
        case 0x8000:
        {
            uint16_t X = opcode & 0x0F00 >> 8;
            uint16_t Y = opcode & 0x00F0 >> 4;
            switch(opcode & 0x000F) {   // logic / arthimetic instructions
                case 0x0000:
                    V[X] = V[Y];
                    break;
                case 0x0001:
                    V[X] |= V[Y];
                    break;
                case 0x0002:
                    V[X] &= V[Y];
                    break;
                case 0x0003:
                    V[X] ^= V[Y];
                    break;
                case 0x0004:
                    V[0xF] = (V[X] + V[Y] > 0xFF) ? 1 : 0;
                    V[X] += V[Y];
                    break;
                case 0x0005:
                    V[0xF] = (V[X] - V[Y] < 0) ? 1 : 0;
                    V[X] -= V[Y];
                    break;
                case 0x0006:
                    V[0xF] = V[X] & 0x1;
                    V[X] >>= 1;
                    break;
                case 0x0007:
                    V[0xF] = (V[Y] - V[X]) ? 1 : 0; 
                    break;
                case 0x000E:
                    V[0xF] = V[X] >> 7;
                    V[X] <<= 1;
                    break;
            }
            break;
        }
        case 0x9000:
        {
            uint16_t X = opcode & 0x0F00 >> 8;
            uint16_t Y = opcode & 0x00F0 >> 4;
            if(V[X] != V[Y]) pc += 2;
            break;
        }
        case 0xA000:
        {
            uint16_t NNN = opcode & 0x0FFF;
            I = NNN;
            break;
        }
        case 0xB000:
        {
            uint16_t NNN = opcode & 0x0FFF;
            pc = NNN + V[0];
            break;
        }
        case 0xC000:
        {
            uint16_t X = opcode & 0x0F00 >> 8;
            uint16_t mask = opcode & 0x00FF;
            V[X] = (rand() % 0xFF + 1) & mask;
            break;
        }
        /* 
         * Sprite drawing function.
         * 
         * V[X], V[Y]   ->  Positional coordinates
         * 0x000N       ->  Sprite height
         * 8            ->  Sprite width
         * 
         * XOR to toggle pixels. Start at I and offset by y.
         * 
         * Set V[F] if screen pixels end up switching on render, clear otherwise
         */    
        case 0xD000:
        {
            uint16_t spriteX = opcode & 0x0F00 >> 8;
            uint16_t spriteY = opcode & 0x00F0 >> 4;
            uint16_t height = opcode & 0x000F;
            uint8_t width = 8;
            uint16_t pixel;
            V[0xF] = 0;
            for(int y = 0; y < height; y++) {
                pixel = memory[I + y];
                for(int x = 0; x < width; x++) {
                    // Read as a BCD
                    if(pixel & (128 >> x) != 0) {
                        if(display[(spriteX + x) + (spriteY + y) * 64]) V[0xF] = 1;
                        display[(spriteX + x) + (spriteY + y) * 64] ^= 1;
                    }
                }
            }
            drawNeeded = true;
            break;
        }
        case 0xE000:
        {
            uint16_t X = opcode & 0x0F00 >> 8;
            switch(opcode & 0x00FF) {
                case 0x009E:
                    if(keypad[V[X]] != 0) pc += 2;
                    break;
                case 0x00A1:
                    if(keypad[V[X]] == 0) pc += 2;
                    break;
            }
            break;
        }
        case 0xF000:
        {
            uint16_t X = opcode & 0x0F00 >> 8;
            switch(opcode & 0x00FF) {
                case 0x0007:
                    V[X] = delayTimer;
                    break;
                case 0x000A:
                {
                    bool pressed = false;
                    // 16 = size of keypad
                    for(int i = 0; i < 16; i++) {
                        if(keypad[i]) {
                            V[X] = i;
                            pressed = true;
                        }
                    }
                    if(!pressed) return;
                    break;
                }
                case 0x0015:
                    delayTimer = V[X];
                    break;
                case 0x0018:
                    soundTimer = V[X];
                    break;
                case 0x001E:
                    // set VF to 1 if overflow occurs
                    V[0xF] = 0;
                    if(I + V[X] > 0xFFF) V[0xF] = 1;
                    I += V[X];
                    break; 
                case 0x0029:
                    // Location of sprite for digit V[X].
                    // The value of I is set to the location for the hexadecimal 
                    // sprite corresponding to the value of V[X]
                    I = V[X] * 0x5; 
                    break;
                case 0x0033:
                    // Store BCD representation of V[X] in I, I + 1 & I + 2
                    // 100s digit at I
                    // tens digit at I + 1
                    // ones digit at I + 2
                    memory[I] = X / 100;
                    memory[I + 1] = (V[X] / 10) % 10;
                    memory[I + 2] = V[X] % 10;
                    break;
                case 0x0055:
                    for(int i = 0; i <= X; i++) { memory[I + i] = V[i]; }
                    break;
                case 0x0065:
                    for(int i = 0; i <= X; i++) { V[i] = memory[I + i]; }
                    break;
            }   
        }
        default:
            std::cout << "Unknown opcode" << std::endl;
            return;
    }
    if(delayTimer > 0) delayTimer--; 
    if(soundTimer > 0) soundTimer--; 
    pc += 2;
}

void Chip8::LoadFontsetToMemory() {
    uint8_t fontset[80] = {
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
    uint8_t offset = 0x50; // fontset stored 0x50 -> 0x9F typically
    for(int i = 0; i < 80; i++) {
        memory[i + offset] = fontset[i];
    }
}