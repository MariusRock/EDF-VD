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



//uint8_t statsBuffer[40*5]; // Enough space for 5 tasks - this needs to be global, since task stack is too small

// Task #1 properties
uint32_t para1[4]; // para[0]=deadline para[1]=c_low para[2]=c_high para_[3]=xi (1=high 0=low)
unsigned int impicit_deadline1=10000;

// Task #2 properties
uint32_t para2[4];
unsigned int impicit_deadline2=100000;

uint32_t para3[4];
unsigned int impicit_deadline3=2000;

uint32_t para4[4];
unsigned int impicit_deadline4=2000;

uint32_t para5[4];
unsigned int impicit_deadline5=2000;

uint32_t para6[4];
unsigned int impicit_deadline6=2000;

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
	    //make_capacity(4500);
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
	if (xTaskCreate(CAWarningTask,"CA Task", UART_STACK_SIZE, para1, 2, &CAWarningTaskTcb) != pdTRUE)
	{
		/* Task could not be created */
		while(1);
	}


    para2[0]=impicit_deadline2; // deadline = 1000 ms
    para2[1]= 40000; // c_low    = 400  ms
    para2[2]= 70000; // c_high   = 700  ms
    para2[3]=     1; // xi       = high critical task
	if (xTaskCreate(hillAssistTask,"hill Assist Task", configMINIMAL_STACK_SIZE,para2, 3, &hillAssistTcb) != pdTRUE)
	{
		/* Task could not be created */
		while(1);
	}


//	para1[0]=impicit_deadline3; // deadline = 20ms
//	para1[1]=200; // c_low  = 2 ms
//	para1[2]=300; // c_high = 3 ms
//	para1[3]=1;
//
//    para2[0]=impicit_deadline3; // deadline = 20ms
//    para2[1]=200; // c_low  = 2 ms
//    para2[2]=300; // c_high = 3 ms
//    para2[3]=1;
//
//    para3[0]=impicit_deadline3; // deadline = 20ms
//    para3[1]=200; // c_low  = 2 ms
//    para3[2]=300; // c_high = 3 ms
//    para3[3]=1;
//
//    para4[0]=impicit_deadline3; // deadline = 20ms
//    para4[1]=200; // c_low  = 2 ms
//    para4[2]=300; // c_high = 3 ms
//    para4[3]=1;
//
//    para5[0]=impicit_deadline3; // deadline = 20ms
//    para5[1]=200; // c_low  = 2 ms
//    para5[2]=300; // c_high = 3 ms
//    para5[3]=0;
//
//    para6[0]=impicit_deadline3; // deadline = 20ms
//    para6[1]=200; // c_low  = 2 ms
//    para6[2]=300; // c_high = 3 ms
//    para6[3]=0;
//
//    if (xTaskCreate(canTask,"CAN Task", configMINIMAL_STACK_SIZE, para1, 7, &canTaskTcb) != pdTRUE)
//    {
//        /* Task could not be created */
//        while(1);
//    }
//
//    if (xTaskCreate(steerTask,"Steer Task", configMINIMAL_STACK_SIZE, para2, 6, &steerTaskTcb) != pdTRUE)
//    {
//        /* Task could not be created */
//        while(1);
//    }
//
//    if (xTaskCreate(radarTask,"Radar Task", configMINIMAL_STACK_SIZE, para3, 5, &radarTaskTcb) != pdTRUE)
//    {
//        /* Task could not be created */
//        while(1);
//    }
//
//    if (xTaskCreate(vehToVehTask,"Veh to Veh Task", configMINIMAL_STACK_SIZE, para5, 5, &vehToVehTaskTcb) != pdTRUE)
//    {
//        /* Task could not be created */
//        while(1);
//    }
//
//    if (xTaskCreate(CATask,"Collision Avoidance Task", UART_STACK_SIZE, para4, 4, &CATcb) != pdTRUE)
//    {
//
//        /* Task could not be created */
//        while(1);
//    }
//
//    if (xTaskCreate(cruiseControlTask,"Cruise Control Task", configMINIMAL_STACK_SIZE, para6, 3, &cruiseControlTaskTcb) != pdTRUE)
//    {
//        /* Task could not be created */
//        while(1);
//    }





	vimEnableInterrupt(16, SYS_IRQ);

	/* Start Scheduler */
	vTaskStartScheduler();

	/* Run forever */
	while(1);
	/* USER CODE END */
}
