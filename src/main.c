#include "ses_led.h"
#include "ses_lcd.h"
#include "ses_button.h"
#include "ses_button.c"
#include "ses_timer.h"
#include "ses_timer.c"
#include "util/delay.h"
#include "avr/io.h"
#include "ses_common.h"
#include "ses_scheduler.h"
#include "ses_uart.h"
#include "util/atomic.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/* DEFINES & MACROS **********************************************************/



// Flag for joystick button press
static int joy_press_count = 1;
static int rotary_press_count = 1;

void leds_init() {
	led_greenInit();
	led_redInit();
	led_yellowInit();
	led_redOff();
	led_greenOff();
	led_yellowOff();
}




enum{
	ENTRY,
	EXIT
};

/* dispatches events to state machine, called in application*/
inline static void fsm_dispatch(Fsm *fsm, const Event *event) {
	static Event entryEvent = {.signal = ENTRY};
	static Event exitEvent = {.signal = EXIT};
	State s = fsm->state;
	fsmReturnStatus r = fsm->state(fsm, event);
	if (r==RET_TRANSITION) {
		s(fsm, &exitEvent); //< call exit action of last state
		fsm->state(fsm, &entryEvent); //< call entry action of new state
		}
}

/* sets and calls initial state of state machine */
inline static void fsm_init(Fsm *fsm, State init) {
	//... other initialization
	Event entryEvent = {.signal = ENTRY};
	fsm->state = init;
	fsm->state(fsm, &entryEvent);
}

#define TRANSITION(newState) (fsm->state = newState, RET_TRANSITION)
static void joystickPressedDispatch(void * param) {
	Event e = {.signal = JOYSTICK_PRESSED};
	fsm_dispatch(&theFsm, &e);
}

fsmReturnStatus running(Fsm * fsm, const Event* event) {
	switch(event->signal) {
		//... handling of other events
		case JOYSTICK_PRESSED:
			return TRANSITION(setHourAlarm);
		default:
			return RET_IGNORED;
	}
}


int main(void) {

	//Initialization functions
	uart_init(57600);
	lcd_init();
	button_init(true);
	leds_init();
	scheduler_init();


	// Set callbacks

	// Initialize scheduler tasks

	// Add tasks to the scheduler

	while (1) {
		scheduler_run();
	}

	return 0;
}
