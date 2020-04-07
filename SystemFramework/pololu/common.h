// common.h
//
// Author: Amy Larson

#ifndef COMMON_H_
#define COMMON_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <inttypes.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define TRUE 1
#define FALSE 0

#define PIN0 0
#define PIN1 1
#define PIN2 2
#define PIN3 3
#define PIN4 4
#define PIN5 5
#define PIN6 6
#define PIN7 7

#define SET_BIT(reg, pin) (reg |= (1<<pin))
#define CLEAR_BIT(reg, pin) (reg &= ~(1<<pin))
#define TOGGLE_BIT(reg, pin) (reg ^= (1<<pin))

#define CONFIG_OUTPUT(port, pin) (SET_BIT(port,pin))
#define CONFIG_INPUT(port,pin) (CLEAR_BIT(port,pin))

typedef struct {
  volatile uint8_t * ddr;
  volatile uint8_t * port;
  volatile uint8_t * portin;
  uint8_t pin;
} IO_struct;

extern volatile uint64_t ms_ticks;

/* Used as a placeholder for any function pointer to be defined.
  These are defined and initialized in main. */
extern void empty_function();
extern void(*initialize_system)(void);
extern void(*cyclic_executive)(void);

// functions for setting the initialization and control loop
void function_to_initialize(void(*fn)(void));
void loop_function(void(*fn)(void));

void main_setup();

#endif
