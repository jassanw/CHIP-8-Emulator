#ifdef _WIN32
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "chip8.h"

void init_cpu(chip8 *chip8)
{
    srand(time(NULL));
    
    // clear ram
    memset(chip8->memory, 0 , MEMORY_SIZE);

    // clear display memory
    memset(chip8->display, 0 , DISPLAY_HEIGHT * DISPLAY_WIDTH);

    // clear stack
    memset(chip8->stack, 0 , STACK_SIZE);
    
    // clear general purpose registers
    memset(chip8->V, 0 , V_REGISTER_SIZE);

    // clear input
    memset(chip8->keyboard, 0 , NUM_OF_KEYS);

    // Set Program Counter To 0x200 which is start of program
    chip8->pc = PC_START;

    // clear index register
    chip8->I = 0;

    chip8->delaytimer = 0;

    chip8->soundtimer = 0;

    chip8->drawflag = 0;

    // load font into memory in address range (0x50-0x9F)
    for (int i = 0x50; i <= 0x9F; i++)
    {
        chip8->memory[i] = fontset[i - 0x50];
    }
}

void cpu_cycle(chip8 *chip8)
{

    unsigned short opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];

    // Increment program counter
    chip8->pc += 2;

    // Used to look up one of the 16 registers (VX)
    unsigned char x = (opcode & 0x0F00) >> 8;

    // Used to look up one of the 16 registers (VY)
    unsigned char y = (opcode & 0x00F0) >> 4;

    unsigned short n = (opcode & 0x000F);

    unsigned short nn = (opcode & 0x0FF0) >> 4;

    unsigned short nnn = (opcode & 0x0FFF);

    switch (opcode & 0xF000)
    {
    case 0x0000:
        switch (opcode & 0x00FF)
        {
        // 00E0: clear display
        case 0x00E0:
            clear_display(chip8);
            break;

        // 00EE: Returning from a subroutine
        case 0x00EE:
            chip8->pc = chip8->stack[chip8->stackpointer];
            chip8->stackpointer--;
            break;

        default:
            printf("ERROR: Unkown Opcode 0x%X\n", opcode);
            exit(EXIT_FAILURE);
        }
        break;

    // 1NNN: set PC to NNN
    case 0x1000:
        chip8->pc = nnn;
        break;

    // 2NNN: calls the subroutine at memory location NNN
    case 0x2000:
        chip8->stackpointer++;
        chip8->stack[chip8->stackpointer] = chip8->pc;
        chip8->pc = nnn;
        break;

    // 3XNN: skip instruction or noop
    case 0x3000:
        if (chip8->V[x] == nn)
        {
            chip8->pc += 2;
        }
        break;

    // 4XNN: skip instruction
    case 0x4000:
        if (chip8->V[x] != nn)
        {
            chip8->pc += 2;
        }
        break;

    // 5XY0: skip instruction
    case 0x5000:

        if (chip8->V[x] == chip8->V[y])
        {
            chip8->pc += 2;
        }
        break;

    // 6XNN: skip instruction
    case 0x6000:
        chip8->V[x] = nn;
        break;

    // 7XNN: skip instruction
    case 0x7000:
        chip8->V[x] += nn;
        break;

    // Logical and arithmetic instructions
    case 0x8000:
        switch (opcode & 0x000F)
        {
        // 8XY0: Set
        case 0x0000:
            chip8->V[x] = chip8->V[y];
            break;

        // 8XY1: Binary OR
        case 0x0001:
            chip8->V[x] = chip8->V[x] | chip8->V[y];
            break;

        // 8XY2: Binary AND
        case 0x0002:
            chip8->V[x] = chip8->V[x] & chip8->V[y];
            break;

        // 8XY3: Logical XOR
        case 0x0003:
            chip8->V[x] = chip8->V[x] ^ chip8->V[y];
            break;

        // 8XY4: Add
        case 0x0004:
            chip8->V[x] = chip8->V[x] + chip8->V[y];
            chip8->V[0xF] = (chip8->V[x] + chip8->V[y]) > 0xFF ? 1 : 0;
            break;

        // 8XY5: Subtract
        case 0x0005:
            chip8->V[x] = chip8->V[x] - chip8->V[y];
            chip8->V[0xF] = (chip8->V[x] > chip8->V[y]) > 1 ? 1 : 0;
            break;

        // 8XY6: Shift
        case 0x0006:
            chip8->V[x] = chip8->V[y] >> 1;
            chip8->V[0x0F] = chip8->V[y] & 0x1;
            break;

        // 8XY7: Subtract
        case 0x0007:
            chip8->V[x] = chip8->V[y] - chip8->V[x];
            chip8->V[0xF] = (chip8->V[y] > chip8->V[x]) > 1 ? 1 : 0;
            break;

        // 8XYE: Shift
        case 0x000E:
            chip8->V[x] = chip8->V[y] << 1;
            chip8->V[0x0F] = chip8->V[y] & 0xF0;
            break;
        }

    // 9XY0: skip instruction
    case 0x9000:
        if (chip8->V[x] != chip8->V[y])
        {
            chip8->pc += 2;
        }
        break;

    // ANNN: Set index
    case 0xA000:
        chip8->I = nnn;
        break;

    // BNNN: Jump with offset
    case 0xB000:
        chip8->pc = nnn + chip8->V[0x0];

    // CXNN: Random
    case 0xC000:
        chip8->V[x] = (rand() % 256) & nn;

    case 0xD000:
        unsigned char xcord = chip8->V[x] % DISPLAY_WIDTH;
        unsigned char ycord = chip8->V[y] % DISPLAY_HEIGHT;

        chip8->V[0xF] = 0;

        for (int row = 0; row < n; row++)
        {
            unsigned char pixel = chip8->memory[chip8->I + row];
            for (int col = 0; col < 8; col++)
            {

                if (pixel & (0x80 >> col) != 0)
                {

                    if (chip8->display[xcord + (ycord * 64)] != 0)
                    {
                        chip8->V[0xF] = 1;
                    }

                    chip8->display[xcord + (ycord * 64)] ^= 1;
                }
                xcord++;
            }
            ycord++;
        }

        chip8->drawflag = 1;

        break;

    case 0XE000:
        switch (opcode & 0x00FF)
        {
        case 0x009E:
            if (chip8->keyboard[chip8->V[x]])
            {
                chip8->pc += 2;
            }
            break;
        case 0x00A1:
            if (chip8->keyboard[chip8->V[x]] == 0)
            {
                chip8->pc += 2;
            }
            break;
        }
        break;

    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007:
            chip8->V[x] = chip8->delaytimer;
            break;
        case 0x0015:
            chip8->delaytimer = chip8->V[x];
            break;
        case 0x0018:
            chip8->soundtimer = chip8->V[x];
            break;

        case 0x001E:
            chip8->I += chip8->V[x];

            // This is only relevant to the CHIP-8 interpreter for the AMIGA
            // No programs rely on this not happening so might as well include it
            if ((chip8->I + chip8->V[x]) > 0xFFF)
            {
                chip8->V[0xF] = 1;
            }

        case 0x000A:
            unsigned char waskeypressed = 0;

            for (int i = 0; i < NUM_OF_KEYS; i++)
            {
                if (chip8->keyboard[i])
                {
                    chip8->V[x] = i;
                    waskeypressed = 1;
                    break;
                }
            }

            if (waskeypressed == 0)
            {
                chip8->pc -= 2;
            }
            break;

        case 0x0029:
            chip8->I = FONTSET_START_ADDRESS + (chip8->V[x] * 5);
            break;

        case 0x0033:
            chip8->memory[chip8->I] = chip8->V[x] / 100;
            chip8->memory[chip8->I + 1] = (chip8->V[x] % 100) / 10;
            chip8->memory[chip8->I + 1] = chip8->V[x] % 10;
            break;

        case 0x0055:
            for (int i = 0; i <= x; i++)
            {
                chip8->memory[chip8->I + i] = chip8->V[i];
            }
            break;

        case 0x0065:
            for (int i = 0; i <= x; i++)
            {
                chip8->V[i] = chip8->memory[chip8->I + i];
            }
            break;
        }
    }

    if(chip8->delaytimer > 0) {
        chip8->delaytimer--;
    }

    if(chip8->soundtimer > 0){
        chip8->soundtimer--;
    }
}

void clear_display(chip8 *chip8)
{
    for (int i = 0; i < DISPLAY_HEIGHT * DISPLAY_WIDTH; i++)
    {
        chip8->display[i] = 0;
    }
}
