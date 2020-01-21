/*
To find locations (i.e. ports) of I/O, consult the AStar pinout.

The information has been summarized in the AStar DataCheatSheet document
on Github.

Control of the I/O can be found in the AtMega Datasheet.
*/

/*
This is a demonstration of interacting with the on-board led.
Green LED = Port D, pin 5
And a button.
Button A = Port B, pin 3
Yellow LED = Port C, pin 7
Button C = Post B, pin 0
*/

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

int main() {

  // This prevents the need to reset after flashing
  //USBCON = 0;

  // Configures green LED pin as output
  // DDRD: Data Direction Register for Port D
  // DDD5: Data Direction pin/bit mask of Port D, pin 5
  DDRD |= ( 1 << DDD5 );

  // Configure button A: Port B, pin 3
  // as input
  DDRB &= ~(1<<DDB3);

  // Enable Button pull-up resistor
  PORTB |= (1<<PORTB3);

  // Configure yellow LED as output
  // DDC7: Data Direction pin/bit mask of Port C, pin 7
  DDRC |= (0x01 << DDC7);

  // Configure button C: Port B, pin 0 as input
  DDRB &= ~(0x01);

  // Enable button pull-up resistor
  PORTC |= (0x01);

  // Perform sanity check.
  // Is the LED configured properly?
  // Is the board reset and running main?

  // Green ON then off quick for sanity check
  // Green is turned on by setting port low!! (Not true for all the LEDs)
  // Yellow is turned on by setting port high
  int i;
  for (i=0; i<3; i++) {
    // Yello and Green ON
    PORTC |= (0x01 << PORTC7);
    PORTD &= ~( 1 << PORTD5 );
    _delay_ms(250);

    // Yellow and Green OFF
    PORTC &= ~( 0x01 << PORTC7);
    PORTD |= ( 1 << PORTD5 );
    _delay_ms(250);
  }

  while(1) {
    // If button A is pressed, green led is on.
    if (0 == (PINB & (1 << 3))) {
      PORTD ^= (1 << PORTD5);
    } else {
      PORTD |= (1 << PORTD5);
    }

    // If button C is pressed, yellow LED is on
    if (0 == (PINB & 0x01)){
      PORTC |= (0x01 << PORTC7);
    }
    else {
      PORTC &= ~( 0x01 << PORTC7);
    }
  } // end while
} // end main()
