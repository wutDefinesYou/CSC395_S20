#include "buttons.h"
#include "leds.h"

// comment this line out when you are done debugging
// #define DEBUG

uint8_t button_mask = 0;

IO_struct _buttonA = { &DDRB, &PORTB, BUTTONA, &PINB };
IO_struct _buttonC = { &DDRB, &PORTB, BUTTONC, &PINB };

INTERRUPT_struct _interruptA =
{ PCINT_BUTTONA, (1<<BUTTONA), 0, (1<<BUTTONA), empty_function, empty_function};

INTERRUPT_struct _interruptC =
{ PCINT_BUTTONC, (1<<BUTTONC), 0, (1<<BUTTONC), empty_function, empty_function};

void empty_function() {}

// configure the system to use the button
void initialize_button(IO_struct * button) {
  CLEAR_BIT(*button->ddr, button->pin);
  // enable pull-up resistor
  SET_BIT(*button->port, button->pin);
}

void enable_pcint(INTERRUPT_struct *state) {

  // PCICR: Pin Change Interrupt Control Register
  // PCIE0: Pin Change Interrupt Enable Bit:
  //    Any change on any enabled PCINT7..0 can fire ISR.
  PCICR |= (1 << PCIE0);

  // PCMSK0: Pin Change Mask for Interrupt0, which is for all pins 0 through 7
  // Enable interrupts on Button A (PCINT3) and Button C (PCINT0)
  PCMSK0 |= (1 << state->pcint);

  state->enabled = 1;
  button_mask |= state->mask;
  state->prev_state = PINB & state->mask;
}

void setup_button_action(
  INTERRUPT_struct *state, int release, void(*callback)(void)) {
    if (release) {
      state->release_fn = callback;
    } else {
      state->press_fn = callback;
    }
}

// button A to fire the interrupt
ISR(PCINT0_vect) {
	uint8_t pinb_now = (PINB & button_mask);
  _delay_ms(10);
  if (pinb_now ^ (PINB & button_mask)) {
    return;
  }

  uint8_t state;
  if (_interruptA.enabled) {
    state = pinb_now & _interruptA.mask;

    if (state != _interruptA.prev_state) {
      // if it was pressed, call the press_fn()
      if (!state) {
        _interruptA.press_fn();
      }
      // else, call the release_fn()
      else {
        _interruptA.release_fn();
      }
      // save state as prev_state
      _interruptA.prev_state = state;
    }
  }
}

// When button A is released, blink the green LED 5 times at 2.5Hz
void A_release () {
  uint8_t i;
  for (i=0; i<5; i++) {
    _delay_ms(200);
    led_toggle(&(_GPIO_green));
    _delay_ms(200);
    led_toggle(&(_GPIO_green));
  }
}
// ISR(PCINT0_vect) {
//
//   // read port B, mask for enabled buttons
//   // pinb_now would look like xxxx (0/1)xx(0/1)
//   //                                 |      |
//   //                         pin of: A      C
//   uint8_t pinb_now = (PINB & button_mask);
//
//   // check that pin change persists. ignore if not.
//   _delay_ms(10);
//   if (pinb_now ^ (PINB & button_mask)) {
//     return;
//   }
//
//   #ifdef DEBUG
//   // If debugging, it will be good to flash an led to know that you made
//   // it into the ISR. Remeber that red and buttonC share the port, so don't
//   // use the red led. If you soldered your headers, you can also set up
//   // an LED in general IO to give yourself more options.
//   TOGGLE_BIT(*(&_green)->port, _green.pin);
//   _delay_ms(150);
//   TOGGLE_BIT(*(&_green)->port, _green.pin);
//   #endif
//
//   // ************************************************************/
//   // ***********    COMPLETE THIS PART BELOW ********************/
//   // ************************************************************/
//   // record which button(s) changed state from last interrupt
//   // remember that pinb_now holds state for only button A and C
//   uint8_t state;
//   if (_interruptA.enabled) {
//     // pinb_now is the current state for port B
//     // & mask of button A would produce xxxx (0/1)xxx
//     // so the bit related to button C is masked
//     // xxxx 1xxx : button A released
//     // xxxx 0xxx : button A pressed
//     state = pinb_now & _interruptA.mask;
//
//     // if there was a state change
//     if (state != _interruptA.prev_state) {
//       // if it was pressed, call the press_fn()
//       if (!state) {
//         _interruptA.press_fn();
//       }
//       // else, call the release_fn()
//       else {
//         _interruptA.release_fn();
//       }
//       // save state as prev_state
//       _interruptA.prev_state = state;
//     }
//   }
//     // repeat for button C
//     if (_interruptC.enabled) {
//       // same thing just masked the bit related to A, so only the state of button C is left
//       state = pinb_now & _interruptC.mask;
//
//       // if there was a state change
//       if (state != _interruptC.prev_state) {
//         // if it was pressed, call the press_fn()
//         if (!state) {
//           _interruptC.press_fn();
//         }
//         // else, call the release_fn()
//         else {
//           _interruptC.release_fn();
//         }
//         // save state as prev_state
//         _interruptC.prev_state = state;
//       }
//     }
// }
