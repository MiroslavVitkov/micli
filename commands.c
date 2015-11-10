#include "usart.h"

void cmd_reprogram(void)
{
    // TODO: obviously extract that address somewhere.
    typedef void (* fn_ptr_t) (void);
    fn_ptr_t my_ptr = (void *)0x1E00;
    my_ptr();
}


typedef void (* fn_ptr_t) (void);
typedef struct command
{
    fn_ptr_t handler;
    const char *msg;
} command_t;
const command_t Commands[] =
{
{cmd_reprogram, "reprogram"}
};


// Listens to usart until a valid command is received.
// Commands have the format:
// !command parameters
// Blocking!
// TODO: use https://en.wikipedia.org/wiki/Radix_tree
void listen_for_command(void)
{
    while (usart_receive() != '!');
    for(int i = 0; i < sizeof(Commands) / sizeof(command_t); ++i)
    {
        unsigned command_length = sizeof(Commands[i].msg);       // Without the terminating '\0'.
        for(int j = 0; j < command_length; ++j)
        {
            char in = usart_receive();
            if(in != Commands[i].msg[j]) break;                   // Continue to test next command.
            if(j == command_length)
            {
                Commands[i].handler();
                return;
            }
        }
    }
}


#include <util/delay.h>
void main(void)
{
    usart_init();

    char msg[] = "Loading bootloader in 3 seconds.";
    usart_transmit_block(msg, sizeof(msg));
    _delay_ms(3000);

    typedef void (* fn_ptr_t) (void);
    fn_ptr_t my_ptr = (void *)0x1800;
    my_ptr();


    //usart_init();
    //while(1) usart_transmit('S');
    //listen_for_command();
    //return 0;
}
