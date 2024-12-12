#include "game_logic.h"
#include "i2c_helper.h"
#include "ssd1306.h"
#include "inc/tm4c123gh6pm.h"
#include "utils/ustdlib.h"

#define GAME_SEED 212958 // You can enter your student code
#define DELAY 500
#define PF0 0x01
#define PF3 0x08
#define PF4 0x10

typedef struct {
    uint8_t column;
    uint8_t page;
    uint8_t speed;
} Ball;


void Debounce(void) {
    for (volatile uint32_t i = DELAY; i > 0; i--) {}
}

uint8_t RandInRange(uint8_t max, uint8_t min) {
    int rand = urand();
    rand *= rand < 0 ? -1 : 1;
    rand = rand % (max + 1 - min) + min;
    return rand;
}

void SysTickHandler(void) {
    GPIO_PORTF_DATA_R ^= PF3; // toggle LED + buzzer 
}

/*void Delay(uint32_t milliseconds) {
    uint32_t cycles_per_ms = 16000; // 1ms
    uint32_t total_cycles = milliseconds * cycles_per_ms;
    for (volatile uint32_t i = 0; i < total_cycles; i++) {}
}*/

void GameLoop(void) {
    usrand(GAME_SEED);
    Ball balls[2];
    uint8_t basket_position = 3; // initial position
    uint8_t score = 0;           // score dec
    const uint8_t MAX_SCORE = 99;
    bool button_pressed = false; // button state boolean

    for (int i = 0; i < 2; i++) {              // generating struct for 2 balls
        balls[i].column = 0;                   // ball horizontal between 0-100px (x)
        balls[i].page = RandInRange(7, 0);     // ball vertical spawn between pages 0-7 (y(0;63))
        balls[i].speed = RandInRange(5, 1);    // ball speed between 1-5px/tick
    }

    while (true) {
        DisplayClear();

        SetCursor(98, basket_position);             // drawing the basket "]"
        DrawChar(']');

        DrawVerticalLine(100, 0, 63);               // vertical "ground" line at x=100 (101-st pixel on x-axis)

        for (int i = 0; i < 2; i++) {               // drawing balls & updating positions + caught & score logic
            SetCursor(balls[i].column, balls[i].page); // drawing the ball
            DrawChar('o');
            balls[i].column += balls[i].speed;      // changing x/column position 

            if (balls[i].column >= 100) {           // ball reaches vertical line (not caught)
                balls[i].column = 0;                // respawn at 0
            }

            if (balls[i].page == basket_position && balls[i].column == 98) { // collision
                if (score < MAX_SCORE) {
                    score++;
                } 
                balls[i].page = RandInRange(7, 0);
                balls[i].column = 0; // Respawn at column 0
                balls[i].speed = RandInRange(5, 1);
                GPIO_PORTF_DATA_R |= PF3;           // buzzer + LED
                //Delay(100);
                Debounce(); //delay is fuccy
                GPIO_PORTF_DATA_R &= ~PF3;          // turn off buzzer + LED
            }
        }

        if (!(GPIO_PORTF_DATA_R & PF4) && !button_pressed) { // move basket up if SW2 pressed
            if (basket_position > 0) {
                basket_position--;
            }
            button_pressed = true;
        } 
        else if (!(GPIO_PORTF_DATA_R & PF0) && !button_pressed) { // move basket down if SW1 pressed
            if (basket_position < 7) {
                basket_position++;
            }
            button_pressed = true; // Mark button press as registered
        } 
        else if ((GPIO_PORTF_DATA_R & PF4) && (GPIO_PORTF_DATA_R & PF0)) {  // no input?? + reset state
            button_pressed = false;
        }

        char score_str[4];
        sprintf(score_str, "%u", score);    // conversion
        SetCursor(120, 0);                  // top right
        DrawStr(score_str);                 // draw score

        DisplayUpdate();
    }
}



/*
void HandleBasketMovement(void) {
    static bool button_pressed = false; // Static to retain value between function calls

    // Read the current state of the buttons
    bool sw1_pressed = !(GPIO_PORTF_DATA_R & PF4); // SW1 (PF4) pressed
    bool sw2_pressed = !(GPIO_PORTF_DATA_R & PF0); // SW2 (PF0) pressed

    if (sw1_pressed && !button_pressed) { // Move basket up if SW1 pressed
        if (basket_position > 0) {
            basket_position--;
        }
        button_pressed = true; // Mark button press as registered
    } 
    else if (sw2_pressed && !button_pressed) { // Move basket down if SW2 pressed
        if (basket_position < 7) {
            basket_position++;
        }
        button_pressed = true; // Mark button press as registered
    } 
    else if (!sw1_pressed && !sw2_pressed) { // No buttons pressed
        button_pressed = false; // Reset button press registration
    }
}
*/