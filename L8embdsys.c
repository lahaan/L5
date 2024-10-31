/*
    Lab8 - FSM

    Complete port initializations and main code with FSM in the main.c file.
    Navigate to fsm.h to add states.

    To launch the UART serial connection, open the terminal and run:
    'picocom -b 115200 -d 8 -p 1 -y n /dev/ttyACM0'.
    Don't forget to restart the launch board!
*/

#include "verify.h"
#include "fsm.h"

#define SYSCTL_RCGC2_R          (*((volatile uint32_t *)0x400FE108))

#define GPIO_PORTF_DATA_R       (*((volatile uint32_t *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile uint32_t *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile uint32_t *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile uint32_t *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile uint32_t *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile uint32_t *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile uint32_t *)0x4002552C))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // Port F Clock Gating Control
	
#define GPIO_PORTB_DATA_R       (*((volatile uint32_t *)0x400053FC))
#define GPIO_PORTB_DIR_R        (*((volatile uint32_t *)0x40005400))
#define GPIO_PORTB_AFSEL_R      (*((volatile uint32_t *)0x40005420))
#define GPIO_PORTB_DEN_R        (*((volatile uint32_t *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile uint32_t *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile uint32_t *)0x4000552C))
#define SYSCTL_RCGC2_GPIOB      0x00000002  // Port B Clock Gating Control
	
#define GPIO_PORTE_DATA_R       (*((volatile uint32_t *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile uint32_t *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile uint32_t *)0x40024420))
#define GPIO_PORTE_DEN_R        (*((volatile uint32_t *)0x4002451C))
#define GPIO_PORTE_AMSEL_R      (*((volatile uint32_t *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile uint32_t *)0x4002452C))
#define SYSCTL_RCGC2_GPIOE      0x00000010  // Port E Clock Gating Control
	
#define NVIC_ST_CTRL_R          (*((volatile uint32_t *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile uint32_t *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile uint32_t *)0xE000E018))
#define NVIC_ST_CTRL_COUNT      0x00010000  // Count Flag

void SysTickWait100ms(uint32_t times);
void SysTickInit(void);
void PortBInit(void);
void PortEInit(void);
void PortFInit(void);

const FSMStateData_t fsm[Count] = {
    {0x04, 0x01, 10, {CarGreen, CarYellow, CarGreen, CarYellow}}, // CarGreen state
    {0x02, 0x01, 2, {CarRed, CarRed, CarRed, CarRed}}, // CarYellow state
    {0x01, 0x08, 10, {CarRed, CarRed, CarGreen, CarGreen}}, // CarRed state
    {0x01, 0x08, 10, {PedHurry, PedHurry, PedHurry, PedHurry}}, // PedWalk state
    {0x01, 0x02, 2, {PedDontWalk, PedDontWalk, PedDontWalk, PedDontWalk}}, // PedHurry state
    {0x01, 0x01, 10, {CarGreen, CarGreen, CarGreen, CarGreen}} // PedDontWalk state
};

int main(void) {
	PortBInit(); //PINS FOR 3 LED OUTPUTS (FOR CARS); PB0 - RED,  PB1 - YELLOW, PB2 - GREEN
	PortEInit(); //PINS FOR 2 SW INPUTS (2 SWITCHES); PF0 - CAR "SENSOR", PF1 PEDESTRIAN "SENSOR" 
	PortFInit(); // PINS FOR 2 LED OUTPUTS [internal] (FOR PEDESTRIANS) - PF1 - RED : PF3 - GREEN
    SysTickInit();
	BESGrader();
    uint32_t input;           // input combination from sensors on PE0-1
    FSMState_t state; // current state index
    while (true) {
        GPIO_PORTB_DATA_R = fsm[state].portb_out; //car light output
        GPIO_PORTF_DATA_R = fsm[state].portf_out; //pedestrian light output
        SysTickWait100ms(fsm[state].times); //wait
        input = GPIO_PORTE_DATA_R & 0x03; //inputs
        state = fsm[state].next[input]; //next state
    }
}

