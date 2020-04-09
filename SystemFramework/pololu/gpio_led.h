#ifndef GPIO_LED_H_
#define GPIO_LED_H_

#include "common.h"

// These function as enumerated types (using pin numbers as values)
#define GPIO_GREEN 4
#define GPIO_YELLOW 1
#define GPIO_RED 2

typedef struct {
  volatile uint8_t * ddr;
  volatile uint8_t * port;
  volatile uint8_t * portin;
  uint8_t pin;
  uint8_t configured;
} GPIO_LED_struct;

// Global variables. They MUST be defined using initialize_led(<COLOR>).
extern GPIO_LED_struct _gpio_yellow;
extern GPIO_LED_struct _gpio_red;
extern GPIO_LED_struct _gpio_green;

/* configure the gpio led as output */
void gpio_configure_led(GPIO_LED_struct * color);

/* Flash led (on,delay,off,delay) */
void gpio_flash_led(GPIO_LED_struct * color);

/* Flash all configured gpio leds for a sanity check light show */
void gpio_light_show();

void gpio_led_on(GPIO_LED_struct * color);
void gpio_led_off(GPIO_LED_struct * color);
void gpio_led_toggle(GPIO_LED_struct * color);


#endif
