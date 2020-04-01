#include "task.h"
#include "buttons.h"
#include "leds.h"
#include <stdio.h>

// define tasks
task _taskRed = {0, 0, 0, empty_function};
task _taskYellow = {0, 0, 0, empty_function};
task _taskButtonA = {0, 0, 0, empty_function};
task _taskButtonC = {0, 0, 0, empty_function};
task _print = {0, 0, 0, empty_function};

// initalize tasks
void initialize_task(int taskType) {
  switch(taskType){
    case(TASKRED):
      // update my task period
      _taskRed.period = 500;
      // update my task function
      _taskRed.tick_fn = task_red;
      break;
    case(TASKYELLOW):
      _taskYellow.period = 250;
      _taskYellow.tick_fn = task_yellow;
      break;
    case(TASKBUTTONA):
      _taskButtonA.period = 1000;
      _taskButtonA.tick_fn = task_buttonA;
      break;
    case(TASKBUTTONC):
      _taskButtonC.period = 2000;
      _taskButtonC.tick_fn = task_buttonC;
      break;
    case(TASKPRINT):
      _print.period = 3000;
      _print.tick_fn = counter_print;
      break;
    default:
      break;
  }
}

// task of red led
// toggle the red led every 500ms
void task_red() {
  led_toggle(&_GPIO_red2);
}

// task of yellow led
// toggle the yellow led every 250ms
void task_yellow() {
  led_toggle(&_GPIO_green);
}

// task of button A
// increment a counter every 1s when button A is pressed
void task_buttonA() {
  // if button A pressed
  if(is_button_pressed(&_buttonA)) {
    // if the task hasn't been implemented
    if(!implementedA) {
      // count
      pressed_times++;
      // refresh the flag
      implementedA = 1;
    }
  }
}

void task_buttonC() {
  // if button C is pressed
  // change the frequency of another red led every 2s if button C is pressed
  if (is_button_pressed(&_buttonC)) {
    // if the task hasn't been implemented
    if(!implementedC) {
      // if the red led is not turned on
      if (!on) {
        // going from 0 to 100
        OCR1B += OCR1A / 4;
        // reset the flag
        if (OCR1B == OCR1A) {
          on = 1;
        }
      }
      // if the red led is turned on
      else {
        // going from 100 to 0
        OCR1B -= OCR1A / 4;
        // reset the flag
        if (OCR1B == 0) {
          on = 0;
        }
      }
      // refresh the flag
      implementedC = 1;
    }
  }
}

// need a dongle to have the actual output printing out
void counter_print() {
  printf("Button A has been pressed %d times.\n", pressed_times);
}
