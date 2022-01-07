#include <stdio.h>

#define F_CPU 16000000ul
#define USART_BAUD 115200ul
#define USART_UBBR_VALUE ((F_CPU/(USART_BAUD<<3))-1)

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

int put_char(char c, FILE *stream)
{
    // Wait if UDR is not free
    while((UCSR0A & (1<<UDRE0)) == 0);

    // Transmit data
    UDR0 = c;

    return 0;
}

int main (void)
{
    FILE uart_str;
    uart_str.put = put_char;
    uart_str.get = NULL;
    uart_str.flags = _FDEV_SETUP_WRITE;

    int i = 1;

    // Set PB5 as output
    DDRB = (1 << PB5);

    init_usart();
    stdout = &uart_str; // set the output stream

    printf("Hello World!!!\r\n");

    while(1)
    {
        // Toggle PB5
        PORTB ^= (1 << PB5);

        if (i++ % 2)
        {
            printf("%dº Tic...", i>>1);
        }
        else
        {
            printf("Tac\r\n");
        }

        _delay_ms(1000);
    }

    return (0);
}
