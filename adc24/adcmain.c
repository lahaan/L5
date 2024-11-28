/*
    Lab11 - ADC: Slide potentiometer, screen

    Complete functions in this file and initialization in adc.c.

    To launch the UART serial connection, open the terminal and run:
    'picocom -b 115200 -d 8 -p 1 -y n /dev/ttyACM0'.
    Don't forget to restart the launch board!
*/

#include "verify.h"
#include "i2c_helper.h"
#include "ssd1306.h"
#include "adc.h"

#define NVIC_ST_CTRL_R          (*((volatile uint32_t *)0xE000E010))
#define NVIC_ST_RELOAD_R        (*((volatile uint32_t *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile uint32_t *)0xE000E018))
#define NVIC_SYS_PRI3_R         (*((volatile uint32_t *)0xE000ED20))

// Calibration 
#define A 1
#define B 0

#define DISTANCE_LEN 8

void ConvertDistance(uint32_t num);
void SysTickInit(void);
void SysTickHandler(void);
void ConfigureDisplay(void);
void OLEDprint(const char* unit, const char* str);

char str[DISTANCE_LEN + 1]; // + 1 for '\0'
uint32_t distance;

int main(void) {
    ADC0Init();
    SysTickInit();
    //ConfigureDisplay(); // Comment out if using UART
    BESGrader();
    while (true) {
        ConvertDistance(distance);
        //UARTprintf("Distance ~ %s\n", str); // Comment out if using display
        UARTprintf("ADC Value: %u, Distance: %s\n", distance, str);
        //DisplayClear(); // Comment out if using UART
        //OLEDprint("Distance ~ ", str); // Comment out if using UART
        //DisplayUpdate(); // Comment out if using UART
    }
}

/*
    \brief Converts the decimal number to a formatted distance string.
    4       to "0.004 cm", 
    31      to "0.031 cm",
    102     to "0.102 cm",
    2210    to "2.210 cm",
    10000   to "*.*** cm",
    >9999   to "*.*** cm".
    \param num Decimal numerical value to be converted to a string
    \return None
    \note Remember to set the last character as '\0'!
*/
void ConvertDistance(uint32_t num) {
    if (num > 9999) {
        str[0] = '*';
        str[1] = '.';
        str[2] = '*';
        str[3] = '*';
        str[4] = '*';
        str[5] = ' ';
        str[6] = 'c';
        str[7] = 'm';
        str[8] = '\0';
        return;
    }

    str[7] = 'm';			   //        m
    str[6] = 'c';			   //       cm
    str[5] = ' ';
    str[4] = (num % 10) + '0'; //     x cm
    num /= 10;
    str[3] = (num % 10) + '0'; //    xx cm
    num /= 10;
    str[2] = (num % 10) + '0'; //   xxx cm
    num /= 10;
    str[1] = '.';			   //  .xxx
    str[0] = num + '0';        // x.xxx cm
    str[8] = '\0';             // Null-terminate
}

/* 
    \brief Subroutine initializes SysTick periodic interrupts.
    \param None
    \return None
    \note Initialize SysTick interrupts to trigger at 40 Hz, 25 ms.
*/
void SysTickInit(void) {
    NVIC_ST_CTRL_R = 0;                 // disable SysTick during setup
    NVIC_ST_RELOAD_R = 400000 - 1;      // reload value - 16000000*0.025s=400000
    NVIC_ST_CURRENT_R = 0;              // any write to current clears it
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x40000000; // priority 2 (What priority should be?)        
    NVIC_ST_CTRL_R = 0x07;              // enable SysTick with core clock and interrupts
}

/*
    \brief Collects an 12-bit ADC sample, converts it to 32-bit unsigned
    fixed-point distance (resolution 0.001 cm) and stores in mailbox.
    Calibrate the A and B constants to achieve maximum linearity.
    \param None
    \return None
    \note Handler executes every 25 ms.
*/
void SysTickHandler(void) {
    // Collect a 12-bit 0 to 4095 ADC sample
	// Calibrate to units 0.001 cm
    uint32_t adc_data = ADC0In();
    distance = A * adc_data / 1024 + B;
    UARTprintf("ADC Value: %u\n", adc_data);
}

/*
    \brief Configures the OLED screen display for use.
    PD0 (PUR by PB6) is SCL (Clk pin on screen), and PD1 (PUR by PB7) is SDA (Data pin on screen).
    Connect +3.3V and GND to 3Vo and GND on screen.
    \param None
    \return None
*/
void ConfigureDisplay(void) {
    I2C3Config();
    DisplayInit();
}

/*
    \brief Prints the unit and the string on the OLED screen
    \param unit Unit name, like "Distance ~ " 
    \param str_distance String to be displayed on the screen
    \return None
*/
void OLEDprint(const char* unit, const char* str_distance) {
    SetCursor(0, 0);
    DrawStr(unit);
    DrawStr(str_distance);
}
