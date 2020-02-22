##### CSC395 RTES Spring 2020

#### Lab 6 : Pseudo-Scheduling
> DUE: Thursday, February 27 before class <br>
> Submit via Moodle

This is building on the previous lab but with some major reworking to get us closer to a scheduler. The framework is essentially a nonpreemptive (i.e. non-interrupting) scheduler in that any scheduled task that is executing, runs to completion before another task can begin. Interrupts are still enabled and interrupting, but not scheduled tasks.

The ISR in which you increment ms_ticks is the scheduler. It watches for the release of the various tasks and puts them in the ready state. The main while-loop (i.e. cyclic executive) functions as the server and calls or executes the "ready" task(s). No priority has been provided for the tasks and it is not explicitly maintained as part of the scheduling (yet). Instead, you can control priority implicitly by the structure of the "server" through the use of if-statements.

<hr>

#### Create Task Structure

Create a struct that holds 1) a task ready flag (uint8_t), 2) the next release time (uint32_t), and 3) the period of the task (uint32_t). For now, you will create a separate variable of this type for each task you want to run. In future assignments (or now if you want to), you will gather all the tasks into a single table which will function as the ready queue.

In the ISR in which you update ms_ticks, at each tick, the "scheduler" will check if any of the tasks are ready for release based on the "next release time". If it is, then the scheduler sets the task ready flag and resets the next release time (to be "period" milliseconds from now).

In the ISR, you might have this ...

```C
++ms_ticks;
if (ms_ticks >= taskRed.nextRelease) {
  taskRed.ready = 1;
  taskRed.nextRelease = ms_ticks + taskRed.period;
}
/* if .... check the rest of the tasks */
```


In the main loop, each task checks its flag to see if it is set, which indicates it is time for it to execute. If it is, the appropriate function is called (for example, a function to toggle an LED), then the ready flag is reset to 0.

In the while loop, you might have this ...

```C
while(1) {
  if (taskRed.ready) {
    taskRedFunction();   // OR if you add to struct, taskRed.fn()
    taskRed.ready = 0;
  }
  /* if ...   check for the rest of the tasks */
}
```

#### The Tasks Being Scheduled

1. Toggle Red every 500ms.
2. Toggle Yellow every 250ms.
3. Poll for the state of button A every 1000ms, and if the button is being pressed, then increment a counter. Or if you want to do something more interesting, you can add the potentiometer or the pressure sensor or some other sensor (I have many) and poll for its state.
4. Every 2 seconds, poll for a **key** press (key 'f') to modify the PWM signal rate. If you do not have communication working, then you can use button C by setting a callback function that sets a flag that is polled by this task.
5. Every 3 seconds, print the value of the counter that tracks the number of button presses.

#### Tasks Not Being Scheduled

1. The scheduler -- 1 ms timer interrupt that increments ms_ticks and checks for the release of tasks.
2. The LED toggling using the PWM signal.
3. The toggling of the LED whenever Button A is released.
4. Possibly a Button C release that sets a flag to indicate the PWM frequency should be modified.
5. If you are using the dongle, the communication is interrupt driven.
