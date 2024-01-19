#ifndef CHIP8_H
#define CHIP8_H

#ifdef _WIN32
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif


#define DISPLAY_HEIGHT 32
#define DISPLAY_WIDTH 64
#define PC_START 0x200
#define MEMORY_SIZE 4096
#define STACK_SIZE 16
#define NUM_OF_KEYS 16
#define V_REGISTER_SIZE 16
#define FONTSET_SIZE = 80;
#define FONTSET_START_ADDRESS  0x50;

// Chip-8 Font Set
unsigned char fontset[80] = {
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

// Key Map
SDL_Scancode kepmap[16] = {
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
    SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
    SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V

};


typedef struct chip8
{
    unsigned char memory[MEMORY_SIZE];         // Random Access Memory

    unsigned char display[DISPLAY_HEIGHT*DISPLAY_WIDTH];

    unsigned char stackpointer;
    unsigned short stack[STACK_SIZE];           // chip-8 usually has at least 16 two-byte entries

    unsigned short pc;                  // Program Counter
    unsigned short I;                   // Index Register
    unsigned char delaytimer;
    unsigned char soundtimer;

    unsigned char V[V_REGISTER_SIZE];                //16 8-bit general purpose registers (denoted Vx where x is hex value)

    unsigned char keyboard[NUM_OF_KEYS];

    unsigned char drawflag;
} chip8;


void init_cpu(chip8* cpu);

void cpu_cycle(chip8* cpu);

void clear_display(chip8* cpu); 

#endif
