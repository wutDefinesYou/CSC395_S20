// Author: Amy Larson

#include "common.h"

void empty_function() {}
void(*cyclic_executive)(void) = empty_function;
void(*initialize_system)(void) = empty_function;

void function_to_initialize(void(*fn)(void)) {
  initialize_system = fn;
}

void loop_function(void(*fn)(void)) {
  cyclic_executive = fn;
}

int main(void) {
	// This prevents the need to reset after flashing
	USBCON = 0;
  main_setup();
	initialize_system();
	while(1) {
    cyclic_executive();
	}
}
