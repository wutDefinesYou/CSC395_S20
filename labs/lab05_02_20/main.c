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

	// GPIO_initialize_led(GPIO_RED);
	// GPIO_initialize_led(GPIO_GREEN);
	// GPIO_initialize_led(GPIO_YELLOW);
	GPIO_initialize_led(GPIO_RED2);

	// The "sanity check".
	// When you see this pattern of lights you know the board has reset
	light_show();

	// initalize button A
	initialize_button(&_buttonA);
	initialize_button(&_buttonC);

}

// setup for timer 1
void timer1_setup() {
	// WGM 1111 to set to fast PWM mode
	// COM 10 to set to non-inverted mode
	// CS 101 to set to a 1024 prescaler
	TCCR1A |= (1 << WGM11) | (1 << WGM10) | (1 << COM1B1);
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS12) | (1 << CS10);

	// top of the PWM waveform
	OCR1A = 0xFFFF;
}

/****************************************************************************
   MAIN
****************************************************************************/

int main(void) {
  // This prevents the need to reset after flashing
  USBCON = 0;

	initialize_system();

	// call the setup function
	timer1_setup();

	// sei();

	// initialize the match number
	OCR1B = 0;
	// flag to identify LED state
	uint8_t on = 0;
	// brightness of LED
	uint16_t brightness;
  while(1) {
		// if button C released, change the frequency
		if (is_button_released(&_buttonC)) {
			if (!on) {
				// going from 0 to 100
				OCR1B += OCR1A / 4;
				// reset the flag
				if (OCR1B == OCR1A) {
					on = 1;
				}
			}
			else {
				// going from 100 to 0
				OCR1B -= OCR1A / 4;
				// reset the flag
				if (OCR1B == 0) {
					on = 0;
				}
			}
		}

		// if button A released, change the frequency with showing each level of duty cycle
		// the time period of the waveform has to be small so the frequency change can work as a brightness change. Do so by changing the value of prescaler and the top

		if (is_button_released(&_buttonA)) {
			if (!on) {
				// 0 to 100 (LED off to on)
				for (brightness=0;brightness<OCR1A;brightness++) {
					OCR1B = brightness;
					_delay_ms(10);
				}
				on = 1;
			}
			else {
				// 100 to 0 (LED on to off)
				for (brightness=OCR1A;brightness>0;brightness--) {
					OCR1B = brightness;
					_delay_ms(10);
				}
				on = 0;
			}
		}
	} // end while(1)

} /* end main() */
