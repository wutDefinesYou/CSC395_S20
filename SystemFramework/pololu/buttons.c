// buttons.c
//
// Author: Amy Larson

#include "buttons.h"

// comment this line out when you are done debugging
// #define DEBUG

// Initialize button mask variable to 0
uint8_t button_mask = 0;

// Set up buttons A and C
IO_struct _buttonA = { &DDRB, &PORTB, &PINB, BUTTONA };
IO_struct _buttonC = { &DDRB, &PORTB, &PINB, BUTTONC };


// Setup interrupt A structure
INTERRUPT_struct _interruptA =
{ PCINT_BUTTONA, (1<<BUTTONA), 0, (1<<BUTTONA), empty_function, empty_function};


// Setup interrupt C structure
INTERRUPT_struct _interruptC =
{ PCINT_BUTTONC, (1<<BUTTONC), 0, (1<<BUTTONC), empty_function, empty_function};

// configure the system to use the button
void initialize_button(IO_struct * button) {
  CLEAR_BIT(*button->ddr, button->pin);
  // enable pull-up resistor
  SET_BIT(*button->port, button->pin);
}

/**
 * Enable button inturrupt
 */
void enable_pcint(INTERRUPT_struct *button) {
  // PCICR: Pin Change Interrupt Control Register
  // PCIE0: Pin Change Interrupt Enable Bit:
  //    Any change on any enabled PCINT7..0 can fire ISR.
  PCICR |= (1 << PCIE0);

  // PCMSK0: Pin Change Mask for Interrupt0, which is for all pins 0 through 7
  // Enable interrupts on Button A (PCINT3) and Button C (PCINT0)
  PCMSK0 |= (1 << button->pcint);

  button->enabled = 1;
  button_mask |= button->mask;
  button->prev_state = PINB & button->mask;
}


// Setup button action to link to callback function
void setup_button_action(
  INTERRUPT_struct *button, int release, void(*callback)(void)) {
    if (release) {
      button->release_fn = callback;
    } else {
      button->press_fn = callback;
    }
}

/**
 * Inturrupt Service Routine for Buttons
 */
ISR(PCINT0_vect) {
  // read port B, mask for enabled buttons
  uint8_t pinb_now = (PINB & button_mask);

  // check that pin change persists. ignore if not.
  _delay_ms(1);
  if (pinb_now ^ (PINB & button_mask)) {
    return;
  }

  #ifdef DEBUG
  // If debugging, it will be good to flash an led to know that you made
  // it into the ISR.
  debug();
  #endif

  // pinb_now holds state for only button A and C

  // store the current state of a button. 0 pressed. !0 released.
  uint8_t state = 0;

  /********** Inturrupt A **********/
  if (_interruptA.enabled) {

    // state holds button state: 0 is pressed. !0 is released
    state = (pinb_now & _interruptA.mask);

    // compare to previous state to see if button A has changed
    if (state != _interruptA.prev_state) {
      if (!state) {
        _interruptA.press_fn();
      }
      else {
        _interruptA.release_fn();
      }
      // save state for A as prev_state for interrupt A
      _interruptA.prev_state = state;
    } // end if state
  } // end if interruptA enabled

  /********** Inturrupt C **********/
  if (_interruptC.enabled) {

    state = (pinb_now & _interruptC.mask);
    // compare to previous state to see if button C has changed
    if (state != _interruptC.prev_state) {
      if (!state) {
        _interruptC.press_fn();
      }
      else {
        _interruptC.release_fn();
      }
      // save state for C as prev_state for interrupt C
      _interruptC.prev_state = state;
    } // end if
  } // end if
} // end ISR(PCINT0_vect)
