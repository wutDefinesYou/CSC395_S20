#include "leds.h"

/* Scope is limited to this file. Used to indicate if LED is initialized. */
static uint8_t bYellowInit = 0;
static uint8_t bRedInit = 0;
static uint8_t bGreenInit = 0;

// Indicators for LEDs from general purpose I/O
static uint8_t bGPIOYellowInit = 0;
static uint8_t bGPIORedInit = 0;
static uint8_t bGPIOGreenInit = 0;

/* configure the data direction for the specified on-board led.
 */
void configure_led(IO_struct * color) {
  SET_BIT(*color->ddr, color->pin);
}

void initialize_led(int color) {
  switch(color) {
    case (YELLOW) :
    _yellow = (IO_struct) { &DDRC, &PORTC, YELLOW, &PINC };
    configure_led(&_yellow);
    bYellowInit = 1;
    break;

    case(GREEN):
    _green = (IO_struct) { &DDRD, &PORTD, GREEN, &PIND };
    configure_led(&_green);
    bGreenInit = 1;
    break;

    case(RED):
    _red = (IO_struct) { &DDRB, &PORTB, RED, &PINB };
    configure_led(&_red);
    bRedInit = 1;
    break;
  }
}

void GPIO_initialize_led(int color) {
  switch(color) {
    case(GPIO_YELLOW):
      _GPIO_yellow = (IO_struct) {&DDRD, &PORTD, PIN1, &PIND};
      configure_led(&_GPIO_yellow);
      bGPIOYellowInit = 1;
      break;
    case(GPIO_GREEN):
      _GPIO_green = (IO_struct) {&DDRD, &PORTD, PIN4, &PIND};
      configure_led(&_GPIO_green);
      bGPIOGreenInit = 1;
      break;
    case(GPIO_RED):
      _GPIO_red = (IO_struct) {&DDRD, &PORTD, PIN2, &PIND};
      configure_led(&_GPIO_red);
      bGPIORedInit = 1;
      break;
  }
}

void led_on(IO_struct * color, int inverted) {
  if (!inverted) {
    SET_BIT(*color->port, color->pin);
  }
  else {
    CLEAR_BIT(*color->port, color->pin);
  }
}

void led_off(IO_struct * color, int inverted) {
  if (!inverted) {
    CLEAR_BIT(*color->port, color->pin);
  }
  else {
    SET_BIT(*color->port, color->pin);
  }
}

void led_toggle(IO_struct * color) {
  TOGGLE_BIT(*color->port, color->pin);
}

/* Flash the designated on-board led for 250ms on, then 250ms off.
 * Assumes led is initialized */
void flash_led(IO_struct * color, int inverted) {
  if (!inverted) {
    SET_BIT(*color->port, color->pin);
  } else {
    CLEAR_BIT(*color->port, color->pin);
  }
  _delay_ms(250);
  TOGGLE_BIT(*color->port, color->pin);
  _delay_ms(250);
}

/* Flash all the initialized leds for a sanity check light show */
void light_show() {
  int i;
  for (i = 0; i < 2; i++) {
    if (bYellowInit) flash_led(&_yellow, 0);  // not inverted (1 turns led on)
    if (bRedInit) flash_led(&_red, 1);  // inverted (0 turns led on)
    if (bGreenInit) flash_led(&_green, 1);

    // added LEDs on the breadboard to the light_show
    if (bGPIOYellowInit) flash_led(&_GPIO_yellow, 0);
    if (bGPIOGreenInit) flash_led(&_GPIO_green, 0);
    if (bGPIORedInit) flash_led(&_GPIO_red, 0);
  }
}
