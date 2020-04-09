#include "common.h"
#include "buttons.h"
#include "leds.h"
#include "motor.h"
#include "task.h"
#include "serial.h"
#include "analog.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <string.h>

#define INVERTED 1

volatile uint64_t ms_ticks = 0;
uint64_t taskTime = 0;
uint64_t loopTime = 0;
volatile int16_t global_last_m2a_val = 0;
volatile int16_t global_last_m2b_val = 0;

void initialize_system(void) {
	// initalize green and yellow only.
	// initialization defines the IO_structs and sets DDR
	initialize_led(GREEN);
	initialize_led(YELLOW);

	// GPIO_initialize_led(GPIO_RED);
	GPIO_initialize_led(GPIO_GREEN);
	// GPIO_initialize_led(GPIO_YELLOW);
	GPIO_initialize_led(GPIO_RED2);

	// initalize my task structures
	initialize_task(TASKRED);
	initialize_task(TASKYELLOW);
	initialize_task(TASKBUTTONA);
	initialize_task(TASKBUTTONC);
	initialize_task(TASKPRINT);

  // initalize button A and C
  initialize_button(&_buttonA);
  initialize_button(&_buttonC);

	// The "sanity check".
	// When you see this pattern of lights you know the board has reset
	light_show();

  setupUART();
  sendString("Here we go!\r\n ");

  initialize_pot();
}

// setup for timer0
void timer0_setup() {
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

// fire my ISR every 1 ms according to the setup in timer 0
ISR(TIMER0_COMPA_vect) {
	ms_ticks++;
}

void scheduler_task() {
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

void encoder_task() {
  // Make a copy of the current reading from the encoders
  uint8_t tmpB = PINB;

  // Get value of each channel, making it either a 0 or 1 valued integer
  uint8_t m2a_val = (tmpB & (1 << chA_pin )) >> chA_pin;
  uint8_t m2b_val = (tmpB & (1 << chB_pin )) >> chB_pin;

  // Adding or subtracting counts is determined by how these change between interrupts
  int8_t plus_m2 = m2a_val ^ global_last_m2b_val;
  int8_t minus_m2 = m2b_val ^ global_last_m2a_val;

  // Add or subtract encoder count as appropriate
  if(plus_m2) { global_counts_m2 += 1; }
  if(minus_m2) { global_counts_m2 -= 1; }

  // If both values changed, something went wrong - probably missed a reading
  if(m2a_val != global_last_m2a_val && m2b_val != global_last_m2b_val) {
    global_error_m2 = 1;
    PORTD ^= (1 << PORTD5);
  }

  // Save for next interrupt
  global_last_m2a_val = m2a_val;
  global_last_m2b_val = m2b_val;
}

uint16_t potentiometer_task(uint8_t channel) {
  ADMUX &= 0b11100000;
  if (channel >= 8) {
    ADCSRB |= (1<<MUX5);
    ADMUX |= (channel - 8);
  }
  else {
    ADCSRB &= ~(1<<MUX5);
    ADMUX |= channel;
  }

  // start single conversion
  // write '1' to ADSC
  ADCSRA |= (1<<ADSC);

  // wait for conversion to complete
  // ADSC becomes '0' again
  // till then, run loop continuously
  while(ADCSRA & (1<<ADSC));

  return (ADC);
}

int main(void) {
  USBCON = 0;

  initialize_system();
  timer0_setup();

  sei();
  uint32_t iterations = 100000;

  if (user_command_ready) {
    sendChar('B');
    handleInput();
  }

  cli();
  uint64_t start = ms_ticks;
  sei();

  uint32_t i;
  for(i=0; i<iterations; i++){
    __asm__ __volatile__("nop");
  }

  cli();
  loopTime = ms_ticks - start;
  char loop_str[50];
  sprintf(loop_str, "%d", (int)loopTime);
  strcat(loop_str, " <- loop time\n");
  sendString(loop_str);
  start = ms_ticks;
  sei();
  led_on(&_yellow, 0);

  for(i=0; i<iterations; i++){
    scheduler_task();
  }

  cli();
  taskTime = ms_ticks - start - loopTime;
  char taskTime_str[50];
  sprintf(taskTime_str, "%d", (int)taskTime);
  strcat(taskTime_str, " <- scheduler task time\n");
  sendString(taskTime_str);
  sei();
  led_on(&_green, 1);

  cli();
  start = ms_ticks;
  taskTime = 0;
  sei();

  for(i=0; i<iterations; i++){
    encoder_task();
  }

  cli();
  taskTime = ms_ticks - start - loopTime;
  char encoder_str[50];
  sprintf(encoder_str, "%d", (int)taskTime);
  strcat(encoder_str, " <- encoder task time\n");
  sendString(encoder_str);
  sei();
  led_on(&_GPIO_red2, 0);

  cli();
  start = ms_ticks;
  taskTime = 0;
  sei();

  for(i=0; i<iterations; i++){
    potentiometer_task(10);
  }

  cli();
  taskTime = ms_ticks - start - loopTime;
  char pot_str[50];
  sprintf(pot_str, "%d", (int)taskTime);
  strcat(pot_str, " <- potentiometer task time\n");
  sendString(pot_str);
  sei();
  led_on(&_GPIO_green, 0);

  return 0;
} /* end main() */
