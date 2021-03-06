/*
 ================================================================================================
 Name        : Control_ECU.c
 Author      : Kareem Mohamed
 Description : Control_ECU Application Code (Source File)
 Date        : 5/11/2021
 ================================================================================================
 */


/*********************************************************************************/
#include "Control_ECU.h"
#include "external_eeprom.h"
#include "motor.h"
#include "buzzer.h"
#include "timer.h"
#include "twi.h"
#include "uart.h"
#include <avr/io.h> /* to enable the global interrupt*/
#include "util/delay.h"
/*******************************************************************************
 *                                global variables                                  *
 *******************************************************************************/
/* flag to know when the timer finish counting*/
uint8 timer_tick=0;

/* to know how many times the user entered wrong password*/
uint8 WrongPasswordCounts = 0;


/*******************************************************************************
 *                              Functions Definitions                           *
 *******************************************************************************/

/*
 * Description : This function recive the password byte by byte from HMIECU and save it into buffer
 */
void receivePassword(uint8* password)
{
	/* Wait until HMI ECU reply that it is ready to start communication */
	while(UART_recieveByte()!= HMI_ECU_READY);

	 /* Send dummy byte to tell HMIECU that Control ECU is ready */
	UART_sendByte(CONTROL_ECU_READY);

	/* Save the password which is recieved by UART Protocol byte by byte */
	for(uint8 i = 0;i < PASSWORD_LENGTH;i++){
		password[i] = UART_recieveByte();
	}
}
/*****************************************************************************************/

/*
 * Description : This Function check if the 2 entered password are matched or not
*/
uint8 match_passwords(const uint8* first_password,const uint8* second_password){
	for(int i=0;i<PASSWORD_LENGTH;i++)
		{
			if(first_password[i] == second_password[i])
				 continue;
			else
				return FALSE;
		}
		return TRUE;
}

/***********************************************************************************************/
/*
 * Description : This Function saves the correct password in External EEPROM
*/
void Save_Password_in_EEPROM(const uint8* password){
	for(uint8 i=0;i<PASSWORD_LENGTH;i++)
		{
			/* Write Every digit of password in the external EEPROM */
			EEPROM_writeByte( (PASSWORD_LOCATION) + i, password[i]);
		}
}

/*************************************************************************************************/

/*
 * Description : This Function Saves the correct password in External EEPROM if
 * the Entered Passwords Are Matched
 */
