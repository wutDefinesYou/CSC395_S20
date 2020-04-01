#ifndef ANALOG_H_
#define ANALOG_H_

#include "common.h"

void initialize_pot();

void adc_init();

uint16_t adc_read(uint8_t channel);

#endif
