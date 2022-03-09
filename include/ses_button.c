/* INCLUDES ******************************************************************/
#include "ses_timer.h"

#include "ses_common.h"
#include "ses_button.h"
#include "stdint.h"
#include "avr/io.h"

/* DEFINES & MACROS **********************************************************/

// LED wiring on SES board
#define BUTTON_ROTARY_PORT			PORTB
#define BUTTON_ROTARY_PIN				6

#define BUTTON_JOYSTICK_PORT		PORTB
#define BUTTON_JOYSTICK_PIN  	  		7

//Pin change interrupt enable 0 in PCICR register
#define PCIE0_BIT                       0

//Two global variables for the callback pointers
pButtonCallback rotary_ptr = NULL;
pButtonCallback joystick_ptr = NULL;

//Exercise 3.3
#define BUTTON_NUM_DEBOUNCE_CHECKS       5


void button_init( bool debouncing) {
//  Initialize DDR register and button ports
	DDR_REGISTER(BUTTON_ROTARY_PORT) &= ~(1 << BUTTON_ROTARY_PIN);
	DDR_REGISTER(BUTTON_JOYSTICK_PORT) &= ~(1 << BUTTON_JOYSTICK_PIN);

	BUTTON_ROTARY_PORT |= (1 << BUTTON_ROTARY_PIN);
	BUTTON_JOYSTICK_PORT |= (1 << BUTTON_JOYSTICK_PIN);

/*  It is better to write:
    BUTTON_ROTARY_PORT |= (1 << BUTTON_ROTARY_PIN);
	BUTTON_JOYSTICK_PORT |= (1 << BUTTON_JOYSTICK_PIN);

	This would apply when writing any hexadecimal address into the codes
	I.E. would need to be changed in ses_led.c and others
	Asked lab TA's which is preferred, and why? said shifting because:
	1. easier to read among group working on a project
	2. if (which shouldn't happen in this lab) the pins are switched
	   it does not take nearly as much work to make changes
 */

	// initialization for both techniques (e.g. setting up the DDR register)
	// Done in lines 31 and 32
	if(debouncing) {
	// TODO initialization for debouncing
	timer1_setCallback(button_checkState());
	}
	else {
	// initialization for direct interrupts (e.g. setting up the PCICR register)
	// Done in main.c, exercise 3 after it was recommended by TA to initialize
	// the interrupt in the main.c.
	// A LITTLE BIT CONFUSED NOW.
	}
}

ISR(PCINT0_vect){
		cli();  // START: Clears the global interrupt flag in SREG so prevent any form of interrupt occurring.
		//TODO Insert ISR here
        while(button_isJoystickPressed() || button_isRotaryPressed()){

        	if(button_isJoystickPressed() && PCMSK0 == (1 << BUTTON_JOYSTICK_PIN) && joystick_ptr != NULL){
        		(joystick_ptr)();
        	}
        	if(button_isRotaryPressed() && PCMSK0 == (1 << BUTTON_ROTARY_PIN) && rotary_ptr != NULL){
        		(rotary_ptr)();
        	}
        }

		sei(); // END: Globally enabling Interrupts
}

bool button_isJoystickPressed(void) {
//  Get the state of the joystick button.
	return ((PIN_REGISTER(BUTTON_JOYSTICK_PORT) & (1 << BUTTON_JOYSTICK_PIN)) != (1 << BUTTON_JOYSTICK_PIN));
}

bool button_isRotaryPressed(void) {
//  Get the state of the rotary button.
	return ((PIN_REGISTER(BUTTON_ROTARY_PORT) & (1 << BUTTON_ROTARY_PIN)) != (1 << BUTTON_ROTARY_PIN));
}


void button_setRotaryButtonCallback(pButtonCallback callback){
	rotary_ptr = callback;
}
void button_setJoystickButtonCallback(pButtonCallback callback){
	joystick_ptr = callback;
}



void button_checkState() {
	static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = {};
	static uint8_t index = 0;
	static uint8_t debouncedState = 0;
	uint8_t lastDebouncedState = debouncedState;
	// each bit in every state byte represents one button
	state[index] = 0;
	if(button_isJoystickPressed()) {
		state[index] |= 1;
	}
	if(button_isRotaryPressed()) {
		state[index] |= 2;
	}
	index++;
	if (index == BUTTON_NUM_DEBOUNCE_CHECKS) {
		index = 0;
	}
	// init compare value and compare with ALL reads, only if
	// we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "1" in the state
	// array, the button at this position is considered pressed
	uint8_t j = 0xFF;
	for(uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++) {
		j = j & state[i];
	}
	debouncedState = j;

	// TODO extend function
	if(debouncedState == 1){
		rotary_ptr();
	}
	if(debouncedState == 2){
		joystick_ptr();
	}

	lastDebouncedState = debouncedState;
}
