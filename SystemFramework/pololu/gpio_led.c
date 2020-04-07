
#include "gpio_led.h"

// Global variables. They MUST be defined using initialize_led(<COLOR>).
GPIO_LED_struct _gpio_yellow = { &DDRD, &PORTD, &PIND, GPIO_YELLOW, 0 };
GPIO_LED_struct _gpio_red = { &DDRD, &PORTD, &PIND, GPIO_RED, 0 };
GPIO_LED_struct _gpio_green = { &DDRD, &PORTD, &PIND, GPIO_GREEN, 0 };

/**
 * configure the data direction for the specified on-board led.
 */
void gpio_configure_led(GPIO_LED_struct * color) {
  SET_BIT(*color->ddr, color->pin);
  color->configured = 1;
}

void gpio_led_on(GPIO_LED_struct * color) {
  SET_BIT(*color->port, color->pin);
}

void gpio_led_off(GPIO_LED_struct * color) {
  CLEAR_BIT(*color->port, color->pin);
}

void gpio_led_toggle(GPIO_LED_struct * color) {
  TOGGLE_BIT(*color->port, color->pin);
}


/* Flash the designated on-board led for 250ms on, then 250ms off.
 * Assumes led is initialized */
void gpio_flash_led(GPIO_LED_struct * color) {
  SET_BIT(*color->port, color->pin);
  _delay_ms(150);
  TOGGLE_BIT(*color->port, color->pin);
  _delay_ms(150);
}


/* Flash all the initialized leds for a sanity check light show */
void gpio_light_show() {
  int i;
  for (i = 0; i < 2; i++) {
    if (_gpio_red.configured) gpio_flash_led(&_gpio_red);
    if (_gpio_yellow.configured) gpio_flash_led(&_gpio_yellow);
    if (_gpio_green.configured) gpio_flash_led(&_gpio_green);
  }
}
