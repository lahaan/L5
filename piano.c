#include "piano.h"

// RESERVED VARIABLE - DO NOT USE
extern uint32_t gpio_porte_data;

void PianoInit(void) {
    volatile uint32_t delay;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE; // Enable clock gating for Port E
    delay = SYSCTL_RCGC2_R;              // Short delay to ensure clock is stable
    GPIO_PORTE_AMSEL_R &= ~0x0F;         // Clear PE0-PE3 analog functionality
    GPIO_PORTE_AFSEL_R &= ~0x0F;         // Disable alternate functions on PE0-PE3
    GPIO_PORTE_DIR_R &= ~0x0F;           // Set PE0-PE3 as inputs
    GPIO_PORTE_DEN_R |= 0x0F;            // Enable digital I/O on PE0-PE3
    GPIO_PORTE_DATA_R &= ~0x0F;          // Clear PE0-PE3 data (optional safety step)
}

uint32_t PianoInput(void) {
    return GPIO_PORTE_DATA_R & 0x0F; 
}