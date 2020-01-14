/*
To find locations (i.e. ports) of I/O, consult the AStar pinout.

The information has been summarized in the AStar DataCheatSheet document
on Github.

Control of the I/O can be found in the AtMega Datasheet.
*/

/*
This is a demonstration of interacting with the on-board leds.
Yellow LED = Port C, pin 7
Green LED = Port D, pin 5
*/

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

char blink = 0;
volatile long counter = 0;

int main() {

  // This prevents the need to reset after flashing
  //USBCON = 0;

  // Configures yellow LED pin as output
  // DDRC: Data Direction Register for Port C
  // DDC7: Data Direction pin/bit mask of Port C, pin 7
  DDRC |= (1 << DDC7);
  // Configure Green LED
  DDRD |= ( 1 << DDD5 );
  // Configure Red LED
  DDRB |= (1 << DDB0 );

  // Configure button A: Port B, pin 3
  // as input
  DDRB &= ~(1<<DDB3);

  // Enable Button pull-up resistor
  PORTB |= (1<<PORTB3);

  // PCICR: Pin Change Interrupt Control Registe
  // PCIE0: Pin Change Interrupt Enable Bit:
  //    Any change on any enabled PCINT7..0 can fire ISR.
  PCICR |= ( 1 << PCIE0 );

  // PCMSK0: Pin Change Mask for Interrupt0, which is for all pins 0 through 7
  // Enable interrupts on Button A (PCINT3) and Button C (PCINT0)
  PCMSK0 |= ( 1 << PCINT3 );

  // Perform sanity check.
  // Are the LEDs configured properly?
  // Is the board reset and running main?

  // Yellow and Green ON then off quick for sanity check
  // Green is turned on by setting port low
  int i;
  for (i=0; i<3; i++) {
    // Yellow and Green ON
    PORTC |= ( 1 << PORTC7 );
    PORTD &= ~( 1 << PORTD5 );
    _delay_ms(200);

    // Yellow and Green OFF
    PORTC &= ~( 1 << PORTC7 );
    PORTD |= ( 1 << PORTD5 );
    _delay_ms(200);
  }

  sei();
  while(1) {
    // Blink green regardless of button state
    ++counter;
    if (counter >= 40000) {
      PORTD ^= (1 << PORTD5);
      counter = 0;
      if (blink) {
        PORTC ^= (1 << PORTC7);
      }
    }
  }
}

ISR(PCINT0_vect) {
  // Turn on RED led to indicate inside ISR
  PORTB &= ~(1<<PORTB0);
  // Check if it is pressed (not released)
  if (0 == (PINB & (1 << 3))) {
    // Check again in case of noise on system
    _delay_ms(10);
    if (0 == (PINB & (1 << 3))) {
      // toggle blink
      if (blink) {
        blink = 0;
        PORTC &= ~(1 << PORTC7);
      }
      else {
        blink = 1;
        counter = 0;
      }
    }
  }
}
