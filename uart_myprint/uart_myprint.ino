#include <avr/io.h>

#define F_CPU 16000000ul
#define USART_BAUD 115200ul
#define USART_UBBR_VALUE ((F_CPU/(USART_BAUD<<3))-1)

#define CMD_SIZE 10

uint8_t data[8];

enum print_format
{
    PRINT_HEX = 0,
    PRINT_DEC
};

void init_usart(void)
{
    // Set boudrate
    UBRR0H = (uint8_t) (USART_UBBR_VALUE >> 8);
    UBRR0L = (uint8_t) USART_UBBR_VALUE;
    UCSR0A = (1 << U2X0);

    // Set frame format to...
    UCSR0C =
        (3 << UCSZ00)   // 8 data bits
        | (0 << UPM00)  // no parity bit
        | (0 << USBS0); // 1 stop bit

    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

void my_put_char(char c)
{
    // Wait if UDR is not free
    while((UCSR0A & (1  << UDRE0)) == 0);

    // Transmit data
    UDR0 = c;
}

char my_get_char(void)
{
    // Wait until a byte has been received
    while((UCSR0A & (1 << RXC0)) == 0);

    // Return received data
    return UDR0;
}

void write_str(char* p)
{
    char c;
    while((c = *p++) != '\0')
    {
        my_put_char(c);
    }
}

void write_dec(uint8_t b)
{
    uint8_t t;

    t = b % 100;
    my_put_char('0' + b / 100);
    my_put_char('0' + t / 10);
    my_put_char('0' + t%10);
}

uint8_t hex2ascii(uint8_t b)
{
    return((b > 9) ? 'A' + b - 10 : '0' + b);
}

void write_hex(uint8_t b)
{
    my_put_char(hex2ascii(b >> 4));
    my_put_char(hex2ascii(b & 0x0F));
}

uint8_t hex2byte(char c)
{
    return ((c < 'A') ? (c - '0') : ((c & ~0x20) - 'A' + 10));
}

void print_data(print_format base)
{
    for(uint8_t n = 0; n < 8; n++)
    {
        switch(base)
        {
            case PRINT_HEX:
                write_hex(data[n]);
                break;
            case PRINT_DEC:
            default:
                write_dec(data[n]);
                break;
        }
        my_put_char(' ');
    }
    my_put_char('\n');
    my_put_char('\r');
}

int main (void)
{
    char cmd[CMD_SIZE];
    uint8_t index = 0;
    char c;

    // Initialise USART
    init_usart();

    // Send string
    write_str("My print demo\n\r");

    // loop forever
    while(1)
    {
        c = my_get_char();
        if (c == 0xA) // \n ascii is 0xA
        {
            switch (cmd[0])
            {
                case 'h':
                    print_data(PRINT_HEX);
                    break;
                case 'd':
                    print_data(PRINT_DEC);
                    break;
                case 'w':
                    data[cmd[1]-'0'] = (hex2byte(cmd[2])<<4) + hex2byte(cmd[3]);
                    print_data(PRINT_HEX);
                    break;
            }
            index = 0;
        }
        else
        {
            if (index < CMD_SIZE)
            {
                cmd[index++] = c;
            }
        }
    }

    return 0;
}
