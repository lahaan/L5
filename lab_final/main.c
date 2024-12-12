#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "i2c_helper.h"
#include "ssd1306.h"
#include "game_logic.h"

void PortFInit(void) {
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // Turn on the clock for Port F
    volatile uint32_t delay = SYSCTL_RCGC2_R; // Delay to allow clock to stabilize
    GPIO_PORTF_LOCK_R = 0x4C4F434B;      // Unlock PortF
    GPIO_PORTF_CR_R |= 0x19;             // Allow changes to PF4, PF3, PF0
    GPIO_PORTF_AMSEL_R &= ~0x19;         // Disable analog functionality
    GPIO_PORTF_PCTL_R &= 0x000FF00F;     // Clear PCTL for PF4, PF3, PF0
    GPIO_PORTF_DIR_R &= ~0x11;           // PF4 and PF0 as input (SW1, SW2)
    GPIO_PORTF_DIR_R |= 0x08;            // PF3 as output (Green LED)
    GPIO_PORTF_AFSEL_R &= ~0x19;         // Disable alternate functions
    GPIO_PORTF_PUR_R |= 0x11;            // Enable pull-up resistors on PF4 and PF0
    GPIO_PORTF_DEN_R |= 0x19;            // Enable digital functionality on PF4, PF3, PF0
    GPIO_PORTF_DATA_R &= ~0x08;          // Initially turn off the green LED (PF3)
}

void SysTickInit(void) {
    NVIC_ST_CTRL_R = 0;                  // Disable SysTick during setup
    NVIC_ST_RELOAD_R = 0;                // Set RELOAD to 0 - tbh 16000 - 1 = 1000Hz
    NVIC_ST_CURRENT_R = 0;              // Clear current value
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF); // Set priority to 0
    NVIC_ST_CTRL_R = 0x07;              // Enable SysTick with core clock and interrupts
}

int main(void) {
    PortFInit();   // SW1 and SW2 switch, PF3 Green LED + connected Buzzer
    SysTickInit();
    I2C3Config();
    DisplayInit();
    GameLoop();
}