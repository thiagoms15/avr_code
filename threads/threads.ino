constexpr int pin13 = (1<<PORTB5);
constexpr int pin12 = (1<<PORTB4);
const int timerZero = 0x06;

// Interrupcao
ISR(TIMER0_OVF_vect)
{
    static int th1 = 0, th2 = 0;
    TCNT0 = timerZero;   // reinicializa Timer0
    th1++;            // incrementa contador a cada 1ms
    th2++;            // incrementa contador a cada 1ms

    // Tempo de 100ms
    if(th1 == 100)
    {
        th1 = 0;
        PORTB ^= pin13;  // inverte o estado do PB5
    }

    // Tempo de 300ms
    if(th2 == 300)
    {
        th2 = 0;
        PORTB ^= pin12;  // inverte o estado do PB5
    }
}

int main(void)
{
    DDRB  |= pin13 | pin12;      // configura (PB5/PB4) como saida
    PORTB &= ~(pin13 | pin12);   // inicializa (PB5/PB4) em LOW

    cli();                   // desliga interrupcoes
    TCNT0  = timerZero;      // inicializa TIMER0 para contar ate 250
    TCCR0B = 0x03;           // prescaler 1:64
    TIMSK0 = 0x01;           // habilita interrupcoes do TIMER0

    sei();                   // liga interrupcoes

    // BASE DE TEMPO PRINCIPAL = 62.5E-9 x 64 x 250 = 1ms

    while(1);                // aguarda interrupcoes

    return 0;
}
