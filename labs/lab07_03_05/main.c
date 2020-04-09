#include "common.h"

#include <avr/interrupt.h>
#include <util/delay.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "motor.h"
#include "leds.h"
#include "communication.h"

// flags to identify which rotate task is going on
uint8_t Frotate360 = 0;
uint8_t Brotate360 = 0;

/****************************************************************************
   ALL INITIALIZATION
****************************************************************************/
void initialize_system(void)
{
  // initalize green and yellow only.
	// initialization defines the IO_structs and sets DDR
	initialize_led(GREEN);
	initialize_led(YELLOW);

	// The "sanity check".
	// When you see this pattern of lights you know the board has reset
	light_show();

  setupMotor2();
  setupEncoder();
  motorForward();
}

void blink_yellow() {
  TOGGLE_BIT(*(&_yellow)->port, _yellow.pin);
  _delay_ms(300);
  TOGGLE_BIT(*(&_yellow)->port, _yellow.pin);
}

void move_motor() {
  // going faster
  OCR1B = 0;
  _delay_ms(1000);
  OCR1B = 250;
  _delay_ms(1000);
  OCR1B = 500;
  _delay_ms(1000);
  OCR1B = 750;
  _delay_ms(1000);
  OCR1B = 1000;
  blink_yellow();

  // going slower
  OCR1B = 750;
  _delay_ms(1000);
  OCR1B = 500;
  _delay_ms(1000);
  OCR1B = 250;
  _delay_ms(1000);
  OCR1B = 0;
  blink_yellow();
}

/****************************************************************************
   MAIN
****************************************************************************/

int main(void) {
  // This prevents the need to reset after flashing
  USBCON = 0;

	initialize_system();

	menu = "MENU\n\r";
	recv_buffer_ptr = 0;
	user_command_ready = 0;

	//DDRD = 0x
	setupUART();
	sendString("Here we go!\r\n ");

  sei();

  while(1) {

		if (user_command_ready) {
			sendChar('B');
			handleInput();
		}

		// if motorForward
		// correspond to A pressed
		if (Frotate360 == 1) {
			reference = global_counts_m2 + 2249;
			motorForward();
			OCR1B = 500;
			// if have rotated +360 degrees, stop
			if (global_counts_m2 >= reference) {
				OCR1B = 0;
				Frotate360 = 0;
			}
		}

		// if motorBackward
		// correspond to C pressed
		if (Brotate360 == 1) {
			reference = global_counts_m2 - 2249;
			motorBackward();
			OCR1B = 500;
			// if have rotated -360 degree, stop
			if (global_counts_m2 <= reference) {
				OCR1B = 0;
				Brotate360 = 0;
			}
		}
  } /* end while(1) loop */
} /* end main() */
