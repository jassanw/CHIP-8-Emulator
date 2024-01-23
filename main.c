#ifdef _WIN32
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#include <unistd.h>
#include <stdio.h>
#include "chip8.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 640

char shouldQuit = 0;
SDL_Window *gWindow = NULL;

int main(int argc, char **args)
{
    if (argc < 2)
    {
        printf("No path to ROM provided\n");
    }

    chip8 chip8;
    initcpu(&chip8);

    const char *romPath = args[1];

    FILE *file = fopen(romPath, "rb");

    if (file == NULL)
    {
        printf("Error opening file");
        return 1;
    }

    fseek(file, 0, SEEK_END); // move the file pointer to the end of the file
    long fileSize = ftell(file);
    rewind(file);

    unsigned char programData[fileSize];

    const size_t ret_code = fread(&programData, sizeof(char), fileSize, file);

    for(int i = 0; i < fileSize-1; i+=2) {
        unsigned short opcode = (programData[i] << 8) | programData[i+ 1];
        printf("Opcode 0x%X\n", opcode);
    }


    if (ret_code == fileSize)
    {
        loadprogram(&chip8, programData, fileSize);
    }
    else
    {
        printf("ERROR: could not read entire program");
        exit(EXIT_FAILURE);
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    gWindow = SDL_CreateWindow("CHIP 8 Emu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    const u_int8_t *keyboardstate = SDL_GetKeyboardState(NULL);

    SDL_Renderer *renderer = NULL;
    renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    while (!shouldQuit)
    {
        // next cpu cycle
        cpucycle(&chip8);

        // Render Graphics
        if(chip8.drawflag) {
            SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
            SDL_RenderClear(renderer);
            
            SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
    
            for(int i = 0; i < DISPLAY_HEIGHT; i++) {
                for(int j = 0; j < DISPLAY_WIDTH; j++) {
                    unsigned char pixelon = chip8.display[(i*DISPLAY_WIDTH) + j];
                    if(pixelon) { 
                        SDL_Rect pixel; 
                        pixel.h = 20;
                        pixel.w = 20;
                        pixel.y = i*20;
                        pixel.x = j*20;

                        SDL_RenderFillRect(renderer, &pixel);
                    }
                }
            }
            SDL_RenderPresent(renderer);
        }
        
        // handle input
        SDL_PumpEvents();

        if (keyboardstate[SDL_SCANCODE_ESCAPE])
        {
            shouldQuit = 1;
        }

        for (int i = 0; i < NUM_OF_KEYS; i++)
        {
            chip8.keyboard[i] = keyboardstate[kepmap[i]];
        }

        usleep(1000);
    }

    return 0;
}
