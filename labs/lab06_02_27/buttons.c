#include "buttons.h"
#include "leds.h"

// comment this line out when you are done debugging
// #define DEBUG

uint8_t button_mask = 0;

IO_struct _buttonA = { &DDRB, &PORTB, BUTTONA, &PINB };
IO_struct _buttonC = { &DDRB, &PORTB, BUTTONC, &PINB };

INTERRUPT_struct _interruptA =
{ PCINT_BUTTONA, (1<<BUTTONA), 0, (1<<BUTTONA), empty_function, empty_function};

// INTERRUPT_struct _interruptC =
// { PCINT_BUTTONC, (1<<BUTTONC), 0, (1<<BUTTONC), empty_function, empty_function};

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

// return true if the button is pressed
int is_button_pressed(IO_struct * button) {
  return (0 == (*button->portin & (1 << button->pin)));
}

// return true if the button is pressed and released
int is_button_released(IO_struct * button) {
  if (is_button_pressed(button)) {
    // see if the change persists
    _delay_ms(10);
    return !is_button_pressed(button);
  }
  else {
    return 0;
  }
}
