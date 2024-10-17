
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
