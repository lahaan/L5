#ifndef FSM_H
#define FSM_H

#include <stdint.h>

#define NEXT_SIZE 4 // The amount of possible next states

typedef struct FSMStateData {
    uint32_t portb_out;         // PB0-2 car lights
    uint32_t portf_out;         // PF3 and PF1 pedestrian lights
    uint32_t times;             // X times 100ms delay
    uint32_t next[NEXT_SIZE];   // Array of next possible states
} FSMStateData_t;

// State must be 1:1 with the same state index in fsm array
typedef enum FSMState {
    CarGreen,
    CarYellow,
    CarRed,
    PedWalk,
    PedHurry,
    PedDontWalk,
    Count
} FSMState_t;

#endif /* FSM_H */