/*
 * USART communication. Hardcoded configuration. Polling mode.
 * Frame: 1 start bit, 8 data bits, 1 even parity bit, 2 stop bits
 * baud rate = 38.4k; UBRR = 12; ClockError = 0.2% 
 * mode: asynchronious normal mode

*/

#include "usart.h"
#include "Types.h"
#include <avr/io.h>

#define UBRR_FOR_BAUD_RATE_38K4_CPU_CLOCK_8M 12

void USARTinit(void){
	UBRRH = (unsigned char)(UBRR_FOR_BAUD_RATE_38K4_CPU_CLOCK_8M>>8);	//Set baud rate
	UBRRL = (unsigned char)UBRR_FOR_BAUD_RATE_38K4_CPU_CLOCK_8M;
	UCSRB = (1<<RXEN)|(1<<TXEN);						//Enable receiver and transmitter
	UCSRC = (1<<URSEL) | (3<<UCSZ0);                                        //Set frame format: 8 data bits, 1 stop bit, no parity aka 8N1
}

void	USARTtransmit( unsigned char data ){
	while ( !( UCSRA & (1<<UDRE)) );		//Wait for empty transmit buffer
	UDR = data;					//Put data into buffer, sends the data
}

void	USARTtransmitBlock(unsigned char* data, unsigned char bytes ){
	int i;
	for(i = bytes; i > 0; i--){
		USARTtransmit(*data);
		data++;
	}
}

error_t USARTreceive(unsigned char* c){
	while ( !(UCSRA & (1<<RXC)) );	//Wait for data to be received

/*	if(UCSR0A & (1 << FE0))
		return ERROR_USART_FRAME_ERROR;
	if(UCSR0A & (1 << DOR0))
		return ERROR_USART_DATA_OVERRUN_ERROR;
	if(UCSR0A & (1 << UPE0))
		return ERROR_USART_PARITY_ERROR;
*/
	*c = UDR;			//Get and return received data from buffer
	return SUCCESS;
}

void USARTflush( void ){
	unsigned char dummy;
	while ( UCSRA & (1<<RXC) ) dummy = UDR;
}



