/******************************************************************************
 *
 * Module: Timer Driver
 *
 * File Name: Timer.h
 *
 * Description: Header file for the Timer AVR driver
 *
 * Author: Kareem Mohamed
 *
 *******************************************************************************/

#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

/* Choose one of the 3 AVR timers */
typedef enum
{
	Timer0,Timer1,Timer2
}Timer_Number;


/* AVR Timer Modes */
typedef enum
{
	NormalMode,Compare_mode
}Timer_Modes;

/*
 *  AVR Timer Prescalar For Timer 0,1
*/
typedef enum
{
	No_Clock,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024,Ext_CLK_Falling_Edge,Ext_CLK_Rising_Edge
}Timer_Prescalar;

/*
 *  AVR Timer Prescalar For Timer2 as it has different prescalar than TIMERS 0,1
*/
typedef enum
{
	no_clock,f_cpu_clock,f_cpu_8,f_cpu_32,f_cpu_64,f_cpu_128,f_cpu_256,f_Cpu_1024
}Timer2_Prescalar;


/* Configuration Structure for AVR Timer Driver Which configure:
 1- The timer ID we want to use (0,1,2)
 2- The Timer driver modes (Normal,Compare)
 3- The Timer Prescalar
 4- The Timer Initial value
 5- The Timer Compare Value
*/
typedef struct
{
	Timer_Number Timer_ID;
	Timer_Modes Timer_mode ;
	Timer_Prescalar Prescalar;
	Timer2_Prescalar timer2_prescalar;
	uint16 Timer_Initial_value;
	uint16 Timer_Compare_value;
}Timer_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/


/* Description:
 *  This Function Configures:
 1- The Timer Id
 2- The Timer Mode (Normal , Compare)
 3- The Timer Prescalar
 4- The Timer Initial Value That will start counting from it
 5- The The Timer Compare Value (In Compare Mode Only)
*/
void Timer_init(const Timer_ConfigType* Timer_Config);


/*
 * Description: Function to set the Call Back function address of Timer0.
 */
void Timer0_setCallBack(void(*T0_ptr)(void));

/*
 * Description: Function to set the Call Back function address of Timer1.
 */
void Timer1_setCallBack(void(*T1_ptr)(void));

/*
 * Description: Function to set the Call Back function address of Timer2.
 */
void Timer2_setCallBack(void(*T2_ptr)(void));

/*
 * Description: Function to disable the Timer driver
 */
void Icu_DeInit(Timer_Number Timer_ID);


#endif /* TIMER_H_ */
