#include "sound.h"
#include "dac.h"

// RESERVED VARIABLE - DO NOT USE
extern uint32_t nvic_st_reload;

const uint32_t sinewave[] = {
    7, 8, 9, 10, 11, 12, 13, 14,
    15, 15, 15, 14, 13, 12, 11, 10,
    9, 8, 7, 6, 5, 4, 3, 2,
    1, 1, 1, 2, 3, 4, 5, 6
};
uint32_t i = 0;

void SoundInit(void) {
    NVIC_ST_CTRL_R = 0;                  // Disable SysTick
    NVIC_ST_RELOAD_R = 0x00FFFFFF;       // Maximum reload value for initial setup
    NVIC_ST_CURRENT_R = 0;               // Any write clears the current register
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x20000000;
    NVIC_ST_CTRL_R = 0x07;               // Enable SysTick with core clock and interrupts
}

void SoundPlay(uint32_t period) {
    if (period == 0) {
        NVIC_ST_CTRL_R = 0;  // Disable SysTick if period is 0
    } else {
        NVIC_ST_RELOAD_R = period - 1; // Set reload value for desired frequency
        NVIC_ST_CURRENT_R = 0;        // Clear the current value register
        NVIC_ST_CTRL_R |= 0x01;
    }
}

void SysTickHandler(void) {
    DACOut(sinewave[i]);     // Output one value from the sinewave to DAC
    i = (i + 1) % 32;   
}

uint32_t ChooseNote(uint32_t input) {
    switch (input) {
        case 0x01: // C
            return (uint32_t)(16000000 / (523.251 * 32) + 0.5);
        case 0x02: // D
            return (uint32_t)(16000000 / (587.330 * 32) + 0.5);
        case 0x04: // E 
            return (uint32_t)(16000000 / (659.255 * 32) + 0.5);
        case 0x08: // G 
            return (uint32_t)(16000000 / (783.991 * 32) + 0.5);
        default: 
            return 0;
    }
}