/******************************************************************************
 *
 * Module: DC Motor
 *
 * File Name: motor.h
 *
 * Description: Header file for the DC Motor driver
 *
 * Author: Kareem Mohamed
 *
 *******************************************************************************/

#ifndef MOTOR_H_
#define MOTOR_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define MOTOR_DRIVER_PORT  PORTB_ID

#define MOTOR_DRIVER_PIN1  PIN0_ID

#define MOTOR_DRIVER_PIN2  PIN1_ID

#define MOTOR_SPEED_CONTROL_PIN  PIN3_ID

/*******************************************************************************
 *                             Static Configurations                            *
 *******************************************************************************/

typedef enum
{
	STOP = 0X00 , CW  = 0X01 , A_CW = 0X02
}DcMotor_State;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * The Function responsible for setup the direction for the two motor pins through the GPIO driver.
 */
void DcMotor_Init(void);

/*
 * Description :
 * The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor based on the motor state.
 * Send the required duty cycle to the PWM driver based on the required speed value.
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed);


#endif /* MOTOR_H_ */
