 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Kareem Mohamed
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

/* UART No of data bits Which Stored in UCZ2:0 bits */
typedef enum
{
	Five_bits,Six_bits,Seven_bits,Eight_bits,Nine_bits=0x07
}no_of_data_bits;


/* UART Parity bit type Which represented in UPM1:0 bits
 * Reserved value Can't be chosen as Parity_Type
 */
typedef enum
{
	Disabled,Reserved,Even,Odd
}Parity_Type;


/* UART Frame , No of stop bits Which represented in USBS bit */
typedef enum
{
	one_bit,two_bits
}no_of_stop_bits;


/* UART Operating Mode */
typedef enum
{
	Normal_mode,Double_Speed_mode
}UART_OperatingMode;


/* Configuration Structure for UART AVR Driver Which configure:
 ** number of data bits of UART Frame
 ** Enable Parity bit (Odd or Even) or Disable Parity bit
 ** number of stop bits of UART Frame
 ** UART Operating Mode
 ** Baud rate Of UART
*/
typedef struct
{
	no_of_data_bits data;
	Parity_Type Parity_bit;
	no_of_stop_bits stop_bit;
	UART_OperatingMode Operating_mode;
	uint32 baud_rate;
}UART_ConfigType;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device Using Configuration Structure by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup UART Operating Mode
 * 4. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType* UART_Config);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint16 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
