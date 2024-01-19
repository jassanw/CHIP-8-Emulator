#ifdef _WIN32
#include <SDL/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "chip8.h"


void init_cpu(chip8* cpu) {

    //clear ram
    for(int i = 0; i < MEMORY_SIZE; i++) {
        cpu->memory[i] = 0;
    }

    //clear display memory
    for(int i = 0; i < DISPLAY_HEIGHT*DISPLAY_WIDTH; i++) {
        cpu->display[i] = 0;
    }

    //clear stack
    for(int i = 0; i < STACK_SIZE; i++) {
        cpu->stack[i] = 0;
    }

    //clear general purpose registers
    for(int i = 0; i < V_REGISTER_SIZE; i++) {
        cpu->V[i] = 0;
    }

    //clear input
    for(int i = 0; i < NUM_OF_KEYS; i++) {
        cpu->keyboard[i] = 0;
    }

    //Set Program Counter To 0x200 which is start of program
    cpu->pc = PC_START;

    //clear index register
    cpu->I = 0;


    cpu->delaytimer = 0;

    cpu->soundtimer = 0;

    //load font into memory in address range (0x50-0x9F)
    for(int i = 0x50; i <= 0x9F; i++) {
        cpu->memory[i] = fontset[i-0x50];
    }
}



void cpu_cycle(chip8* cpu) {

    unsigned short opcode = (cpu->memory[cpu->pc] << 8) | cpu->memory[cpu->pc+1];
    
    //NOTE: if something erros move program counter inc to case statement
    cpu->pc += 2;

    //Used to look up one of the 16 registers (VX)
    unsigned short x = (opcode & 0x0F00) >> 8;

    //Used to look up one of the 16 registers (VY)
    unsigned short y = (opcode & 0x00F0) >> 4;

    unsigned short n = (opcode & 0x000F);

    unsigned short nn = (opcode & 0x0FF0) >> 4;

    unsigned short nnn = (opcode & 0x0FFF);

    switch(opcode & 0xF000) {
        case 0x0000 : 
            switch(opcode & 0x00FF) {
                //00E0 clear display
                case 0x00E0:
                    clear_display(cpu);
                    break;

                //00EE Returning from a subroutine
                case 0x00EE:
                    cpu->pc =  cpu->stack[cpu->stackpointer--];
                    break;

                default:
                    printf("ERROR: Unkown Opcode 0x%X\n", opcode);
                    exit(EXIT_FAILURE);
            }
            break;
        
        //1NNN set PC to NNN
        case 0x1000: 
            cpu->pc = nnn;
            break;

        //2NNN calls the subroutine at memory location NNN
        case 0x2000:
            cpu->stackpointer++;
            cpu->stack[cpu->stackpointer] = cpu->pc;
            cpu->pc = nnn;
            break;

        //3XNN skip instruction or noop
        case 0x3000:
            if(cpu->V[x] == nn) {
                cpu->pc += 2;
            }
            break;
      
        //4XNN skip instruction or noop
        case 0x4000:
           
            //skip instruction
            if(cpu->V[x] != nn) {
                cpu->pc += 2;
            }
            break;

        case 0x5000:
            switch(opcode & 0x000F) {
                //5XY0 skip instruction or noop
                case(0x0000):
                    if(cpu->V[x] == cpu->V[y]) {
                        cpu->pc += 2;
                    }
                    break;
            }
            break;
        
        //6XNN
        case 0x6000:
            cpu->V[x] = nn;
            break;
     
        //7XNN
        case 0x7000:
            cpu->V[x] += nn;

        
        case 0x9000:
            switch(opcode & 0x000F) {
                //9XY0
                case 0x0000:
                    if(cpu->V[x] != cpu->V[y]) {
                        cpu->pc += 2;
                    }
            }
    }
}

void clear_display(chip8* cpu) {
    for(int i = 0; i < DISPLAY_HEIGHT*DISPLAY_WIDTH; i++) {
        cpu->display[i] = 0;
    }
}


