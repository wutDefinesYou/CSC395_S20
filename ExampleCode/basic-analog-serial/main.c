
#define F_CPU 16000000
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

#include "analog.h"
#include "serial.h"

void initialize_heartbeat() {
  	DDRD |= (1<<DDD5);
}

void heartbeat_with_delay() {
  PORTD ^= (1<<DDD5);
  _delay_ms(100);
}

void initialize() {

  // initialize power supply
  // I use this generally to plug into breadboard to power everything
  // AStar pin 11
  DDRB |= ( 1 << DDB7 );
  PORTB |= ( 1 << PORTB7 );

  initialize_heartbeat();
  heartbeat_with_delay();
  heartbeat_with_delay();
  heartbeat_with_delay();
  heartbeat_with_delay();

  setupUART();
  adc_init();
}

int main()
{
    initialize();

    uint16_t adc_results;
    char buffer[50];

    sei();

    sendString("Here we go!\r\n ");

    while(1)
    {
      adc_results = adc_read();
      sprintf(buffer,"%d\r\n",adc_results);
      sendString(buffer);

      heartbeat_with_delay();
    }
}
