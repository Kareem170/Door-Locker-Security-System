/******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: buzzer.c
 *
 * Description: Source file for the Keypad driver
 *
 * Author: Kareem Mohamed
 *
 *******************************************************************************/
#include "buzzer.h"
#include "gpio.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/* This Function Sets the direction of buzzer pin as output pin */
void Buzzer_init(void)
{
	GPIO_setupPinDirection(PORTD_ID,PIN2_ID,PIN_OUTPUT);
}

/* This Function turns on the buzzer */
void Buzzer_on(void)
{
	GPIO_writePin(PORTD_ID,PIN2_ID,LOGIC_HIGH);
}

/* This Function turns off the buzzer */
void Buzzer_off(void)
{
	GPIO_writePin(PORTD_ID,PIN2_ID,LOGIC_LOW);

}
