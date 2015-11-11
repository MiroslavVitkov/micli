#include "config.h"
#include "usart.h"

#include <util/delay.h>

void cmd_reprogram(void)
{
    printf("Jumping to bootloader in 3 seconds." NEWLINE);
    _delay_ms(3000);

    typedef void (* fn_ptr_t) (void);
    fn_ptr_t my_ptr = (void *)BOOTLOAD;
    my_ptr();
}


typedef void (* fn_ptr_t) (void);
typedef struct command
{
    const fn_ptr_t handler;
    const char *msg;
    const unsigned len;           // Without the terminating '\0'.
} command_t;
#define DECLARE_COMMAND(name) {cmd_##name, #name, sizeof(#name)-1},
const command_t Commands[] =
{
    DECLARE_COMMAND(reprogram)
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
        for(int j = 0; j < Commands[i].len; ++j)
        {
            char in = usart_receive();
            if(in != Commands[i].msg[j]) break;                   // Continue to test next command.
            if(j == Commands[i].len-1)
            {
                cmd_reprogram(); //Commands[i].handler();
                return;
            }
        }
    }
}


void main(void)
{
    usart_init();
    listen_for_command();
}
