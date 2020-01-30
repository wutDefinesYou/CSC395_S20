#include "common.h"

#include <avr/io.h>
#include <util/delay.h>

#include "leds.h"
#include "buttons.h"

#define INVERTED 1

/****************************************************************************
   ALL INITIALIZATION
****************************************************************************/
void initialize_system(void) {
	// initalize green and yellow only.
	// initialization defines the IO_structs and sets DDR
	initialize_led(GREEN);
	initialize_led(YELLOW);

	// The "sanity check".
	// When you see this pattern of lights you know the board has reset
	light_show();

	// initalize only buttonA and buttonC because they are connected to PCINT
	// NOTE: button C and the RED led are on the same line.
  initialize_button(BUTTONA);
	initialize_button(BUTTONC);
}

/****************************************************************************
   MAIN
****************************************************************************/

int main(void) {
  // This prevents the need to reset after flashing
  USBCON = 0;

	initialize_system();

  //*************************************************************//
  //*******         THE CYCLIC CONTROL LOOP            **********//
  //*************************************************************//

  // FILL THIS IN TO MEET FUNCTIONAL REQUIREMENTS:

	// Always toggle yellow LED every 1000ms
	// Toggle greed LED every 500ms, but only if is in an on state
	// Press and release button A switches green to on state.
	// Press and release button C switches green to off state.

	// Assume both buttons start in a not pressed state.

	uint8_t green_on = 1;
	uint8_t timer = 0;	// set up a timer
	uint8_t stateA = is_button_pressed(&_buttonA);	// obtain the state of button A
	uint8_t stateC = is_button_pressed(&_buttonC); 	// obtain the state of button C

  while(1) {
		// if button A is not pressed
		if (!stateA) {
			// see if being pressed
			stateA = is_button_pressed(&_buttonA);
		}
		// if pressed
		else {
			// if pressed and released
			if (stateA && !is_button_pressed(&_buttonA)) {
				// LED green on
					led_on(&_green,INVERTED);
					stateA = is_button_pressed(&_buttonA);
			}
		}

		// if button C is not pressed
		if (!stateC) {
			// see if being pressed
			stateC = is_button_pressed(&_buttonC);
		}
		// if pressed
		else {
			// if pressed and released
			if (stateC && !is_button_pressed(&_buttonC)) {
				// LED green off
					led_off(&_green,INVERTED);
					stateC = is_button_pressed(&_buttonC);
			}
		}

		//hang on for 500ms first
		_delay_ms(500);
		// check and reset the timer
		timer = (timer + 1) % 4;
		// if any multiple of 500ms
		if (0 == timer % 1) {
			// toggle the green LED
			if (green_on) {
				led_toggle(&_green);
			} else {
				led_off(&_green,INVERTED);
			}
		}
		// if any multiple of 1000ms
		if (0 == timer % 2) {
			// toggle the yellow LED
			led_toggle(&_yellow);
		}

	} // end while(1)

} /* end main() */
