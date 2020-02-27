#ifndef TASK_H_
#define TASK_H_

#include "common.h"
#include <avr/io.h>
#include <inttypes.h>

// constants serving as parameters to the switch statement in the initialize_task
#define TASKRED 0
#define TASKYELLOW 1
#define TASKBUTTONA 2
#define TASKBUTTONC 3
#define TASKPRINT 4

// task structure
typedef struct {
  uint8_t ready;
  uint32_t nextRelease;
  uint32_t period;
  void (*tick_fn) (void);
} task;

// declare tasks first
extern task _taskRed;
extern task _taskYellow;
extern task _taskButtonA;
extern task _taskButtonC;
extern task _print;

// flags or counters that we will use
// see main() for details
volatile uint8_t on;
volatile uint16_t pressed_times;
volatile uint8_t implementedA;
volatile uint8_t implementedC;

void initialize_task(int taskType);

void task_red();

void task_yellow();

void task_buttonA();

void task_buttonC();

void counter_print();

#endif
