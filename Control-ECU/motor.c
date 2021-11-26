/******************************************************************************
 *
 * Module: Motor
 *
 * File Name: motor.c
 *
 * Description: Source file for the DC Motor driver
 *
 * Author: Kareem Mohamed
 *
 *******************************************************************************/

#include "common_macros.h" /* To use the macros like SET_BIT */
#include "motor.h"
#include "gpio.h"
#include "pwm.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * The Function responsible for setup the direction for the two motor pins through the GPIO driver.
 */
void DcMotor_Init(void)
{
	GPIO_setupPinDirection(MOTOR_DRIVER_PORT,MOTOR_DRIVER_PIN1,PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_DRIVER_PORT,MOTOR_DRIVER_PIN2,PIN_OUTPUT);

    /*Stop at the DC-Motor at the beginning through the GPIO driver*/
	GPIO_writePin(MOTOR_DRIVER_PORT,MOTOR_DRIVER_PIN1,LOGIC_LOW);
	GPIO_writePin(MOTOR_DRIVER_PORT,MOTOR_DRIVER_PIN2,LOGIC_LOW);
}

/*
 * Description :
 * The function responsible for rotate the DC Motor CW/ or A-CW or stop the motor based on the motor state.
 * Send the required duty cycle to the PWM driver based on the required speed value.
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed)
{
	/*rotate the DC Motor CW/ or A-CW or stop the motor based on the state input state value*/
	if(state == CW)
	{
		GPIO_writePin(MOTOR_DRIVER_PORT,MOTOR_DRIVER_PIN1,LOGIC_HIGH);
		GPIO_writePin(MOTOR_DRIVER_PORT,MOTOR_DRIVER_PIN2,LOGIC_LOW);
	}
	else if(state == A_CW)
	{
		GPIO_writePin(MOTOR_DRIVER_PORT,MOTOR_DRIVER_PIN1,LOGIC_LOW);
		GPIO_writePin(MOTOR_DRIVER_PORT,MOTOR_DRIVER_PIN2,LOGIC_HIGH);
	}

	else
	{
		GPIO_writePin(MOTOR_DRIVER_PORT,MOTOR_DRIVER_PIN1,LOGIC_LOW);
		GPIO_writePin(MOTOR_DRIVER_PORT,MOTOR_DRIVER_PIN2,LOGIC_LOW);
	}

	/*decimal value for the required motor speed, it should be from 0 --> 100*/
	PWM_Timer0_Start(speed);


}


