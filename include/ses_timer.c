/* INCLUDES ******************************************************************/
#include "ses_timer.h"

/* DEFINES & MACROS **********************************************************/
#define TIMER1_CYC_FOR_5MILLISEC         155
#define TIMER2_CYC_FOR_1MILLISEC	     124

//Timer 2
#define OCIE2A_BIT                         1
#define OCF2A_BIT                          1
#define WGM21_BIT                          1
#define CS2_BIT                            2

//Timer 1
#define OCIE1A_BIT                         1
#define OCF1A_BIT                          1
#define CS1_BIT                            2
#define WGM12_BIT                          3

pTimerCallback timer2_ptr = NULL;
pTimerCallback timer1_ptr = NULL;

/*FUNCTION DEFINITION ********************************************************/
void timer2_setCallback(pTimerCallback cb) {
	timer2_ptr = cb;
}

void timer2_start() {
	    /*  Use CTC mode
	     *  WGM 22:20
	     *  So WGM21 = 1 as 010 = 0x2 is CTC mode
	     */

		TCCR2A |=  (1 << WGM21_BIT);     //0x02

		//Set a prescaler of 64
		TCCR2B |= (1 << CS2_BIT);      //0x04

		/* TIMSK2 : Timer 2 interrupt mask register
		 * //TODO: Set interrupt mask register for Compare A
		 *   OCIE2A bit: Timer 2 output compare match A interrupt enable
		 *   OCIE2A bit --> 1
		 */
		TIMSK2 |= (1 << OCIE2A_BIT);

		/* Clear interrupt flag
		 * TIFR2 : Timer 2 interrupt flag register
		 * OCF2A bit : Output compare flag 2 A
		 *         1 : interrupt flag cleared
		 */
		TIFR2 |= (1 << OCF2A_BIT);

		/*  Formula on page 316 --> OCRnx => OCR2A
		 * 	OCR2A = 124 in decimals for a period of 1ms
		 */
		OCR2A &= TIMER2_CYC_FOR_1MILLISEC;                   //0x7C
}


void timer2_stop() {
	   // stop timer: write 0  to the CS22 bit in TCCR2B
       TCCR2B &= (0 << CS2_BIT);
}

void timer1_setCallback(pTimerCallback cb) {
	timer1_ptr = cb;
}


void timer1_start() {
	/*  Enable CTC mode
	 *  WGM13:10
	 *  So WGM12 = 1 as 0100 = 0x04 => CTC mode
	 *  WGM12 is bit 3 in TCCR1B
	 */
	 TCCR1B |= (1 << WGM12_BIT);

	// Set prescaler of 256 to obtain a vaild OCRnA value
	 TCCR1B |= (1 << CS1_BIT);

	/* TIMSK1 : Timer 1 interrupt mask register
	 * //TODO: Set interrupt mask register for Compare A
	 *   OCIE1A bit: Timer 1 output compare match A interrupt enable
	 *   OCIE1A bit --> 1
	 */
	  TIMSK1 |= (1 << OCIE1A_BIT);

	 /* Clear interrupt flag
	  * TIFR1 : Timer 1 interrupt flag register
	  * OCF1A bit : Output compare flag 1 A
	  *         1 : interrupt flag cleared
	  */
	  TIFR1 |= (1 << OCF1A_BIT);

	 /*  OCR1A
	  *  OCR1A = 155 in decimals for a period of 5ms
	  */
	  OCR1AL &= TIMER1_CYC_FOR_5MILLISEC;

}


void timer1_stop() {
	//Stop timer1
	TCCR1B &= (0 << CS1_BIT);
}

ISR(TIMER1_COMPA_vect) {
	if(timer1_ptr != NULL)
		timer1_ptr();
}

ISR(TIMER2_COMPA_vect) {

	if( timer2_ptr != NULL)
		timer2_ptr();

}
