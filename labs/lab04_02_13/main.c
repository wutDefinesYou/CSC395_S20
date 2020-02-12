#include "common.h"

#include <avr/io.h>
#include <util/delay.h>

#include "leds.h"
#include "buttons.h"

#define INVERTED 1

// for use of button action setup
#define RELEASE 1
#define PRESS 0

// enums of states of yellow and green LEDs
enum Yellow_States {Y_Init, Y_First, Y_Second, Y_Third} Yellow_State;
enum Green_States {G_Init, G_First, G_Second, G_Third} Green_State;

// blink timers for yellow and green LEDs
uint8_t timerY = 0;
uint8_t timerG = 0;

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

	// initalize only buttonA and buttonC because they are connected to PCINT
	// NOTE: button C and the RED led are on the same line.
	initialize_button(&_buttonA);
	initialize_button(&_buttonC);

	// enable pin change interrupt 0 and enable interrupts on Button A and C
	enable_pcint(&_interruptA);
	enable_pcint(&_interruptC);

	// set up the function to be called as the action when the button is pressed or released
	// when pressed do nothing
	// when released call release function
	// the third argument we are passing in is a pointer to a function. That is function name only
	setup_button_action(&_interruptA, PRESS, empty_function);
	setup_button_action(&_interruptA, RELEASE, A_release);
	setup_button_action(&_interruptC, PRESS, empty_function);
	setup_button_action(&_interruptC, RELEASE, C_release);
}

// State machine for yellow LED
// basically three states
// first release: LED on
// second release: blink
// third release: LED off
void Yellow_Switch (){
	switch(Yellow_State) {
		case Y_Init:
			// initially LED is off
			led_off(&(_yellow), !INVERTED);
			// if button A pressed and released turn to state first release
			if (1 == A_released % 3) {
				Yellow_State = Y_First;
			}
			break;
		case Y_First:
			// after first release LED keeps on
			led_on(&(_yellow), !INVERTED);
			// if button A pressed and released now it turns into state second release
			if (2 == A_released % 3) {
				Yellow_State = Y_Second;
			}
			break;
		case Y_Second:
			// after second release LED will blink at 0.4 Hz
			// 0ms LED off. 1250ms LED on. 2500ms LED off. 3750ms LED on ...
			if (1 == timerY % 2) {
				led_on(&(_yellow), !INVERTED);
			}
			if (0 == timerY % 2) {
				led_off(&(_yellow), !INVERTED);
			}
			_delay_ms(1250);
			timerY = (timerY + 1) % 2;

			// if button A pressed and released now turn to state third release
			if (0 == A_released % 3) {
				Yellow_State = Y_Third;
			}
			break;
		case Y_Third:
			// after third release LED is turned off
			led_off(&(_yellow), !INVERTED);
			// refresh the counter flag
			A_released = A_released % 3;

			// if pressed and released now, turn to first state again and start a new round
			if (1 == A_released % 3) {
				Yellow_State = Y_First;
			}
			break;
		default:
			Yellow_State = Y_Init;
	}
}

// see comments for Yellow_Switch for reference. Same idea
// only difference green LED blinks at 2 Hz
void Green_Switch (){
	switch(Green_State) {
		case G_Init:
			led_off(&(_green), INVERTED);
			if (1 == C_released % 3) {
				Green_State = G_First;
			}
			break;
		case G_First:
			led_on(&(_green), INVERTED);
			if (2 == C_released % 3) {
				Green_State = G_Second;
			}
			break;
		case G_Second:
			if (1 == timerG % 2) {
				led_on(&(_green), INVERTED);
			}
			if (0 == timerG % 2) {
				led_off(&(_green), INVERTED);
			}
			_delay_ms(250);
			timerG = (timerG + 1) % 2;

			if (0 == C_released % 3) {
				Green_State = G_Third;
			}
			break;
		case G_Third:
			led_off(&(_green), INVERTED);
			C_released = C_released % 3;

			if (1 == C_released % 3) {
				Green_State = G_First;
			}
			break;
		default:
			Green_State = G_Init;
	}
}
/****************************************************************************
   MAIN
****************************************************************************/

int main(void) {
  // This prevents the need to reset after flashing
  USBCON = 0;

	initialize_system();

	// initialize the counter flag for button A
	A_released = 0;
	// initalize the state machine for the yellow LED
	Yellow_State = Y_Init;

	// initialize the counter flag for button C
	C_released = 0;
	// initalize the state machine for the green LED
	Green_State = G_Init;
  //*************************************************************//
  //*******         THE CYCLIC CONTROL LOOP            **********//
  //*************************************************************//

  // FILL THIS IN TO MEET FUNCTIONAL REQUIREMENTS:

	// Always toggle yellow LED every 1000ms
	// Toggle greed LED every 500ms, but only if is in an on state
	// Press and release button A switches green to on state.
	// Press and release button C switches green to off state.

	// Assume both buttons start in a not pressed state.

	// call this function to enable your ISR to listen to external events
	sei();

  while(1) {
		// start yellow LED state machine
		Yellow_Switch();
		// start green LED state machine
		Green_Switch();
	} // end while(1)

} /* end main() */
