#include "../pololu/leds.h"
//#include "../pololu/timers.h"
//#include "../pololu/buttons.h"
#include "../pololu/gpio_led.h"
#include "../pololu/scheduler.h"

#include "tasks.h"

volatile uint64_t ms_ticks = 0;

/**
 * System Initialization
 */
void initialize(void) {
  configure_led(&_yellow);
  configure_led(&_green);

  initialize_error();
  initialize_debug();

  /* configure the gpio led as output */
  gpio_configure_led(&_gpio_green);
  gpio_configure_led(&_gpio_yellow);
  gpio_configure_led(&_gpio_red);

  initialize_scheduler();
  initialize_tasks();

	// "sanity check"
	light_show();
  gpio_light_show();

  sei();
}

/*
void loop() {
  if (ms_ticks >= next_release) {
    led_toggle(&_yellow);
    next_release = ms_ticks + 1000;
  }
}
*/

void main_setup() {
  function_to_initialize(initialize);
  loop_function(server);
}
