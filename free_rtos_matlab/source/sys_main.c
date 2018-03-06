//#include "sys_pmu.h"
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






//CA Warnings Task :

void CAWarningTask(void *pvParameters)
{
	//For periodicity
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	int i =0;
	while(1)
	{

		vTaskDelayUntil(&xLastWakeTime, 20000); // 200ms

		gioSetPort(hetPORT1, gioGetPort(hetPORT1) ^ 0x80000021);

//		for (i = 0; i<100000;i++){
//
//			gioSetPort(hetPORT1, gioGetPort(hetPORT1) ^ 0x80000021);
//		}

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
		vTaskDelayUntil(&xLastWakeTime, 100000); //100 tick = 1 ms
		gioSetPort(hetPORT1, gioGetPort(hetPORT1) ^ 0x2000000);

	}
}




void main(void)
{

	/* Set high end timer GIO port hetPort pin direction to all output */
	gioSetDirection(hetPORT1, 0xFFFFFFFF);

	vimDisableInterrupt(16);

	if (xTaskCreate(CAWarningTask,"CA Task", UART_STACK_SIZE, NULL, 2, &CAWarningTaskTcb) != pdTRUE)
	{
		/* Task could not be created */
		while(1);
	}

	if (xTaskCreate(hillAssistTask,"hill Assist Task", configMINIMAL_STACK_SIZE, NULL, 3, &hillAssistTcb) != pdTRUE)
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