uint8 AdjustPassword_FirstTime(uint8* first_password,uint8* second_password)
{
	/* Receive the first password from HMIECU */
	receivePassword(first_password);

	/* Receive the Second password from HMIECU */
	receivePassword(second_password);

	/* Save the password in External EEPROM if the 2 Passwords are matched */
	if(match_passwords(first_password,second_password))
	{
		Save_Password_in_EEPROM(first_password);
		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}

/*******************************************************************************************/

/*
 * Description : this function tells the timer to start counting with specific configuration
 * and give it how many interrupts you want
 */
void delayWithTimer(Timer_ConfigType* timer_config,uint8 interrupt_number)
{
	/*set Timer Configuration*/
	Timer_init(timer_config);

	/* waiting until the timer count reaches the specified no_of_interrupts */
	while(timer_tick < interrupt_number);

	/* Stop The Timer*/
	Timer_DeInit(timer_config->Timer_ID);

	/*reset number of ticks*/
	timer_tick = 0;
}

/*****************************************************************************************/
/*
 * Description : it will get the option from user (Open door - Change Password)
 */
uint8 getOption_From_HMIECU(void)
{
	/* Wait until HMI ECU reply that it is ready to receive  */
	while(UART_recieveByte()!= HMI_ECU_READY);

	/* Send dummy byte to tell HMIECU that ControlECU is ready to start communication */
	UART_sendByte(CONTROL_ECU_READY);

	return UART_recieveByte();
}

/*****************************************************************************************/
/*
 * Description : This function send status to HMI ECU (DOOR_IS_OPENING - DOOR_IS_CLOSING - DOOR_IS_CLOSED)
 */
void send_status_to_HMIECU(uint8 state){

	/* Send dummy byte to tell HMIECU that ControlECU is ready to start communication */
	UART_sendByte(CONTROL_ECU_READY);

	/* Wait until HMI ECU reply that it is ready to receive  */
	while(UART_recieveByte()!= HMI_ECU_READY);

	UART_sendByte(state);
}

/*************************************************************************************************/

/*
 * Description : This function read the saved paswword from External EERROM
*/
void read_Password_in_EEPROM(uint8* password)
{
	for(uint8 i = 0; i < PASSWORD_LENGTH;i++)
	{
		EEPROM_readByte(PASSWORD_LOCATION  + i,password+i);
	}
}

/**************************************************************************************/

/*
 * Description : This function is to handle open the door request ,
 * It takes Password From HMI_ECU Then:
 * Compares this password with the one saved in EEPROM , if the 2 passwords matches:
 * it will open the door for 15 seconds and send the status for HMI ECU (Door is Opening) to display it on LCD.
 * if the user entered wrong password for three times:
 * the control ECU will Turn on buzzer alarm for 1 minute, send the status for HMI ECU (Error) ,
 * then  control ECU informs the HMI ECU  to continue the program After 1 minute.
*/
void handelOpenDoorOption(uint8* password,uint8* EEPROM_password)
{
	while(1){
		/*	TIMER1 configuration
		 * 1) F_CPU = 8Mhz  , Used prescaler for Timer1 = 1024 to get less no.of.interrupts
		 * 2) F_Timer1 = 8M/1024 ~ 7813hz => T_Count = 1/7813 = (128?sec)
		 * 3) We will get the 15 seconds in 2 interrupts.
		 * 		      1(count) --> (128?sec)    /  (62500 count)  ----> (8 sec)
		 * 4) the compare value = 62500
		*/

		Timer_ConfigType Timer1_config = {Timer1,Compare_mode,F_CPU_1024,0,0,62500};

		/* receive the password from the HMI ECU */
		receivePassword(password);

		/*get the password saved in EEPROM*/
		read_Password_in_EEPROM(EEPROM_password);

		/*check the two password*/
		if(match_passwords(password,EEPROM_password) == TRUE)
		{
			/*if they match open the door and send the status to inform the HMI ECU*/
			send_status_to_HMIECU(DOOR_IS_OPENING);

			/* Dc motor will rotates for 15 seconds to open the door */
			DcMotor_Rotate(CW,100);

			/*start counting 15 seconds*/
			delayWithTimer(&Timer1_config,2);

			/*after 15 seconds the motor will stop for 3 seconds*/
			DcMotor_Rotate(STOP,0);

			/* TIMER1 configuration */
			/*
			 * 1) F_CPU = 8Mhz Used prescaler for Timer1 = 1024 to get less no.of.interrupts
			 * 2) F_Timer1 = 8M/1024 ~ 7813hz => T_Count = 1/7813 = (128?sec)
			 * 3) We will get the 3 seconds in 1 interrupt.
			 *   1(count) --> (128?sec)    /  (23438 count)  ----> (3 sec)
			 * 4) the compare value = 23438
			 */
			Timer_ConfigType Timer1_config_three_seconds = {Timer1,Compare_mode,F_CPU_1024,0,23438};

			/* wait 3 sconds*/
			delayWithTimer(&Timer1_config_three_seconds,1);

			/* send status to HMIECU to inform it that the door is closing*/
			send_status_to_HMIECU(DOOR_IS_CLOSING);

			/*  Dc motor will rotates Anti-Clockwise for 15 seconds to close the door*/
			DcMotor_Rotate(A_CW,100);

			delayWithTimer(&Timer1_config,2);

			DcMotor_Rotate(STOP,0);

			/* to let HMI ECU knows to stop displaying DOOR_IS_CLOSING */
			send_status_to_HMIECU(DOOR_IS_CLOSED);
			break;
		}
		else
		{
			WrongPasswordCounts++;
			if(WrongPasswordCounts < 3)
			{
				send_status_to_HMIECU(PASSWORD_DISMATCH);
			}
			else
			{
				/*reset the counter to be able to use it again*/
				WrongPasswordCounts = 0;

				/*tell HMI ECU to display error message*/
				send_status_to_HMIECU(ERROR_MESSAGE);

				/*START the alarm*/
				Buzzer_on();

				/*setup the TIMER1 configuration */
				/*
				 * 1) F_CPU = 8Mhz Used prescaler for Timer1 = 1024 to get less no.of.interrupts
				 * 2) F_Timer1 = 8M/1024 ~ 7813hz => T_Count = 1/7813 = (128?sec)
				 * 3) to count 60 seconds we need 8 interrupts.
				 * 		1(count) --> (128?sec)    /  (65535 count)  ----> (8 sec)
				 * 4) we will work with timer 1 overflow mode
				 */
				Timer_ConfigType s_timer1_config = {Timer1,NormalMode,F_CPU_1024,0,0};

				/* waiting 1 minute using timer 1*/
				delayWithTimer(&s_timer1_config,7);

				/*stop the alarm*/
				Buzzer_off();

				/*tell HMI ECU to display the main menu again*/
				send_status_to_HMIECU(CONTINUE_PROGRAM);
				break;
			}
		}
	}
}


/**************************************************************************************/
/*
 * Description : This function will take 2 passwords , first one is the password which user entered ,
 * the second one is the password saved in External EEPROM and it will compare the 2 passwords :
 * if they match : save the new password
 */
void handleChangePasswordOption(uint8* password,uint8* EEPROM_password){
	while(1){

		/* receive the password from the HMI ECU */
		receivePassword(password);

		/*get the password saved in EEPROM*/
		read_Password_in_EEPROM(EEPROM_password);

		/* check the two password*/
		if(match_passwords(password,EEPROM_password) == TRUE)
		{
			/* if they match change the password in EEPROM and send the status to inform the HMIECU*/
			send_status_to_HMIECU(PASSWORD_MATCH);

			while(1)
			{
				/* save the password received from HMI ECU IN EEPROM*/
				if(AdjustPassword_FirstTime(password,EEPROM_password) == SUCCESS)
				{
					send_status_to_HMIECU(PASSWORD_MATCH);
					break;
				}
				else
				{
					send_status_to_HMIECU(PASSWORD_DISMATCH);
				}
			}

			WrongPasswordCounts = 0;
			break;
		}

		else
		{
			WrongPasswordCounts++;
			if(WrongPasswordCounts < 3)
			{
				send_status_to_HMIECU(PASSWORD_DISMATCH);
			}
			else
			{
				/*reset the wrong password counter */
				WrongPasswordCounts = 0;

				/*tell HMI ECU to display error message*/
				send_status_to_HMIECU(ERROR_MESSAGE);

				/* START the alarm*/
				Buzzer_on();
				/* TIMER1 configuration */
				/*
				 * 1) F_CPU = 8Mhz i used prescaler = 1024
				 * 2) F_Timer1 = 8M/1024 ~ 7813hz => T_Count = 1/7813 = (128?sec)
				 * 3) to count 60 seconds we need 8 interrupts.
				 * 		1(count) --> (128?sec)    /  (65535 count)  ----> (8 sec)
				 * 4) We will work with timer 1 overflow mode
				 */
				Timer_ConfigType s_timer1_config = {Timer1,NormalMode,F_CPU_1024,0,0};

				/*waiting 1 minute using timer 1*/
				delayWithTimer(&s_timer1_config,8);

				/*stop the alarm*/
				Buzzer_off();

				/*tell HMI ECU to display the main menu again*/
				send_status_to_HMIECU(CONTINUE_PROGRAM);
				break;
		}
	}
}
}
/************************************************************************************************/

/*
 * Description : this is a call back function contains a global variable which increases Every ISR Execution
 */
void handelTimer(void){
	timer_tick++;
}

/*******************************************************************************************/

/*******************************************************************************
 *                             Main Function                                   *
 *******************************************************************************/
int main(void){
	/*to hold the first_password taken from the user*/
	uint8 first_password[PASSWORD_LENGTH];

	/*to hold the second_password taken from the user*/
	uint8 second_password[PASSWORD_LENGTH];

	/*to hold the selected option from HMI ECU*/
	uint8 selected_option;

	/* UART configuration*/
	UART_ConfigType s_uart_config = {Eight_bits,Disabled,one_bit,Double_Speed_mode,UART_BAUD_RATE};

	/* I2C configuration*/
	TWI_ConfigType s_twi_config = {100000,CONTROL_ECU_ADDRESS};

	/*setting the callback function*/
	Timer1_setCallBack(handelTimer);

	/* 	calling the init functions for each driver */

	/* initialize the UART driver*/
	UART_init(&s_uart_config);

	/* initialise the I2C driver*/
	TWI_init(&s_twi_config);


	/* initialise the buzzer external driver*/
	Buzzer_init();

	/* initialise the motor external driver*/
	DcMotor_Init();

	/*set the I-bit to be able to use the timer driver*/
	SREG |= (1<<7);

	/* this loop keeps taking inputs until two matches */
	while(1){
		/*check if the passwords sent by HMI_ECU are identical and send to it the status*/
		if(AdjustPassword_FirstTime(first_password,second_password) == SUCCESS)
		{
			send_status_to_HMIECU(PASSWORD_MATCH);
			break;
		}
		else
		{
			send_status_to_HMIECU(PASSWORD_DISMATCH);
		}
	}
	/* This loop to control the selected options taken by user */
	while(1){
			/* receive the option from HMI ECU*/
			selected_option = getOption_From_HMIECU();

/* reset the wrong password counter to turn on the alarm  after three consecutive wrong passwords*/
			WrongPasswordCounts = 0;

			if(selected_option == OPEN_DOOR_OPTION)
			{
				handelOpenDoorOption(first_password,second_password);
			}

			else if(selected_option == CHANGE_PASSWORD_OPTION)
			{
				handleChangePasswordOption(first_password,second_password);
			}
	}
}
