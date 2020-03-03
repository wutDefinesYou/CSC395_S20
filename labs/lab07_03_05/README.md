##### CSC395 RTES Spring 2020

#### Lab 7 : non-PD Positional Control
#### Due Thursday March 4 at end of day

<hr>

#### 1. Example Code Functional

You will need to use these pins for the example code. You can change how you connect the motor, but then you have to change the code.

|Color | Function| Pin |
|-----|-----|-----|
| Red | motor power | motor 2, green header (B6) |
| Black | motor power | motor 2, green header (B6)
| Green | encoder GND | |
| Blue | encoder Vcc | D1 (astar 2) |
| Yellow | encoder A | B4, PCINT 4 (astar 8) |
| White | encoder B  | B5, PCINT 5 (astar 9) |

Connect the motor powers to the green header at M2A and M2B (the 2 in the middle). It does not matter which way you plug it in, but it will impact the "positive" direction as measured by the encoder.

You have to use the wall adapter to give the board enough power to run the motor. There is a power switch on the side of the board. When you turn it on, that gives power to the motors from the wall adapter. A blue LED indicates power is on.

#### 2. "UI" for Entering Reference Position

Configure your system so that whenever button A is pressed, the reference position (__a global volatile int16_t variable__) is set to +360 degrees relative to its current position, and the motor should move until it is at (or just beyond) that position. If you go to the pololu site and look up the motor #4825, you will notice "2248.86 counts per revolution" -- this is how many encoder counts there are per 360 degrees.

Configure your system so that whenever button C is pressed, the reference position is set to -360 degrees relative to its current position and the motor should move until it is at (or just beyond) that position.

Monitor the global counts of the encoder (this is your measured position), and when you have reached your reference position, turn the motor off. Please do not run your motor at more than 60% duty cycle for now as you are getting used to yor system.

#### 3. UI for Real 

If you haven't yet got serial communication working, you should get it working. We are about to start "PD control," and you want to be able to print to debug that code.

<hr>
<hr>

The above is due for Thursday. Below is where we are taking our motor control. If you finished everything above in class, keep going or turn to your neighbors and see if they need help.

#### 4. "UI" for Managing Motor Speed

Use the potentiometer to control the speed of the motor (i.e. the duty cycle). There is basic-analog code posted in the Example Code with directions. Here is a little blurb on potentiometers https://www.arduino.cc/en/tutorial/potentiometer. 

```
Do not let your motor run faster than 60% duty cycle for this exercise.
```

#### 5. Work Motor Into Your Scheduler 

Create a buffer to store reference positions that are requested by the user. For example, if the user pressed button A 2 times (for a total of +720 degrees), then pressed button C before it reached 720 degrees, the motor should still go 720, THEN reverse direction and go -360. Use your scheduler to monitor the encoder counts and determine when it is time to transition to the next reference position.

Add a polling task in your scheduler that checks the potentiometer for any changes. You want to consider the value to have changed only if it is greater than some threshold, because you will get noise and the signal will fluctuate without you moving it.
