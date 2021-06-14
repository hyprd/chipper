#include "chip8.h"

int main(int argc, char *argv[]) { 
    Chip8* c8 = new Chip8;
    c8->Initialise(argv[1]);
    return 0;
}
