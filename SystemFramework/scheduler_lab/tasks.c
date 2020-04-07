#include "tasks.h"

void toggle_gpio_red() {
  gpio_led_toggle(&_gpio_red);
}
void toggle_gpio_green() {
  gpio_led_toggle(&_gpio_green);
}
void toggle_gpio_yellow() {
  gpio_led_toggle(&_gpio_yellow);
}

/* Add tasks to scheduler */
void initialize_tasks() {
    spawn_task(500,toggle_gpio_red);
    spawn_task(250,toggle_gpio_yellow);
    spawn_task(600,toggle_gpio_green);
}
