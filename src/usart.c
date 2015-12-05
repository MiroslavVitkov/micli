#include "usart.h"


int put_char(char c, FILE *out)
{
    usart_transmit(c);
    return 0;                                    //SUCCESS
}


int get_char(FILE *in)
{
    unsigned char c;
    c = usart_receive();
    return (int)c;                               //SUCCESS
}
