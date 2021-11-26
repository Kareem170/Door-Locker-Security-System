/******************************************************************************
 *
 * Module: Buzzer Driver
 *
 * File Name: buzzer.h
 *
 * Description: Header file for Buzzer driver
 *
 * Author: Kareem Mohamed
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"

/*******************************************************************************
 *                         Definitions                                   *
 *******************************************************************************/
#define BUZZER_PORT_ID PORTD_ID
#define BUZZER_PIN_ID PIN2_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/


/* This Function Sets the direction of buzzer pin as output pin */
void Buzzer_init(void);

/* This Function turns on the buzzer */
void Buzzer_on(void);

/* This Function turns off the buzzer */
void Buzzer_off(void);

#endif /* BUZZER_H_ */
