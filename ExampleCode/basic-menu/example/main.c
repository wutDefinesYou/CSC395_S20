#ifdef VIRTUAL_SERIAL
#include <VirtualSerial.h>
#else
#warning VirtualSerial not defined, USB IO may not work
#define SetupHardware();
#define USB_Mainloop_Handler();
#endif

#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

#define menuString "f s b d: faster, slower, brighter, dimmer\n\r"

void handleInput( char c );
void handleCommand( char *command );


void init() {
    SetupHardware(); //This setups the USB hardware and stdio
    sei(); //Enables interrupts - needed for USB
}

int main()
{
    char c;


    DDRC |= (1<<DDC7);
    DDRD |= ( 1 << DDD5 );
    PORTD &= ~(1 << PORTD5 );
    _delay_ms(1000);
    PORTD |= (1 << PORTD5);

    init();

    // Give the user a chance to open up the terminal before
    // displaying welcome message.
    _delay_ms(3000);
    printf("A Simple Menu Using LUFA for communication via USB.\n\r");
    printf(menuString);

    sei();
    while(1)
    {
      USB_Mainloop_Handler(); //Handles USB communication
      if ( ( c = fgetc(stdin) ) != EOF ) {
        handleInput( c );
      }

      // toggle to show liveness
      PORTD ^= (1 << PORTD5 );
      _delay_ms(250);
    }
}

void handleInput( char c ) {
  // WARNING: This uses a busy-wait, thus will block main loop until done
  const int COMMAND_BUFF_LEN = 16;
  int i;
  char command[COMMAND_BUFF_LEN+1];

  USB_Mainloop_Handler();

  // Get chars until end-of-line received or buffer full
  for (i = 0; i < COMMAND_BUFF_LEN; i++) {

    // first char received as input parameter. next char fetched at bottom of for-loop

    // if its backspace, reset index
    if (c == '\b') {
      i -= 2;
      printf("\b \b");
      continue;
    }
    // if newline, go back 1 to write over
    else if (c == '\n') {
      i--;
      continue;
    }
    // if return, we have complete input
    else if (c == '\r') {
      break;
    }

    // otherwise, we got a char - echo it back
    printf("%c", c);
    USB_Mainloop_Handler();
    command[i] = c;

    // in busy-wait until char received
    c = EOF;
    while ( c == EOF ) {
      USB_Mainloop_Handler();
      c = fgetc(stdin);
    }
  }

  // echo back the return
  printf("\r\n");
  USB_Mainloop_Handler();

  // buffer is full - ignore input
  if (i == COMMAND_BUFF_LEN) {
    printf("Buffer full\r\n");
    command[COMMAND_BUFF_LEN-1]='\r';
    USB_Mainloop_Handler();
    return;
  }

  command[i] = '\0';
  if (strlen(command) == 0) {
    return;
  }

  handleCommand( command );

  return;
}

void handleCommand( char *command ) {

  // Start with a char - need to change this to strings
  switch( command[0] ) {
    case( 'F'):
    case ('f'):
      printf("F or f: %c\n\r",command[0]);
      break;
    case( 'S'):
    case('s'):
      printf("S or s: %c \n\r", command[0]);
      break;
    case('B'):
    case('b'):
      printf("B or b: %c\n\r", command[0]);
      break;
    case('D'):
    case('d'):
      printf("D or d: %c\n\r", command[0]);
      break;
    default:
      printf(menuString);
    }
  USB_Mainloop_Handler();

  // Set the reference position
  //else if (sscanf(command, "r %d", &num) == 1 ||
    //sscanf(command, "R %d", &num) == 1) {
    // Keep in range
}
