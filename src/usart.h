// Call usart_init() to make stdin and stdout point to the debug usart.
// From then on printf(), getchar() etc. functions read/write to the usart.
// Use constant NEWLINE for cross-patform compatability.
// Used hardware: TX and RX pins, USART0.
// Used interrupts: none.

#ifndef _USART_H_
#define _USART_H_

#include "usart.h"
#include "config.h"

#include <avr/io.h>
#include <util/setbaud.h>                        // F_CPU and BAUD defined in config.h
#include <stdint.h>
#include <stdio.h>

#define NEWLINE "\r\n"
enum
{
    ASCII_SOH  = 0x01,
    ASCII_EOT  = 0x04,
    ASCII_ACK  = 0x06,
    ASCII_NACK = 0x15,
};


int put_char(char c, FILE *out);
int get_char(FILE *in);
inline void usart_init(void)
{
    UBRR0H = UBRRH_VALUE;                        //Set baud rate.
    UBRR0L = UBRRL_VALUE;
#if USE_2X                                       // We are operating in asychronous mode: we need this consideration.
    UCSR0A |= (1 << U2X0);
#else
    UCSR0A &= ~(1 << U2X0);
#endif
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);              // Enable receiver and transmitter
    UCSR0C = (3<<UCSZ00);                        // Set frame format: 8 data bits, 1 stop bit, no parity aka 8N1

    // Make stdin, stdout and stderr point to the debug usart.
    static FILE fileInOutErr = FDEV_SETUP_STREAM(put_char, get_char, _FDEV_SETUP_RW);
    stdin = &fileInOutErr;
    stdout = &fileInOutErr;
}


inline void usart_transmit( unsigned char data )
{
    while ( !( UCSR0A & (1<<UDRE0)) );           // Wait for empty transmit buffer
    UDR0 = data;                                 // Put data into buffer, sends the data
}


inline char usart_receive(void)
{
    while ( !(UCSR0A & (1<<RXC0)) );             // Wait for data to be received.

/*	if(UCSR0A & (1 << FE0))
		return ERROR_USART_FRAME_ERROR;
	if(UCSR0A & (1 << DOR0))
		return ERROR_USART_DATA_OVERRUN_ERROR;
	if(UCSR0A & (1 << UPE0))
		return ERROR_USART_PARITY_ERROR;
*/
    char c = UDR0;                               // Get and return received data from buffer.
    return c;
}


inline void usart_flush( void )
{
    unsigned char dummy;
    (void) dummy;
    while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
}


#endif //#ifndef _USART_H_
