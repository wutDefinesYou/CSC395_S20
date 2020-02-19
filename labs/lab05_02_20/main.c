#include "common.h"

#include <avr/io.h>
#include <util/delay.h>

#include "leds.h"
#include "buttons.h"

#define INVERTED 1

// for use of button action setup
#define RELEASE 1
#define PRESS 0

// define a clock to keep up with the system time
volatile uint32_t ms_ticks;

/****************************************************************************
   ALL INITIALIZATION
****************************************************************************/
void initialize_system(void) {
	// initalize green and yellow only.
	// initialization defines the IO_structs and sets DDR
	initialize_led(GREEN);
	initialize_led(YELLOW);

	GPIO_initialize_led(GPIO_RED);
	GPIO_initialize_led(GPIO_GREEN);
	GPIO_initialize_led(GPIO_YELLOW);

	// The "sanity check".
	// When you see this pattern of lights you know the board has reset
	light_show();

	// initalize button A
	initialize_button(&_buttonA);
	// initialize_button(&_buttonC);

	// enable pin change interrupt 0 and enable interrupts on Button A
	enable_pcint(&_interruptA);
	// enable_pcint(&_interruptC);

	// set up action when button A is pressed/released
	setup_button_action(&_interruptA, PRESS, empty_function);
	setup_button_action(&_interruptA, RELEASE, A_release);
}

// setup for timer 0
void timer0_setup() {
	// stop other ISRs in order to set up this timer
	// it is automatically implemented in the system but do it for safety
	cli();

	// CTC mode
	TCCR0A |= (1 << WGM01);
	// set CS bits to 011 to set prescaler to 64
	TCCR0B |= (1 << CS01) | (1 << CS00);
	// initialize timer/counter register
	TCNT0 = 0;
	// to obtain an interrupt time period at 1ms, match at the 249th tick per interrupt
	OCR0A = 249;
	// enable compare match
	TIMSK0 |= (1 << OCIE0A);
}

// setup for timer 1
void timer1_setup() {
	cli();

	// CTC mode and set CS bits to 100 to set prescaler to 256
	TCCR1B |= (1 << WGM12) | (1 << CS02);
	// initialization
	TCNT1 = 0;
	// to obtain an interrupt time period at 250ms, match at the 15624th tick per interrupt
	OCR1A = 15624;
	// enable compare match
	TIMSK1 |= (1 << OCIE1A);
}

// timer 0 interrupt
ISR(TIMER0_COMPA_vect) {
	// increment the clock whenever the interrupt is fired
	ms_ticks++;
}

// timer 1 interrupt
ISR(TIMER1_COMPA_vect) {
	// toggle the yellow LED whenever the interrupt is fired
	led_toggle(&(_GPIO_yellow));
}

/****************************************************************************
   MAIN
****************************************************************************/

int main(void) {
  // This prevents the need to reset after flashing
  USBCON = 0;

	initialize_system();

	// initalize the clock for timer 0
	ms_ticks = 0;
	// call the setup function
	timer0_setup();
	timer1_setup();

	// define a flag so ms_ticks wouldn't be needed to be compared to 500 and be reset after
	uint32_t red_expired = 500;
	sei();
  while(1) {
		// whenever 500ms is hit
		if (ms_ticks >= red_expired) {
			// toggle the red LED
			led_toggle(&(_GPIO_red));
			// update the flag
			red_expired = ms_ticks + 500;
		}
	} // end while(1)

} /* end main() */
