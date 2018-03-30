# An EDFVD Implementation for FREERTOS

<br>This is a modification of FREERTOS V8.2.0, such that it schedules accordingly to EDFVD. </br>
It's uesed on an Hercules Safety TMS570 microcontroller</br>

Mainly all changes are made in [os_tasks.c](free_rtos_matlab/source/os_tasks.c).
A macro configUSE_EDFVD_SCHEDULER defines wether this adjustment is used.
The [Coding Standard and Style Guide](http://www.freertos.org/FreeRTOS-Coding-Standard-and-Style-Guide.html) of FREERTOS is widely kept.

***
#### What is EDFVD? 
Earliest Deadline First with Virtual Deadline (EDFVD) is a scheduling algorithm for mixed-criticality systems, which are ubiquitously in in the certification process of Real Time Operating Systems. Certification Authorities like in the automotive or aerospace industry require sometimes certain WCETs for safty critical applications. EDFVD is a good choice to achieve guarantees in budget allocation of tasks and can help this way to get software certified.</br>

---
### Related Projects in FREERTOS
* [github.com/unpsjb-rtsg/hst](https://github.com/unpsjb-rtsg/hst/) (an approach to outsource the scheduling logic to make scheduling more flexible)
* [github.com/sachin-ik/EDF_FreeRTOS](https://github.com/sachin-ik/EDF_FreeRTOS) An EDF implementation for Stellaris LM3S8962 processor(ARM cortex M3)
* [github.com/vedangp/EDF-Scheduler-for-freeRTOS](https://github.com/vedangp/EDF-Scheduler-for-freeRTOS) Another EDF implementation (I don't think that this actually works)
