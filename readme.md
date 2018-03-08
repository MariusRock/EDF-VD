# An EDFVD Implementation for FREERTOS

<br>This is a modification of FREERTOS V8.2.0, such that it schedules accordingly to EDFVD</br>
<br>Earliest Deadline First with Virtual Deadline (EDFVD) is a scheduling algorithm for mixed-criticality systems, which are ubiquitously in in the certification process of Real Time Operating Systems. Certification Authorities like in the automotive or aerospace industry require sometimes certain WCETs for safty critical applications. EDFVD can help to get software certified.</br>


Mainly all changes are made in [os_tasks.c](free_rtos_matlab/source/os_tasks.c).
A macro configUSE_EDFVD_SCHEDULER defines wether this adjustment is used.
The [Coding Standard and Style Guide](http://www.freertos.org/FreeRTOS-Coding-Standard-and-Style-Guide.html) of FREERTOS is widely kept.
