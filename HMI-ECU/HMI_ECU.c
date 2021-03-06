/*
 ================================================================================================
 Name        : HMI.c
 Author      : Kareem Mohamed
 Description : HMI_ECU Application Code (Source File)
 Date        : 5/11/2021
 ================================================================================================
 */

#include "keypad.h"
#include "LCD.h"
#include "HMI_ECU.h"
#include "timer.h"
#include "uart.h"
#include "timer.h"
#include <avr/io.h> /* to enable the global interrupt*/
#include <util/delay.h>
/*******************************************************************************
 *                                global variables                            *
 *******************************************************************************/
/* flag to know when the timer finish counting*/
uint8 timer_flag=0;
/* Contains the status of the passwords sent by control ECU*/
uint8 status;

/*******************************************************************************
 *                              Functions Definitions                           *
 *******************************************************************************/

/*
 * Description : Take the password form keypad and display '*' on the screen to hide password
 */
void HMI_Adjust_And_Display_Password(uint8* password){

	for(uint8 i= 0; i <PASSWORD_LENGTH ;i++)
	{
		/* get The Pressed Key into The Password Buffer */
		password[i] = KEYPAD_getPressedKey();

		/* Display '*' on the screen */
		LCD_moveCursor(1,i+12);
		LCD_displayCharacter('*');

		/* This delay to give chance to take the pressed key in the next iteration */
		_delay_ms(500);
	}
	/* Polling Untill Enter Key is Pressed */
	while( KEYPAD_getPressedKey() != Enter_Key){}
	_delay_ms(500);
}
/*************************************************************************************/

/*
 * Description : this function send the password to control ECU if it is ready to receive
 */
void Send_Password_To_ControlECU(const uint8* password)
{
	/* Send dummy byte to tell controlECU that HMI ECU is ready */
	UART_sendByte(HMI_ECU_READY);

	 /* Wait until Control ECU reply that it is ready to receive password */
	 while(UART_recieveByte()!= CONTROL_ECU_READY);

	/* Send password to control ECU by UART driver byte by byte */
	for(uint8 i  = 0;i <PASSWORD_LENGTH;i ++)
	{
		UART_sendByte(password[i]);
	}
}

/**************************************************************************************/

/*
 * Description : Asks user to enter password then send the 2 passwords
   to control ECU to check if they match or not At the beginning of the program
*/

void Display_EnterPassword(uint8* first_password,uint8* second_password)
{
	/* Take first password */
	LCD_clearScreen();
	LCD_moveCursor(0,3);
	LCD_displayString((uint8*)"Please Enter The Password");
	/* Display '*' on the screen*/
	HMI_Adjust_And_Display_Password(first_password);


	/* Take first password */
	LCD_clearScreen();
	LCD_moveCursor(0,3);
	LCD_displayString((uint8*)"Please Re-Enter The Password");
	/* Display '*' on the screen*/
	HMI_Adjust_And_Display_Password(second_password);
	LCD_clearScreen();

	/* Send Passwords to control ECU */
	Send_Password_To_ControlECU(first_password);
	Send_Password_To_ControlECU(second_password);
}

/**************************************************************************************/

/*
 * Description : display the options menu which the user will choose from it (Open door / Change Password)
 */
void displayUserOptions(void){
	LCD_clearScreen();
	LCD_moveCursor(0,4);
	LCD_displayString((uint8*)"(+): Open Door");

	LCD_moveCursor(1,4);
	LCD_displayString((uint8*)"(-): Change Password");
}
/**************************************************************************************/
/*
 * Description : this function takes two passwords , check if they are matched or not
				and display the result on LCD
*/
void Display_EnterPassword_AndCheckStatus(uint8* first_password,uint8* second_password){
	/*	This loop doesn't terminate until the write password Entered*/
	while(1)
	{
		/* First Ask User To Enter Password */
		Display_EnterPassword(first_password,second_password);

		/* Control ECU Will Check Status Of these 2 Passwords (Matching or not)*/
		status = recievePasswordStatus();

		if(status == PASSWORD_MATCH)
		{
			LCD_clearScreen();
			LCD_moveCursor(0,4);
			LCD_displayString((uint8*)"Correct Password");
			_delay_ms(500);
			break;
		}
		else
		{
			/* Stay in While loop if 2 Passwords doesn't match */
			LCD_clearScreen();
			LCD_moveCursor(0,4);
			LCD_displayString((uint8*)"In Correct Password");
			_delay_ms(500);
		}
	}
}

