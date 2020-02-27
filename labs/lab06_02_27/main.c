#include "common.h"

#include <avr/io.h>
#include <util/delay.h>

#include "leds.h"
#include "buttons.h"
#include "task.h"

#define INVERTED 1

// counter
uint32_t ms_ticks;

/****************************************************************************
   ALL INITIALIZATION
****************************************************************************/
void initialize_system(void) {
	// initalize green and yellow only.
	// initialization defines the IO_structs and sets DDR
	initialize_led(GREEN);
	initialize_led(YELLOW);

	GPIO_initialize_led(GPIO_RED);
	// GPIO_initialize_led(GPIO_GREEN);
	GPIO_initialize_led(GPIO_YELLOW);
	GPIO_initialize_led(GPIO_RED2);

	// initalize my task structures
	initialize_task(TASKRED);
	initialize_task(TASKYELLOW);
	initialize_task(TASKBUTTONA);
	initialize_task(TASKBUTTONC);
	initialize_task(TASKPRINT);

	// The "sanity check".
	// When you see this pattern of lights you know the board has reset
	light_show();

	// initalize button A and C
	initialize_button(&_buttonA);
	initialize_button(&_buttonC);

}

// setup for timer0
void timer0_setup() {
	cli();

	// CTC mode
	TCCR0A |= (1 << WGM01);
	// prescaler 64
	TCCR0B |= (1 << CS01) | (1 << CS00);
	// initialize timer 0
	TCNT0 = 0;
	// match number
	OCR0A = 249;
	// timer mask
	TIMSK0 |= (1 << OCIE0A);
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

// fire my ISR every 1 ms according to the setup in timer 0
ISR(TIMER0_COMPA_vect) {
	// count my ticks
	ms_ticks++;
	// if time for red led task, make it ready
	if (ms_ticks >= _taskRed.nextRelease) {
		_taskRed.ready = 1;
		_taskRed.nextRelease = ms_ticks + _taskRed.period;
	}
	// if time for yellow led task, make it ready
	if (ms_ticks >= _taskYellow.nextRelease) {
		_taskYellow.ready = 1;
		_taskYellow.nextRelease = ms_ticks + _taskYellow.period;
	}
	// if time for button A task, make it ready
	if (ms_ticks >= _taskButtonA.nextRelease) {
		_taskButtonA.ready = 1;
		_taskButtonA.nextRelease = ms_ticks + _taskButtonA.period;
	}
	// if time for button C task, make it ready
	if (ms_ticks >= _taskButtonC.nextRelease) {
		_taskButtonC.ready = 1;
		_taskButtonC.nextRelease = ms_ticks + _taskButtonC.period;
	}
	// if time to print, make it ready
	if (ms_ticks >= _print.nextRelease) {
		_print.ready = 1;
		_print.nextRelease = ms_ticks + _print.period;
	}
}

/****************************************************************************
   MAIN
****************************************************************************/

int main(void) {
  // This prevents the need to reset after flashing
  USBCON = 0;

	initialize_system();

	// call the setup function
	timer0_setup();
	timer1_setup();

	// initialize the match number
	OCR1B = 0;
	// flag to identify the second GPIO red LED state, default: off
	on = 0;
	// counter on times button A being pressed, default: 0
	pressed_times = 0;
	// if button A task has been implemented, default: no
	// have this flag in order to avoid multiple times of implementation of the
	// task when the button is being holded on pressing
	implementedA = 0;
	// if button C task has been implemented, default: no
	implementedC = 0;

	sei();
  while(1) {
		// after button A task implemented, if the button is released, reset the flag
		if(is_button_released(&_buttonA)) {
			implementedA = 0;
		}
		// after button C task implemented, if the button is released, reset the flag
		if(is_button_released(&_buttonC)) {
			implementedC = 0;
		}
		// implement red led task if it's ready
		if (_taskRed.ready) {
			_taskRed.tick_fn();
			_taskRed.ready = 0;
		}
		// implement yellow led task if it's ready
		if (_taskYellow.ready) {
			_taskYellow.tick_fn();
			_taskYellow.ready = 0;
		}
		// implement button A task if it's ready
		if (_taskButtonA.ready) {
			_taskButtonA.tick_fn();
			_taskButtonA.ready = 0;
		}
		// implement button C task if it's ready
		if (_taskButtonC.ready) {
			_taskButtonC.tick_fn();
			_taskButtonC.ready = 0;
		}
		// implement print task if it's ready
		if (_print.ready) {
			_print.tick_fn();
			_print.ready = 0;
		}
	} // end while(1)

} /* end main() */
