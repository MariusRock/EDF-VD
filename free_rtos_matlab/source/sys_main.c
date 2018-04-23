#include "sys_pmu.h"
#include "sys_common.h"
#include "system.h"
#include "stdint.h"
#include "os_projdefs.h"
#include "can.h"
#include "esm.h"
#include "sci.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "sys_core.h"
#include "sys_vim.h"

/* Include FreeRTOS scheduler files */
#include "FreeRTOS.h"
#include "os_task.h"
#include "os_semphr.h"
#include "os_portmacro.h"

/* Include HET header file - types, definitions and function declarations for system driver */
#include "het.h"
#include "gio.h"


/* Define Task Handles */




xTaskHandle CAWarningTaskTcb;

xTaskHandle hillAssistTcb;


xTaskHandle T1Tcb;
xTaskHandle T2Tcb;
xTaskHandle T3Tcb;

int *intvectreg = (int *) 0xFFFFFE70;
int *intindexreg = (int *) 0xFFFFFE00;

#define LOW  		0
#define HIGH 		1
#define DATA_LEN	8

#define LED_TASK 0

#define UART_ABS scilinREG //Tx ABS data
#define UART_STEER sciREG //Tx steering data
#define UART_STACK_SIZE	  ( ( unsigned portSHORT ) 256 )

//#define f_HCLK (float) 180.0 // f in [MHz]; HCLK (depends on device setup)

#define DONT_CARE 42

//uint8_t statsBuffer[40*5]; // Enough space for 5 tasks - this needs to be global, since task stack is too small

// Task #1 properties
uint32_t para1[4]; // para[0]=deadline para[1]=c_low para[2]=c_high para_[3]=xi (1=high 0=low)
unsigned int impicit_deadline1=10000;

// Task #2 properties
uint32_t para2[4];
unsigned int impicit_deadline2=100000;

uint32_t para3[4];
unsigned int impicit_deadline3=90;

uint32_t para4[4];
unsigned int impicit_deadline4=200;

uint32_t para5[4];
unsigned int impicit_deadline5=300;


// uint32 pmuCYCLE_COUNTER=0;

void make_capacity(unsigned int capacity)
{
    volatile unsigned int count = capacity;
    TickType_t xTime = xTaskGetTickCount ();
    TickType_t x;

    while(count != 0)
     {
         if(( x = xTaskGetTickCount () ) > xTime)
         {
             xTime = x;
             count--;
         }
     }
}


//CA Warnings Task :

void CAWarningTask(void *pvParameters)
{
	//For periodicity
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
	    make_capacity(300);
		vTaskDelayUntil(&xLastWakeTime,impicit_deadline1);
		gioSetPort(hetPORT1, gioGetPort(hetPORT1) ^ 0x80000021);
	}
}


//Hill Assist  :

void hillAssistTask(void *pvParameters)
{
	//For periodicity
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
	    make_capacity(30000);
		vTaskDelayUntil(&xLastWakeTime,impicit_deadline2); // 100 tick = 1 ms  or 1 tick = (1000 / configTICK_RATE_HZ)ms
		gioSetPort(hetPORT1, gioGetPort(hetPORT1) ^ 0x2000000);
	}
}

void custom_task1(void *pvParameters)
{
    //For periodicity
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        make_capacity(15);
        vTaskDelayUntil(&xLastWakeTime,impicit_deadline3); // 100 tick = 1 ms  or 1 tick = (1000 / configTICK_RATE_HZ)ms
    }
}

void custom_task2(void *pvParameters)
{
    //For periodicity
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        make_capacity(45);
        vTaskDelayUntil(&xLastWakeTime,impicit_deadline4); // 100 tick = 1 ms  or 1 tick = (1000 / configTICK_RATE_HZ)ms
    }
}

void custom_task3(void *pvParameters)
{
    //For periodicity
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        make_capacity(90);
        vTaskDelayUntil(&xLastWakeTime,impicit_deadline5); // 100 tick = 1 ms  or 1 tick = (1000 / configTICK_RATE_HZ)ms
    }
}



void initializeProfiler()
{
/* Enable PMU Cycle Counter for Profiling */

_pmuInit_();
_pmuEnableCountersGlobal_();
_pmuResetCycleCounter_();
_pmuStartCounters_(pmuCYCLE_COUNTER);

}

uint32_t getProfilerTimerCount()
{

return _pmuGetCycleCount_();

}



void main(void)
{



	/* Set high end timer GIO port hetPort pin direction to all output */
	gioSetDirection(hetPORT1, 0xFFFFFFFF);

	vimDisableInterrupt(16);


	para1[0]=impicit_deadline1; // deadline = 100 ms
	para1[1]= 4000; // c_low    = 40  ms = 4000 * 10^(-5) seconds
 	para1[2]= 7000; // c_high   = 70  ms
	para1[3]=    0; // xi       = low critical task

    para2[0]=impicit_deadline2; // deadline = 1000 ms
    para2[1]= 40000; // c_low    = 400  ms
    para2[2]= 70000; // c_high   = 700  ms
    para2[3]=     1; // xi       = high critical task

	para3[0]=impicit_deadline3; // deadline = 0.9ms
	para3[1]=20;
	para3[2]=DONT_CARE;
	para3[3]=0;

    para4[0]=impicit_deadline4; // deadline = 2ms
    para4[1]=50;
    para4[2]=DONT_CARE;
    para4[3]=0;

    para5[0]=impicit_deadline5; // deadline = 3ms
    para5[1]=100;
    para5[2]=150;
    para5[3]=1;

//    if (xTaskCreate(CAWarningTask,"CA", UART_STACK_SIZE, para1, 2, &CAWarningTaskTcb) != pdTRUE)
//    {
//        /* Task could not be created */
//        while(1);
//    }
//
//    if (xTaskCreate(hillAssistTask,"hill", configMINIMAL_STACK_SIZE,para2, 3, &hillAssistTcb) != pdTRUE)
//    {
//        /* Task could not be created */
//        while(1);
//    }

    if (xTaskCreate(custom_task1,"T1", configMINIMAL_STACK_SIZE, para3, 7, &T1Tcb) != pdTRUE)
    {
        /* Task could not be created */
        while(1);
    }
    if (xTaskCreate(custom_task2,"T2", configMINIMAL_STACK_SIZE, para4, 6, &T2Tcb) != pdTRUE)
    {
        /* Task could not be created */
        while(1);
    }
    if (xTaskCreate(custom_task3,"T3", configMINIMAL_STACK_SIZE, para5, 5, &T3Tcb) != pdTRUE)
    {
        /* Task could not be created */
        while(1);
    }






	vimEnableInterrupt(16, SYS_IRQ);

	/* Start Scheduler */
	vTaskStartScheduler();

	/* Run forever */
	while(1);
	/* USER CODE END */
}