/* 
    \brief Subroutine to delay for a specifiad amount of time.

    \param delay  
    \return The delay parameter is in units of the 16 MHz core clock (62.5 ns).
*/
void SysTickWait(uint32_t delay) {
    NVIC_ST_RELOAD_R = delay - 1;                           // number of counts to wait
    NVIC_ST_CURRENT_R = 0;                                  // any value written to CURRENT clears it
    while ((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) == 0) {}   // wait for count flag
}

/* 
    \brief Subroutine to delay exactly 100ms X times.

    \param times How many times to delay 100ms
    \return None
	\note Use this function for delays in the main program.
*/
void SysTickWait100ms(uint32_t times) {
    for (uint32_t i = 0; i < times; i++) {
        SysTickWait(1600000); // 16 MHz - 1s, ? - 100ms (maybe 161000)
    }
}

/*
    \brief Subroutine to initialize SysTick. 
    \param None
    \return None
    \note Runs at 16 MHz. 
*/
void SysTickInit(void) {
    NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
    NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}

/* 
    \brief Subroutine to initialize port B pins for output.
    PB0 is car red external LED.
    PB1 is car yellow external LED.
	PB2 is car green external LED.

    \param None
    \return None
*/
void PortBInit(void) { // PINS FOR 3 LED OUTPUTS (FOR CARS) - PB0 - RED : PB1 - YELLOW : PB2 - GREEN
    volatile uint32_t delay;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;  // 1) Activate clock for Port B
    delay = SYSCTL_RCGC2_R;  // Allow time for clock to start
    GPIO_PORTB_AMSEL_R = 0x00;  // 2) Disable analog functionality on PB0, PB1, PB2
    GPIO_PORTB_PCTL_R = 0x00000000;  // 3) Configure PB0, PB1, PB2 as GPIO
    GPIO_PORTB_DIR_R |= 0x07;  // 4) Set PB0, PB1, PB2 as outputs
    GPIO_PORTB_AFSEL_R = 0x00;  // 5) Disable alternate functions on PB0, PB1, PB2
    GPIO_PORTB_DEN_R |= 0x07;  // 6) Enable digital I/O on PB0, PB1, PB2
}

void PortEInit(void) { // PINS FOR 2 SW INPUTS (2 SWITCHES)
    volatile uint32_t delay;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;  // 1) Activate clock for Port E
    delay = SYSCTL_RCGC2_R;  // Allow time for clock to start
    GPIO_PORTE_AMSEL_R = 0x00;  // 2) Disable analog functionality on PE0, PE1
    GPIO_PORTE_PCTL_R = 0x00000000;  // 3) Configure PE0, PE1 as GPIO
    GPIO_PORTE_DIR_R &= ~0x03;  // 4) Set PE0, PE1 as inputs
    GPIO_PORTE_AFSEL_R = 0x00;  // 5) Disable alternate functions on PE0, PE1
    GPIO_PORTE_DEN_R |= 0x03;  // 6) Enable digital I/O on PE0, PE1
}

void PortFInit(void) { // PINS FOR 2 LED OUTPUTS [internal] (FOR PEDESTRIANS) - PF1 - RED : PF3 - GREEN
    volatile uint32_t delay;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;  // 1) Activate clock for Port F
    delay = SYSCTL_RCGC2_R;  // Allow time for clock to start
    GPIO_PORTF_AMSEL_R = 0x00;  // 2) Disable analog functionality on PF1, PF3
    GPIO_PORTF_PCTL_R = 0x00000000;  // 3) Configure PF1, PF3 as GPIO
    GPIO_PORTF_DIR_R |= 0x0A;  // 4) Set PF1, PF3 as outputs
    GPIO_PORTF_AFSEL_R = 0x00;  // 5) Disable alternate functions on PF1, PF3
    GPIO_PORTF_DEN_R |= 0x0A;  // 6) Enable digital I/O on PF1, PF3
}

/*
    do we need to enable PUR? only F is defined 
    E - 
    #define GPIO_PORTE_PUR_R        (*((volatile uint32_t *)0x40024510))
    B - 
    ???
    GPIO_PORT?_PUR_R |= 0x??
*/
