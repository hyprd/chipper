#include "chip8.h"

int main(int argc, char *argv[]) { 
    Chip8* chip8 = new Chip8;
    chip8->Initialise(argv[1]);
    for(;;) {
        chip8->Cycle();
        if(chip8->drawNeeded) {
            chip8->drawNeeded = false;
        }
    }
    return 0;
}
