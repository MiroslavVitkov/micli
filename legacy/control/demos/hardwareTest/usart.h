/*
 * USART communication. Hardcoded configuration. Polling mode.
 * Frame: 1 start bit, 8 data bits, 1 even parity bit, 2 stop bits
 * baud rate = 38.4k; UBRR = 12; ClockError = 0.2%
 * mode: asynchronious normal mode

*/

#ifndef LIB_CONTOL_DEMOS_USART_H_
#define LIB_CONTOL_DEMOS_USART_H_

#include "Types.h"

typedef enum{
	OK = 64,
	ERROR_USART_FRAME_ERROR,
	ERROR_USART_DATA_OVERRUN_ERROR,
	ERROR_USART_PARITY_ERROR,
}hardwareError;

void USARTinit(void);
void	USARTtransmit( unsigned char data );
void	USARTtransmitBlock(unsigned char* data, unsigned char bytes );
error_t USARTreceive(unsigned char*);//blocking
void USARTflush( void );

#endif //#ifndef LIB_CONTOL_DEMOS_USART_H_
