/*
 ================================================================================================
 Name        : HMI_ECU.h
 Author      : Kareem Mohamed
 Description : HMI_ECU Application Code (Header File)
 Date        : 5/11/2021
 ================================================================================================
 */
#ifndef HMI_ECU_H_
#define HMI_ECU_H_


/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define PASSWORD_LENGTH 5  			/* Password Length */
#define HMI_BAUD_RATE 9600	 		/* UART Baud Rate */
#define OPEN_DOOR_OPTION '+'		/* Open door option */
#define CHANGE_PASSWORD_OPTION '-'	/* Change Password Option */

/********************* These defintions to sync between the 2 ECU **********************/
#define CONTROL_ECU_READY 0x10
#define HMI_ECU_READY 0x20
#define PASSWORD_MATCH 0x11
#define PASSWORD_DISMATCH 0x00
#define ERROR_MESSAGE 0xFF
#define DOOR_IS_OPENING 0X22
#define DOOR_IS_CLOSING 0X33
#define DOOR_CLOSED 0X44
#define Enter_Key 13
/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
/*
 * Description : Take the password form keypad and display '*' on the screen to hide password
 */
void HMI_Adjust_And_Display_Password(uint8* password);

/*
 * Description : Send the password to control ECU
 */
void Send_Password_To_ControlECU(const uint8* password);

/*
 * Description : Take the passwords twice and send them to control ECU to check if they match or not
*/
void Display_EnterPassword(uint8* first_password,uint8* second_password);

/*
 * Description : display the options menu which the user will choose from it (Open door / Change Password)
 */
void displayUserOptions(void);

/*
 * Description : this function takes two passwords , check them and display status on screen
*/
void Display_EnterPassword_AndCheckStatus(uint8* a_first_password_ptr,uint8* a_second_password_ptr);

/*
 * Description : Display the main options ,then send the option to the control ECU.
*/
void UserOptions(uint8* a_first_password_ptr,uint8* a_second_password_ptr);

/*
 * Description : increment the global counter for the timer (Call Back Funcation)
*/
void HMI_handleTimer(void);

/*
 * Description : sends the option which user chose from the main menu to be handled in control ECU side
*/
void HMI_sendOption(uint8 option);

/*
 * Description : take the user's option (pressed key )
*/
uint8 HMI_takeOption(void);

/*
 * Description : gets the status from control ECU of the passwords  (matching or not)
*/
uint8 recievePasswordStatus(void);

#endif /* HMI_ECU_H_ */
