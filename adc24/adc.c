#include "adc.h"
#include "inc/tm4c123gh6pm.h"

void ADC0Init(void) { 
    SYSCTL_RCGCADC_R |= 0x0001;             // 1) Activate ADC0
    SYSCTL_RCGCPIO_R |= 0x10;               // 2) Activate clock for Port E
    while (!(SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOE)) {} // 3) Wait until Port E clock ready
    GPIO_PORTE_DIR_R &= ~0x04;              // 4) Make PE2 input
    GPIO_PORTE_AFSEL_R |= 0x04;             // 5) Enable alternate function on PE2
    GPIO_PORTE_DEN_R &= ~0x04;              // 6) Disable digital I/O on PE2
    GPIO_PORTE_AMSEL_R |= 0x04;             // 7) Enable analog functionality on PE2
    while (!(SYSCTL_PRADC_R & 0x0001)) {}   // 8) Wait until ADC0 ready for access 
    ADC0_PC_R &= ~0xF;
    ADC0_PC_R |= 0x1;                       // 9) Configure for 125K samples/sec
    ADC0_SSPRI_R = 0x0123;                  // 10) Sequencer 3 is highest priority
    ADC0_ACTSS_R &= ~0x0008;                // 11) Disable sample sequencer 3
    ADC0_EMUX_R &= ~0xF000;                 // 12) Seq3 is software trigger
    ADC0_SSMUX3_R &= ~0x000F;
    ADC0_SSMUX3_R += 1;                     // 13) Set channel 1
    ADC0_SSCTL3_R = 0x0006;                 // 14) No TS0 D0, yes IE0 END0
    ADC0_IM_R &= ~0x0008;                   // 15) Disable SS3 interrupts
    ADC0_ACTSS_R |= 0x0008;                 // 16) Enable sample sequencer 3
}

uint32_t ADC0In(void) { 
    ADC0_PSSI_R = 0x0008;                       // 1) Initiate SS3
    while (!(ADC0_RIS_R & 0x08)) {}             // 2) Wait for conversion done
    uint32_t result = ADC0_SSFIFO3_R & 0xFFF;   // 3) Read result
    ADC0_ISC_R = 0x0008;                        // 4) Acknowledge completion
    return result;
}
