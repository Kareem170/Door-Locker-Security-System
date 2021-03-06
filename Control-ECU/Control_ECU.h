/*
 ================================================================================================
 Name        : Control_ECU.h
 Author      : Kareem Mohamed
 Description : Control_ECU Application Code (Header File)
 Date        : 5/11/2021
 ================================================================================================
 */

#ifndef CONTROL_ECU_H_
#define CONTROL_ECU_H_

#include "std_types.h"
/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define CONTROL_ECU_ADDRESS 0x44
#define PASSWORD_LENGTH 5
#define PASSWORD_LOCATION  0x000
#define UART_BAUD_RATE 9600

/********************* These defintions to sync between the 2 ECU **********************/
#define CONTROL_ECU_READY 0x10
#define HMI_ECU_READY 0x20
#define DOOR_IS_OPENING  0X22
#define DOOR_IS_CLOSING  0X33
#define DOOR_IS_CLOSED 0X44
#define ERROR_MESSAGE 0xFF
#define PASSWORD_MATCH 0x11
#define PASSWORD_DISMATCH 0x00
#define CONTINUE_PROGRAM 0X55
#define OPEN_DOOR_OPTION '+'
#define CHANGE_PASSWORD_OPTION '-'
/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/
/*
 * Description : This function recive the password byte by byte from HMIECU and save it into buffer
 */
void receivePassword(uint8* password_ptr);

/*
 * Description : This Function saves the correct password in External EEPROM
*/
void Save_Password_in_EEPROM(const uint8* password_ptr);

/*
 * Description : it will get the option from user (Open door - Change Password)
 */
uint8 getOption_From_HMIECU(void);

/*
 * Description : This function send status to HMI ECU (DOOR_IS_OPENING - DOOR_IS_CLOSING - DOOR_IS_CLOSED)
 */
void send_status_to_HMIECU(uint8 state);

/*
 * Description : This function read the saved paswword from External EERROM
*/
void read_Password_in_EEPROM(uint8* password_ptr);

/*
 * Description : This function is to handle open the door request ,
 * It takes Password From HMI_ECU Then:
 * Compares this password with the one saved in EEPROM , if the 2 passwords matches:
 * it will open the door for 15 seconds and send the status for HMI ECU (Door is Opening) to display it on LCD.
 * if the user entered wrong password for three times:
 * the control ECU will Turn on buzzer alarm for 1 minute, send the status for HMI ECU (Error) ,
 * then  control ECU informs the HMI ECU  to continue the program After 1 minute.
*/
void handelOpenDoorOption(uint8* password_ptr,uint8* EEPROM_password);

/*
 * Description : this is a call back function contains a global variable which increases Every ISR Execution
 */
void handelTimer(void);

/*
 * Description : This function will take 2 passwords , first one is the password which user entered ,
 * the second one is the password saved in External EEPROM and it will compare the 2 passwords :
 * if they match : save the new password
 */
void handelChangePasswordOption(uint8* password_ptr,uint8* EEPROM_password);

/*
 * Description : This Function Saves the correct password in External EEPROM if
 * the Entered Passwords Are Matched
 */
uint8 AdjustPassword_FirstTime(uint8* first_password,uint8* second_password);

/*
 * Description : This Function check if the 2 entered password are matched or not
*/
uint8 match_passwords(const uint8* a_first_password_ptr,const uint8* a_second_password_ptr);

#endif /* CONTROL_ECU_H_ */
