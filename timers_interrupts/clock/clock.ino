#include <avr/interrupt.h>
#include <avr/io.h>

#define F_CPU 16000000ul
#define USART_BAUD 115200ul
#define USART_UBBR_VALUE ((F_CPU/(USART_BAUD<<3))-1)

#define CMD_SIZE 7

uint8_t hh, mm, ss;

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

    t = b % 10;
    my_put_char('0' + b / 10);
    my_put_char('0' + t%10);
}

void print_clock(void)
{
    write_str("Clock: ");
    write_dec(hh);
    my_put_char(':');
    write_dec(mm);
    my_put_char(':');
    write_dec(ss);

    my_put_char('\n');
    my_put_char('\r');
}


void print_usage(void)
{
    write_str("Help:\n\r");
    write_str("t - display clock (hh:mm:ss)\n\r");
    write_str("shhmmss - set value to hour/minutes/seconds\n\r");
}

void set_timer0(void)
{
    TCCR0A = (2 << WGM00);    // Mode 2 (CTC)
    TCCR0B = (0b011 << CS00); // Start timer with prescaler 64
    TCNT0  = 0;
    OCR0A  = 250;             // Timer will count 250 -> 1ms (250x64/16MHz)
}

ISR(TIMER0_COMPA_vect)
{
    uint16_t static counter = 0;

    if(++counter == 1000)
    {
        counter = 0;
        if(++ss == 60)
        {
            ss = 0;
            if(++mm == 60)
            {
                mm = 0;
                if(++hh == 24)
                {
                    hh = 0;
                }
            }
        }
    }
}

void validate_clock(void)
{
    if(hh > 23)
    {
        hh = 0;
    }
    if(mm > 59)
    {
        mm = 0;
    }
    if(ss > 59)
    {
        ss = 0;
    }
}

uint8_t decAscii2byte(char c)
{
    return (c - '0');
}

int main(void)
{
    char cmd[CMD_SIZE];
    uint8_t index = 0;
    char c;

    // Initialise USART
    init_usart();

    set_timer0();
    TIMSK0 = (1 << OCIE0A); // Enable timer 0 compare interrupt
    sei();                  // Enable interrupts

    // Send string
    write_str("My Clock demo\n\r");

    // loop forever
    while(1)
    {
        c = my_get_char();
        if(c == 0xA) // \n ascii is 0xA
        {
            switch(cmd[0])
            {
                case 'h':
                case '?':
                    print_usage();
                    break;
                case 't':
                    print_clock();
                    break;
                case 's':
                    if(index >= 6)
                    {
                        hh = decAscii2byte(cmd[1])*10 + decAscii2byte(cmd[2]);
                        mm = decAscii2byte(cmd[3])*10 + decAscii2byte(cmd[4]);
                        ss = decAscii2byte(cmd[5])*10 + decAscii2byte(cmd[6]);
                        validate_clock();
                    }
                    print_clock();
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

