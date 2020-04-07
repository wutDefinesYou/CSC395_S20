
#ifndef TASKS_H_
#define TASKS_H_

#include "../pololu/common.h"
#include "../pololu/gpio_led.h"
#include "../pololu/scheduler.h"

void toggle_gpio_red();
void toggle_gpio_green();
void toggle_gpio_yellow();

void initialize_tasks();

#endif