/*************************************************************************************/
/*
 * Description : Display the main options ,then send the option to the control ECU.
*/
void UserOptions(uint8* a_first_password,uint8* a_second_password){

	/* Display User options (Open door / Change Password)*/
	displayUserOptions();

	/* Send user option To Control ECU */
	HMI_sendOption(HMI_takeOption());

	/* Take Actions According To User Option */
	switch(HMI_takeOption())
	{
		case OPEN_DOOR_OPTION:
		/* This Loop won't terminate until The password is correctly entered */
			while(1)
			{

			/* Ask user to enter the password */
			LCD_clearScreen();
			LCD_moveCursor(0,4);
			LCD_displayString((uint8*)"Please Enter Password : ");

			_delay_ms(200);
			/* Display '*' on the screen */
			HMI_Adjust_And_Display_Password(a_first_password);

			/* Send the password to control ECU to check it matches with the saved password or not*/
			Send_Password_To_ControlECU(a_first_password);

			/* Receive the status of the password (Matches or not) */
			status = recievePasswordStatus();

			/* Check on the status comes from Control ECU*/
	/*-->*/		if(status == DOOR_IS_OPENING )
			{
				LCD_clearScreen();

				/* Opening The door as The password Matched */
				LCD_clearScreen();
				LCD_moveCursor(0,4);
				LCD_displayString((uint8*)"Door is Opening...");

				/* Waiting Control ECU To decide when we close the door */
				status = recievePasswordStatus();

				/* If the status sent by control ECU is CLOSING_DOOR */
				if(status == DOOR_IS_CLOSING)
				{
					LCD_clearScreen();

					/* The door is closing*/
					LCD_clearScreen();
					LCD_moveCursor(0,4);
					LCD_displayString((uint8*)"closing The Door");

					/* Wait Until The door is closed to return to main menu*/
					status = recievePasswordStatus();
					break;
				}
			}
	/*-->*/		else if(status == PASSWORD_DISMATCH)
				{
				LCD_clearScreen();
				LCD_moveCursor(0,4);
				LCD_displayString((uint8*)"Wrong Password !");
				_delay_ms(500);
				/* no break as if the password is wrong for 3 times ,Alarm will turn on */
				}

				else if(status == ERROR_MESSAGE)
				{
				LCD_clearScreen();
				LCD_moveCursor(0,4);
				LCD_displayString((uint8*)"Thief !!!!!!!");
				recievePasswordStatus();/*to wait until receive continue program status*/
				/*to display the main menu again*/
				break;
				}
			}
			break;

		case  CHANGE_PASSWORD_OPTION:
			/* This Loop won't terminate until The password is correctly entered */
			while(1)
			{

				/* Tell the user to enter the old password*/
				LCD_clearScreen();
				LCD_moveCursor(0,4);
				LCD_displayString((uint8*)"Please Enter password : ");
				_delay_ms(200);

				/* Take the password from the user and display '*' */
				HMI_Adjust_And_Display_Password(a_first_password);

				/* send the password to control ECU to check it */
				Send_Password_To_ControlECU(a_first_password);

				/* Receive the status of the password (Matches or not)*/
				status = recievePasswordStatus();

				if(status == PASSWORD_MATCH)
				{
					LCD_clearScreen();
					LCD_moveCursor(0,4);
					LCD_displayString((uint8*)"Changing The Password....");
					_delay_ms(1000);
					/* Check The Entered Password */
					Display_EnterPassword_AndCheckStatus(a_first_password,a_second_password);
					break;
				}

				else if(status == PASSWORD_DISMATCH)
				{
					LCD_clearScreen();
					LCD_moveCursor(0,4);
					LCD_displayString((uint8*)"Incorrect Password !");
					_delay_ms(500);
					/* No break statement to keep asking about the password */
				}

				else if(status == ERROR_MESSAGE)
				{
					LCD_clearScreen();
					LCD_moveCursor(0,4);
					LCD_displayString((uint8*)"ERROR !");
					recievePasswordStatus();/*to wait until receive continue program status*/
					/* to display the main menu again*/
					break;
				}
			}
			break;
	}
}

/****************************************************************************************/

/*
 * Description : gets the status from control ECU of the passwords  (matching or not)
*/
uint8 recievePasswordStatus(void){

	/* Wait until the control ECU is ready to send the status */
	while(UART_recieveByte()!= CONTROL_ECU_READY);
	UART_sendByte(HMI_ECU_READY);

	/*read the status*/
	return UART_recieveByte();
}

/*************************************************************************************/
/*
 * Description : take the user's option (pressed key )
*/
uint8 HMI_takeOption(void){
	return KEYPAD_getPressedKey();
}

/***************************************************************************************/
/*
 * Description : increment the global counter for the timer (Call Back Funcation)
*/
void HMI_handleTimer(void){
	timer_flag ++;
}
/****************************************************************************************/
/*
 * Description : sends the option which user chose from the main menu to be handled in control ECU side
*/
void HMI_sendOption(uint8 option){

	UART_sendByte(HMI_ECU_READY);
	/* Wait until the control ECU is ready to recieve option */
	while(UART_recieveByte()!= CONTROL_ECU_READY);
	 UART_sendByte(option);
}

/***************************************************************************************/

/*******************************************************************************
 *                             main Function                 					 *
 *******************************************************************************/
int main(void){
	/* Contain First Password Taken From User */
	uint8 first_password_buffer[PASSWORD_LENGTH];

	/* Contain First Password Taken From User */
	uint8 second_password_buffer[PASSWORD_LENGTH];

	/* callback function of timer1 */
	Timer1_setCallBack(HMI_handleTimer);

	/* UART configuration*/
	UART_ConfigType s_uart_config = {Eight_bits,Disabled,one_bit,Double_Speed_mode,HMI_BAUD_RATE};
	UART_init(&s_uart_config);

	/* LCD Intialization */
	LCD_init();

	/* Enable (I-bit) */
	SREG |= (1<<7);

	/* Ask User to Enter The password for first time the we check if 2 passwords are matched or not*/
	Display_EnterPassword_AndCheckStatus(first_password_buffer,second_password_buffer);

	/*this while loop used to keep asking the user to choose from the main menu*/
	while(1){

		UserOptions(first_password_buffer,second_password_buffer);

	}
	return 0;
}
