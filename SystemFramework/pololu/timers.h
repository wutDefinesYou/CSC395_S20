#ifndef TIMERS_H_
#define TIMERS_H_

#include "common.h"

#define TIMER0 0
#define TIMER1 1
#define TIMER3 3

/* Set up a timer using ctc mode.
* parameter [in] timer_num : 0 (8-bit) or 1,3 (16-bit)
* parameter [in] prescaler : 1,8,64, ... as indicated in datasheet options
* parameter [in] ms_period : desired period of ISR
* parameter [out] : -1 for error in setting up. 1 otherwise.
*
*/
int SetUpTimerCTC( int timer_num, int prescaler, int ms_period );

void SetUpTimer_0(char CSbits, uint8_t match);
void SetUpTimer_1(char CSbits, uint16_t match);
void SetUpTimer_3(char CSbits, uint16_t match);

void set_timer_callback(uint8_t timer, void(*callback)(void));

#endif
