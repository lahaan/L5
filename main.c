//L6
#include "verify.h"
#include "bits.h"

#define ARR_SIZE 50

uint32_t time[ARR_SIZE] = {0}; 
uint32_t data[ARR_SIZE] = {0}; 
uint32_t i = 0;

uint32_t before;               // GPIO_PORTF_DATA_R value before the change
uint32_t last;                 // Last recorded time

void PortFInit(void);
void SysTickInit(void);
void RecordTimeAndData(void);
void DetectInputChange(void);
void Delay50ms(void);

int main(void) {
    PortFInit();
    SysTickInit();
    BESGrader();
    last = NVIC_ST_CURRENT_R;
    while (true) {
        before = GPIO_PORTF_DATA_R;
        DetectInputChange();
        if (ToggledBits(GPIO_PORTF_DATA_R, before) & 0x02) { // Check if PF1 (LED) has change
            RecordTimeAndData(); // Record data if output changes
        }
    }
}

void PortFInit(void) {
    volatile uint32_t delay;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;   // Activate clock for Port F
    delay = SYSCTL_RCGC2_R;                 // Allow time for clock to start
    GPIO_PORTF_LOCK_R = 0x4C4F434B;         // Unlock PF0
    GPIO_PORTF_CR_R = 0x1F;                 // Allow changes to PF4-0
    GPIO_PORTF_AMSEL_R = 0x00;              // Disable analog on PortF
    GPIO_PORTF_PCTL_R = 0x00000000;         // PCTL GPIO on PF4-0
    GPIO_PORTF_DIR_R |= 0x0E;               // PF4, PF0 in, PF1 out
    GPIO_PORTF_AFSEL_R = 0x00;              // Disable alt funct on PF7-0
    GPIO_PORTF_PUR_R |= 0x11;               // Enable pull-up on PF0 and PF4
    GPIO_PORTF_DEN_R |= 0x1F;               // Enable digital I/O on PF4-0      
}

void SysTickInit(void) {
    NVIC_ST_CTRL_R = 0;                   // Disable SysTick during setup
    NVIC_ST_RELOAD_R = 0x00FFFFFF;        // Maximum reload value
    NVIC_ST_CURRENT_R = 0;                // Any write to current clears it             
    NVIC_ST_CTRL_R = 0x00000005;          // Enable SysTick with core clock
}

void RecordTimeAndData(void) {
    if (i < ARR_SIZE && ToggledBits(GPIO_PORTF_DATA_R, before)) {
        uint32_t now = NVIC_ST_CURRENT_R;      // get current clock cycles
        time[i] = (last - now) & 0x00FFFFFF;   // 24-bit time difference (gives values in ms)
        data[i] = GPIO_PORTF_DATA_R & 0x13;    // record PF0, PF1 and PF4
        last = now;
        i++;
    }
}

void DetectInputChange(void) {
    uint32_t current = GPIO_PORFT_DATA_R & 0x11;
    if (current != 0x11){
        GPIO_PORTF_DATA_R ^= 0x02; // Toggle PF1 (LED)
        RecordTimeAndData(); // Record data when input changes
        Delay50ms();
    }
}


#define DELAY_50MS 80500 // ~50ms

void Delay50ms(void) {
    for (volatile uint32_t i = DELAY_50MS; i > 0; i--) {}
}

//L7
/*
    Lab7 - Interrupts and buzzer

    Finish the SysTickHandler() function.

    To launch the UART serial connection, open the terminal and run:
    'picocom -b 115200 -d 8 -p 1 -y n /dev/ttyACM0'.
    Don't forget to restart the launch board!
*/

#include "verify.h"

#define SYSCTL_RCGC2_R          (*((volatile uint32_t *)0x400FE108))

