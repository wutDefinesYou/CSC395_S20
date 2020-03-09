### CSC395 RTES System Analysis
### Due : <del>Thursday, March 12</del> Tuesday, March 24 end of day

Please submit a text file of your answers to the questions below in addition to your code for WCET analysis.

#### Introduction

The purpose of this lab is to analyze an embedded system with respect to its timing requirements, constraints, and characteristics. Unlike past labs, the primary focus here is the questions posed at the bottom of this document. Your answers should be thoughtful and complete.

#### System Analysis

Establishing the WCET (worst-case execution time) of a task is an essential part of schedulability analysis. Static analysis is a technique in which you look at the machine instructions along with flow-of-control to determine the WCET. Dynamic analysis is a technique in which you measure the time taken to execute code, either using an oscilloscope/logic analyzer or by measuring the average of the worst-case.

For this lab, use dynamic analysis to measure the WCET of the encoder task in the PCINT, the scheduler, and potentiometer.

1. Create a completely separate codebase with nothing but a timer and the code to be measured. If you place each task in a separate function, then you can change just 1 line of code in your analyzer (i.e. call the function of the task you are measuring).

2. In main(), protect ms_ticks using cli() and sei() so that access is mutually exclusive (i.e. create a mutex).

3. Structure a WCET "timer" such that you measure the time of an empty for-loop that executes as many times as it will when measuring your task (let's call that loop_time).

  The compiler will optimize away your for-loop if you do not take special measures. If you add a no-op to your loop, it should work:

  ```
  volatile uint32_t count;
  for (count=0; count < number_of_iterations; count++) {
        __asm__ __volatile__("nop");
  }
  ```

4. Measure the time of executing your task many times within the for-loop (let's call that gross_task_time).

5. Print the WCET = (gross_task_time - loop_time)/#_of_iterations. **Be careful with your print statements.** You will probably have a decimal value for your wcet time. Multiply this times 100 (or 1000), then cast to an int before printing.

> Note that you are taking an average of the worst-case. What does that mean? It means structure your test so that you are taking the maximally lengthed path through the code (the worst-case), then record the average of that worst-case path.

Express your findings in microseconds.

# Potentiometer

The potentiometers will be used to adjust the gains of the PD equation. There is sample code on Git in the ExampleCode folder. To know if it is working, you have to have communication running, therefore there are 2 examples -- one for serial and one for Lufa. Set up both potentiometers (using ADC channels 9 and 10) and confirm that they are working with the example code.

# Scheduler

Modify the task structure used with your scheduler from the previous lab to record missed deadlines as well as the number of job executions for that task. Eventually you will have the following tasks in your scheduler:

- PD controller
- Polling the Potentiometer Reading
- Trajectory Interpolation (using a buffer to manage a series of motor way points).
- Data Collection of motor values
- Communication

All tasks have the same code structure within the scheduler. The scheduler checks the release time. If it is time, it checks for a missed deadline and records a missed deadline, sets the next release time, and sets the ready flag. When you do your analysis, be sure to account for a worst case scenario in which all tasks are ready at the same time. Measure WCET for scheduling 5 tasks, but because all tasks have the same behavior in the scheduler, it doesn't matter which 5 tasks.

### QUESTIONS

1. What is the WCET for the following tasks? State whether you used the average time analysis described above or an oscilloscope/logic analyzer.

  - Scheduler with 5 tasks.
  - Encoder code (do NOT run this in the ISR(PCINT) to determine WCET)
  - Potentiometer Readings

2. What is the period of the encoder interrupt when the motor is running at 25% duty cycle and at a 90% duty cycle? Given your WCET above and these periods, what is the utilization of this task for each of the duty cycles?

3. If you run the scheduler every 1ms, what is the utilization of the scheduler? What about every 10ms? State whether or not you think 10ms is a viable period of the scheduler. Briefly justify.

4. The potentiometer readings will be polled. What do you think is a good period for this task and why? Given that period, what is the utilization of polling the potentiometers?

> Communication is an important part of the system -- both for the user to send commands to the system and for the system to report status. It is not included as part of this analysis partly because serial (using the dongle) is interrupt-driven, therefore timing it is difficult. We will work on analyzing this task later.

5. Consider the various scheduling methods used throughout the labs and discussed in class. For each method below, discuss the control over the timing of that task, responsiveness of the system, and the impact on other tasks with respect to timing.
<ol type="a">
  <li> Time-driven execution inside an ISR (e.g. toggle yellow every 250ms),
  <li> Time-driven release managed by the scheduler,
  <li> Event-driven using an external interrupt with execution inside an ISR (e.g. toggle green in response to a button press),
  <li> Event-driven using an external interrupt to release a task (e.g. setting a flag in response to a button press that signals a task in main() to execute).
  <li> Periodic polling of an event (e.g. Polling for a button press).
</ol>
