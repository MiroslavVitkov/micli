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

void 	USARTinit(void){
	UBRR0H = (unsigned char)(UBRR_FOR_BAUD_RATE_38K4_CPU_CLOCK_8M>>8);	//Set baud rate
	UBRR0L = (unsigned char)UBRR_FOR_BAUD_RATE_38K4_CPU_CLOCK_8M;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);						//Enable receiver and transmitter
	UCSR0C = (1<<USBS0)|(3<<UCSZ00)|(1<<UPM01);			//Set frame format: 8data, 2stop bit, 1 even parity bit
}

void	USARTtransmit( unsigned char data ){
	while ( !( UCSR0A & (1<<UDRE0)) );		//Wait for empty transmit buffer
	UDR0 = data;					//Put data into buffer, sends the data
}

void	USARTtransmitBlock(unsigned char* data, unsigned char bytes ){
	int i;	
	for(i = bytes; i > 0; i--){
		USARTtransmit(*data);
		data++;
	}
}

error_t USARTreceive(unsigned char* c){	
	while ( !(UCSR0A & (1<<RXC0)) );	//Wait for data to be received

/*	if(UCSR0A & (1 << FE0))
		return ERROR_USART_FRAME_ERROR;
	if(UCSR0A & (1 << DOR0))
		return ERROR_USART_DATA_OVERRUN_ERROR;
	if(UCSR0A & (1 << UPE0))
		return ERROR_USART_PARITY_ERROR;
*/
	*c = UDR0;			//Get and return received data from buffer
	return SUCCESS;
}

void USARTflush( void ){
	unsigned char dummy;
	while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
}