#define GPIO_PORTA_AMSEL_R      (*((volatile uint32_t *)0x40004528))
#define GPIO_PORTA_PCTL_R       (*((volatile uint32_t *)0x4000452C))
#define GPIO_PORTA_DIR_R        (*((volatile uint32_t *)0x40004400))
#define GPIO_PORTA_DR8R_R       (*((volatile uint32_t *)0x40004508))
#define GPIO_PORTA_AFSEL_R      (*((volatile uint32_t *)0x40004420))
#define GPIO_PORTA_DEN_R        (*((volatile uint32_t *)0x4000451C))
#define GPIO_PORTA_DATA_R       (*((volatile uint32_t *)0x400043FC))
#define SYSCTL_RCGC2_GPIOA      0x00000001  // Port A Clock Gating Control

#define NVIC_ST_CTRL_R          (*((volatile uint32_t *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile uint32_t *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile uint32_t *)0xE000E018))
#define NVIC_SYS_PRI3_R         (*((volatile uint32_t *)0xE000ED20))

void PortAInit(void);
void SysTickInit(void);
void SysTickHandler(void);

// Should remain untouched!
int main(void) {
    PortAInit();
    SysTickInit();
    BESGrader();
    while (true) {}
}

/* 
    \brief Subroutine to initialize port A pins for input and output.
    PA3 is external switch input.
    PA2 is output to the buzzer.

    \param None
    \return None
    \note No analog, regular function, no alt function, digital enabled, 8-mA drive select for output
*/
void PortAInit(void) {
    volatile uint32_t delay;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;   // Activate clock for Port A
    delay = SYSCTL_RCGC2_R;                 // Allow time for clock to start
    GPIO_PORTA_AMSEL_R = 0x00;              // Disable analog on Port A
    GPIO_PORTA_PCTL_R = 0x00000000;         // PCTL GPIO on PA3-0
    GPIO_PORTA_DIR_R |= 0x04;               // PA2 out, PA3 in
    GPIO_PORTA_AFSEL_R = 0x00;              // Disable alt funct on PA7-0
    GPIO_PORTA_DR8R_R |= 0x04;              // Enable 8-mA drive on PA2
    GPIO_PORTA_DEN_R |= 0x0C;               // Enable digital I/O on PA3-2
}

void SysTickInit(void) {
    NVIC_ST_CTRL_R = 0;           // disable SysTick during setup
    NVIC_ST_RELOAD_R = 18181 - 1;     // reload value for 1.13636ms (assuming 16MHz) [16MHz = 16,000,000 -> (16,000,000/880) - 1 = 18181 umbes]
    NVIC_ST_CURRENT_R = 0;        // any write to current clears it
    NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R & 0x00FFFFFF; // priority 0               
    NVIC_ST_CTRL_R = 0x00000007;  // enable with core clock and interrupts
}

/*
    \brief Subroutine to handle the SysTick interrupt. When switch is pressed, the buzzer
    is emmiting a tone at 440 Hz. When the switch is pressed again, the buzzer turns off.
    \param None
    \return None
    \note Interrupt at 1/880Hz = 1.13636ms. 
    When the switch is held down, the buzzer tone is toggled on/off once!
*/
void SysTickHandler(void) {
    static uint8_t buzzer_on = 0; // State of the buzzer
    static uint8_t switch_pressed = 0; // State of the switch

    if ((GPIO_PORTA_DATA_R & 0x08) != 0) { // Check if PA3 (switch) is pressed (positive logic)
        if (!switch_pressed) { // If switch was not previously pressed
            buzzer_on ^= 1; // Toggle buzzer state
            switch_pressed = 1; // Mark switch as pressed
        }
    } else {
        switch_pressed = 0; // Mark switch as not pressed
    }

    if (buzzer_on) {
        GPIO_PORTA_DATA_R ^= 0x04; // buzzer on
    } 
    else {
        GPIO_PORTA_DATA_R &= ~0x04; // buzzer off
    }
}

/*
Buzzer typPower = 0.1W & impedance = 32ohm +-20%
approximately 10-75ohm should be in spec giving 0.2W-0.1W respectively
*/


