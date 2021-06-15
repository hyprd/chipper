#define SDL_MAIN_HANDLED
#include "chip8.h"

int main(int argc, char *argv[]) { 
    SDL_Window* window =  NULL;
    SDL_Renderer* renderer = NULL;
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        return 1; 
    }
    window = SDL_CreateWindow("Chipper - CHIP8 Interpreter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetScale(renderer, 10, 10);
    Chip8* chip8 = new Chip8;
    chip8->Initialise(argv[1]);
    for(;;) {
        chip8->Cycle();
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return 0;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    return 0;
                }
                if (e.key.keysym.sym == SDLK_F1){
                    return 0;
                }
                for (int i = 0; i < 16; ++i) {
                    if (e.key.keysym.sym == chip8->keymap[i]) {
                        chip8->keypad[i] = 1;
                    }
                }
            }
            if (e.type == SDL_KEYUP) {
                for (int i = 0; i < 16; ++i) {
                    if (e.key.keysym.sym == chip8->keymap[i]) {
                        chip8->keypad[i] = 0;
                    }
                }
            }
        }

        if(chip8->drawNeeded) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            for(int y = 0; y < 32; ++y) {
                for(int x = 0; x < 64; ++x) {
                    if(chip8->display[(y * 64) + x] != 0) SDL_RenderDrawPoint(renderer, x, y); 
                }
            }
            SDL_RenderPresent(renderer);
            chip8->drawNeeded = false;
        }
        SDL_Delay(10);
    }
    return 0;
}
