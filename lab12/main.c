/*
    Lab12 - Low power mode

    Complete functions in this file and analog_temp.c.

    To launch the UART serial connection, open the terminal and run:
    'picocom -b 115200 -d 8 -p 1 -y n /dev/ttyACM0'.
    Don't forget to restart the launch board!
*/

#include "verify.h"
#include "analog_temp.h"
#include "inc/tm4c123gh6pm.h"

void RunMode(void);
void SleepMode(void);
void PortFInit(void);
void SysTickInit(void);
void Delay2s(void);

int main(void) {
    PortFInit();
    TempSensorInit();
    BESGrader();
    RunMode(); // Comment out if using SleepMode()
    //SleepMode(); // Comment out if using RunMode()
    return 0;
}

/*
    \brief Makes the uC work all the time (CPU usage is 100%).
    \param None
    \note None
*/
void RunMode(void) {
    UARTprintf("Starting Run mode.\n");
    while (true) {
        Delay2s();
        GetTemp();
    }
}

/*
    \brief Exits the main function and executes the WFI assembly instruction to wake up from sleep
    mode only when the interrupt is recognised.
    \param None
    \note None
*/
void SleepMode(void) {
    SysTickInit();
    UARTprintf("Starting Sleep mode.\n");
    __asm("    wfi\n"); // Wait For Interrupt (enter Sleep Mode)
}

/*
    \brief Initialisation of the PF3 and PF1 ports of the Port F.
    \param None
    \return None
*/
void PortFInit(void) {
    volatile uint32_t delay;
    SYSCTL_RCGCGPIO_R |= 0x20;              // 1) Activate clock for Port F
    delay = SYSCTL_RCGCGPIO_R;              // 2) Allow time for clock to start
    while ((SYSCTL_PRGPIO_R & 0x20) == 0) {}; // Ensure Port F is ready
    GPIO_PORTF_LOCK_R = 0x4C4F434B;         // Unlock GPIO Port F
    GPIO_PORTF_CR_R = 0x0A;                 // Allow changes to PF3 and PF1
    GPIO_PORTF_AMSEL_R &= ~0x0A;            // 3) Disable analog on PF3 and PF1
    GPIO_PORTF_PCTL_R &= ~0x0000F0F0;       // 4) PCTL GPIO on PF3 and PF1
    GPIO_PORTF_DIR_R |= 0x0A;               // 5) PF3 and PF1 out
    GPIO_PORTF_AFSEL_R &= ~0x0A;            // 6) Disable alt funct on PF3 and PF1
    GPIO_PORTF_PUR_R &= ~0x0A;              // 7) Disable pull-up on PF3 and PF1
    GPIO_PORTF_DEN_R |= 0x0A;               // 8) Enable digital I/O on PF3 and PF1
}

/* 
    \brief Subroutine initializes SysTick periodic interrupts with
    precision internal oscillator (PIOSC) divided by 4.
    \param None
    \return None
    \note Initialize SysTick interrupts to trigger every 2 s.
*/
void SysTickInit(void) {
    NVIC_ST_CTRL_R = 0;                     // Disable SysTick during setup
    NVIC_ST_RELOAD_R = 8000000 - 1;         // Reload value for 2 seconds (4 MHz clock - 1s=4M operatsiooni / 2s = 8M)
    NVIC_ST_CURRENT_R = 0;                  // Any write to current clears it
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x40000000; // Priority 2
    NVIC_SYS_CTRL_R |= 0x02;                // Bit 1 set SLEEPEXIT
    NVIC_ST_CTRL_R = 0x07;                  // Enable SysTick with core clock and interrupts
}

/*
    \brief Delays approximately 2s.
    \param None
    \return None
    \note Assumes 16 MHz clock
*/
void Delay2s(void) {
    for (volatile uint32_t i = 3200000; i > 0; i--) {}
}

/*
    \brief Interrupt handler which is called every 2s.
    \param None
    \return None
    \note Puts uC into sleep mode after execution.
*/
void SysTickHandler(void) {
    GetTemp();
}