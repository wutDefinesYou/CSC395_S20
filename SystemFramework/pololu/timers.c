#include "timers.h"
#include "error.h"

void (*timer0_compa_callback)(void) = empty_function;
void (*timer1_compa_callback)(void) = empty_function;
void (*timer3_compa_callback)(void) = empty_function;

void (*press_fn)(void);

/* Return clock-select bits based on prescaler option as defined in datasheet */
char GetCSBits(int prescaler);

void set_timer_callback(uint8_t timer, void(*callback)(void)) {
  switch (timer) {
    case(TIMER0) : timer0_compa_callback = callback; break;
    case(TIMER1) : timer1_compa_callback = callback; break;
    case(TIMER3) : timer3_compa_callback = callback; break;
    default: error();
  }
}

/* Set up a timer using ctc mode.
 * parameter [in] timer_num : 0 (8-bit) or 1,3 (16-bit)
 * parameter [in] prescaler : 1,8,64, ... as indicated in datasheet options
 * parameter [in] ms_period : desired period of ISR
 * parameter [out] : -1 for error in setting up. 1 otherwise.
 */
int SetUpTimerCTC( int timer_num, int prescaler, int ms_period ) {
  if ((TIMER0 != timer_num) && ((TIMER1 != timer_num) && (TIMER3 != timer_num))) {
    return error();
  }

  // determine the match value for the desired period given prescaler
  // @TODO check that this is an integer value -- if not give warning.
  uint32_t match32 = (F_CPU*(ms_period/1000.0))/(prescaler*1.0) - 1;

  // Check that the match fits in its counter register (8-bit or 16-bit)
  if ((TIMER0==timer_num) && (match32 > 255)) {
    return error();
  } else if (match32 > 65535) {
    return error();
  }

  // Get the clock-select bits for the chosen prescaler
  char CSbits = GetCSBits(prescaler);

  switch(timer_num) {
    case(TIMER0) : SetUpTimer_0(CSbits, (uint8_t) match32); break;
    case(TIMER1) : SetUpTimer_1(CSbits, (uint16_t) match32); break;
    case(TIMER3) : SetUpTimer_3(CSbits, (uint16_t) match32); break;
    default : return error();
  }
  return 1;
}

void SetUpTimer_0(char CSbits, uint8_t match) {

  // Clear the Control Registers for Timer 0
  TCCR0A = 0;
  TCCR0B = 0;

  // CTC mode is 2 = 010
  TCCR0A |= (1 << WGM01);

  // Clock Select Bits are bits 00000XXX in TCCR0B
  TCCR0B |= CSbits;

  // CTC uses OCR0A as top / match
  OCR0A = match;

  // Enable the Interrupt on the OCR0A match
  TIMSK0 |= (1<<OCIE0A);
}

ISR(TIMER0_COMPA_vect) {
  timer0_compa_callback();
}

void SetUpTimer_1(char CSbits, uint16_t match) {

  // Clear the Control Registers for Timer 0
  TCCR1A = 0;
  TCCR1B = 0;

  // CTC mode is 4 = 0100
  TCCR1B |= (1 << WGM12);

  // Clock Select Bits are bits 00000XXX in TCCR0B
  TCCR1B |= CSbits;

  // CTC uses OCR0A as top / match
  OCR1A = match;

  // Enable the Interrupt on the OCR1A match
  TIMSK1 |= (1<<OCIE1A);
}

ISR(TIMER1_COMPA_vect) {
  timer1_compa_callback();
}

void SetUpTimer_3(char CSbits, uint16_t match) {

  // Clear the Control Registers for Timer 0
  TCCR3A = 0;
  TCCR3B = 0;

  // CTC mode is 4 = 0100
  TCCR3B |= (1 << WGM32);

  // Clock Select Bits are bits 00000XXX in TCCR0B
  TCCR3B |= CSbits;

  // CTC uses OCR0A as top / match
  OCR3A = match;

  // Enable the Interrupt on the OCR3A match
  TIMSK3 |= (1<<OCIE3A);
}

ISR(TIMER3_COMPA_vect) {
  timer3_compa_callback();
}

char GetCSBits(int prescaler) {
  // homegrown dictionary.
  // key: prescaler. value: clock select bits value
  switch(prescaler) {
    case(1) : return 1; break;
    case(8) : return 2; break;
    case(64) : return 3; break;
    case(256) : return 4; break;
    case(1024) : return 5; break;
    default : error(); return 1;
  }
}
