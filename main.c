

/**
 * main.c
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "systick.h"
#include "button.h"

#define TIM_1_SEC     200
#define TIM_2_SEC     400
#define TIM_3_SEC     600

#define GREEN 0
#define YELLOW 1
#define RED 2
#define RED_YELLOW 3 // Red and Yellow combined state
#define NORWEGIAN_NIGHT 4 // Norwegian Night state (Yellow blinking)
#define EMERGENCY 5 // Emergency state (Red constant)



extern int ticks;
int alive_timer=TIM_3_SEC;




int main() {

    int currentState = RED;
    uint32_t buttonEvent = 0;
    init_systick();

    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOF;        // enable the GPIO port that is used for the on-board LEDs and switches

    GPIO_PORTF_DIR_R = 0x0E;                    // set the direction as output for LED pins on PortF (PF1 - PF3)
    GPIO_PORTF_DEN_R = 0x1E;                    // enable the GPIO pins for digital function (PF1 - PF4)
    GPIO_PORTF_PUR_R = 0x10;

    while (1) {

        while( !ticks );

        // The following will be executed every 5ms
        ticks--;

        // Update button state
        buttonEvent = select_button();
        currentState = traffic_light(buttonEvent, currentState);

    }

    return 0;
}

int traffic_light(uint32_t button_event, int current_state){
    switch (button_event) {
                case 1: // Single press
                    GPIO_PORTF_DATA_R &= 0x00;
                    current_state = NORWEGIAN_NIGHT; // Change traffic light state
                    alive_timer=TIM_1_SEC;
                    break;
                case 2: // Double press
                    GPIO_PORTF_DATA_R &= 0x00;
                    current_state = EMERGENCY; // Change traffic light state
                    break;
                case 3: // Long press
                    GPIO_PORTF_DATA_R &= 0x00;
                    current_state = RED; // Return to normal operation
                    alive_timer=TIM_3_SEC;
                    break;
                default:
                    // No button event or unknown event
                    break;
            }
    switch (current_state) {
                case GREEN:

                    GPIO_PORTF_DATA_R |=0x06;
                    if( ! --alive_timer )
                    {
                        GPIO_PORTF_DATA_R &= ~(0x06);
                        alive_timer=TIM_2_SEC;
                        current_state = YELLOW;
                    }

                    break;
                case YELLOW:

                    GPIO_PORTF_DATA_R |=0x0A;
                    if( ! --alive_timer )
                    {
                        GPIO_PORTF_DATA_R &= ~(0x0A);
                        alive_timer=TIM_3_SEC;
                        current_state = RED;
                    }

                    break;
                case RED:

                    GPIO_PORTF_DATA_R |=0x0C;
                    if( ! --alive_timer )
                    {
                        GPIO_PORTF_DATA_R &= ~(0x0C);
                        alive_timer=TIM_2_SEC;
                        current_state = RED_YELLOW;
                    }

                    break;
                case RED_YELLOW:

                    GPIO_PORTF_DATA_R |=0x08;
                    if( ! --alive_timer )
                    {
                        GPIO_PORTF_DATA_R &= ~(0x08);
                        alive_timer=TIM_3_SEC;
                        current_state = GREEN;
                    }

                    break;
                case NORWEGIAN_NIGHT:

                    if( ! --alive_timer )
                    {
                        GPIO_PORTF_DATA_R |= 0x0A;
                        int a;
                        for(a = 0; a <1000; a++){}
                        alive_timer=TIM_1_SEC;
                        GPIO_PORTF_DATA_R &= ~(0x0A);
                    }

                    break;
                case EMERGENCY:
                    GPIO_PORTF_DATA_R |=0x0C;

                    break;
            }

    return current_state;
}



