 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART AVR driver
 *
 * Author: Kareem Mohamed
 *
 *******************************************************************************/

#include "uart.h"
#include "avr/io.h" /* To use the UART Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device Using Configuration Structure by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup UART Operating Mode
 * 4. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType* UART_Config)
{
	uint16 ubrr_value = 0;

	/*
	 * U2X = 0 for Normal transmission speed
	 * U2X = 1 for double transmission speed
	*/
	UCSRA = (UCSRA & 0xFD) |((UART_Config->Operating_mode) << U2X);

	/************************** UCSRB Description **************************
	 * Disable all interrupts as we will use polling
	 * RXCIE = 0 Disable USART RX Complete Interrupt Enable
	 * TXCIE = 0 Disable USART Tx Complete Interrupt Enable
	 * UDRIE = 0 Disable USART Data Register Empty Interrupt Enable
	 * RXEN  = 1 Receiver Enable
	 * RXEN  = 1 Transmitter Enable
	 * UCSZ2 = 0 For 5 or 6 or 7 or 8-bit data mode
	 * UCSZ2 = 1 For 9-bit data mode
	 * RXB8 & TXB8 used for 9-bit data mode
	 ***********************************************************************/ 
	UCSRB = (1<<RXEN) | (1<<TXEN) | (UCSRB & 0xFB) | ( ((UART_Config->data) & 0x04));
	
	/************************** UCSRC Description **************************
	 * URSEL   = 1 The URSEL must be one when writing the UCSRC
	 * UMSEL   = 0 Asynchronous Operation
	 * UPM1:0  = parity Mode Select bits
	 * USBS    = no. of stop bits
	 * UCSZ1:0 = 5 or 6 or 7 or 8 or 9-bit data mode
	 * UCPOL   = 0 Used with the Synchronous operation only
	 ***********************************************************************/ 	
	UCSRC = (1<<URSEL) |( ((UART_Config->Parity_bit) & 0x03) << 4) | ( (UART_Config->stop_bit) << USBS)
			|( ((UART_Config->data) & 0x03) <<1);
	

	/* Calculate the UBRR register value based on UART Operating mode*/
	if(UART_Config->Operating_mode == Double_Speed_mode)
		ubrr_value = (uint16)(((F_CPU / ((UART_Config->baud_rate) * 8UL))) - 1);

	else
		ubrr_value = (uint16)(((F_CPU / ((UART_Config->baud_rate) * 16UL))) - 1);

	/* First 8 bits from the BAUD_PRESCALE inside UBRRL and last 4 bits in UBRRH*/
	UBRRH = ubrr_value>>8;
	UBRRL = ubrr_value;
}

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint16 data)
{
	/*
	 * UDRE flag is set when the Tx buffer (UDR) is empty and ready for
	 * transmitting a new byte so wait until this flag is set to one
	 */
	while(BIT_IS_CLEAR(UCSRA,UDRE)){}

	/*
	 * Put the required data in the UDR register and it also clear the UDRE flag as
	 * the UDR register is not empty now
	 */
	if((data >> 8) == 0) /* No of data bits = 8 or less*/
		UDR = data;

	else    /* number of data bits = 9*/
	{
		UCSRB = ((data &0x001) << TXB8); /*Assign LSB to TXB8 (ninth bit) */
		UDR = (data >> 1); /* Put The MS 8-bits in UDR Register */
	}
}

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void)
{
	/* RXC flag is set when the UART receive data so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,RXC)){}

	/*
	 * Read the received data from the Rx buffer (UDR)
	 * The RXC flag will be cleared after read the data
	 */
	return UDR;
}

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str)
{
	uint8 i = 0;

	/* Send the whole string */
	while(Str[i] != '\0')
	{
		UART_sendByte(Str[i]);
		i++;
	}
	/************************* Another Method *************************
	while(*Str != '\0')
	{
		UART_sendByte(*Str);
		Str++;
	}		
	*******************************************************************/
}

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str)
{
	uint8 i = 0;

	/* Receive the first byte */
	Str[i] = UART_recieveByte();

	/* Receive the whole string until the '#' */
	while(Str[i] != '#')
	{
		i++;
		Str[i] = UART_recieveByte();
	}

	/* After receiving the whole string plus the '#', replace the '#' with '\0' */
	Str[i] = '\0';
}
