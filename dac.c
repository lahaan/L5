#include "dac.h"

// RESERVED VARIABLE - DO NOT USE
extern uint32_t gpio_portb_data;

void DACInit(void) {
    volatile uint32_t delay;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // Enable clock gating for Port B
    delay = SYSCTL_RCGC2_R;              // Short delay to ensure clock is stable
    GPIO_PORTB_DIR_R |= 0x0F;            // Set PB0-PB3 as outputs (4 LSBs)
    GPIO_PORTB_DR8R_R |= 0x0F;           // Enable 8 mA drive for PB0-PB3
    GPIO_PORTB_DEN_R |= 0x0F;            // Enable digital I/O on PB0-PB3
    GPIO_PORTB_DATA_R &= ~0x0F;          // Clear PB0-PB3
}

void DACOut(uint32_t data) {
    GPIO_PORTB_DATA_R = (GPIO_PORTB_DATA_R & ~0x0F) | (data & 0x0F);
}