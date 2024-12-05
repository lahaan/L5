#ifndef ANALOG_TEMP_H
#define ANALOG_TEMP_H

#include <stdint.h>
#include "utils/uartstdio.h"

/*
    \brief Initializes the Port E pin 2 for ADC0 as a temperature sensor.
    \param None
    \return None
    \note Initialize Port E and ADC0_SSCTL3_R correctly accroding to the comments.
*/
void TempSensorInit(void);

/*
    \brief Collects the ADC sample, converts it to Celsius temperature and formats and prints
    it through UART.
    \param None
    \return None
*/
void GetTemp(void);

#endif /* ANALOG_TEMP_H */
