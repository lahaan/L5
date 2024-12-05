#ifndef VERIFY_H
#define VERIFY_H

#include <stdint.h>
#include <stdbool.h>

/*
    Activates the grader for the student. Sets up the interrupts
    and configures UART. Verifies the port initialisation (once).
    Do not remove this function from where it is placed!
*/
void BESGrader(void);

#endif /* VERIFY_h */