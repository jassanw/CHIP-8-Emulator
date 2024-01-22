#include <stdio.h>
#include "chip8.h"

#ifdef _WIN32
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 640

char shouldQuit = 0;

int main(int argc, char **args)
{
    //The window we'll be rendering to
    SDL_Window* window = NULL;
   
    u_int8_t* keyboardstate  = SDL_GetKeyboardState(NULL);

    chip8 chip8;

    init_cpu(&chip8);

    
    while(!shouldQuit) {
        //next cpu cycle
        cpu_cycle(&chip8);
         


        //Render Graphics

        //handle input
        SDL_PumpEvents();
        
        if(keyboardstate[SDL_SCANCODE_ESCAPE]) {
            shouldQuit = 1;
        }

        for(int i = 0; i < NUM_OF_KEYS; i++) {
            chip8.keyboard[i] = keyboardstate[kepmap[i]];
        }
    }

    return 0;
}




