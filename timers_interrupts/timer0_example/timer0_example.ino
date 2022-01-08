constexpr int pin13 = (1<<PORTB5);
const int timerZero = 0x06;

// Interrupcao
ISR(TIMER0_OVF_vect)
{
    static int baset1 = 0;
    TCNT0 = timerZero;   // reinicializa Timer0
    baset1++;            // incrementa contador a cada 1ms

    // Tempo de 500ms
    if(baset1 == 500)
    {
        baset1 = 0;
        PORTB ^= pin13;  // inverte o estado do PB5
    }
}

int main(void)
{
    DDRB  |= pin13;    // configura digital 13 (PB5) como saida
    PORTB &= ~pin13;   // inicializa digital 13 (PB5) em LOW

    cli();                   // desliga interrupcoes
    TCNT0  = timerZero;      // inicializa TIMER0 para contar ate 250
    TCCR0B = 0x03;           // prescaler 1:64
    TIMSK0 = 0x01;           // habilita interrupcoes do TIMER0

    sei();                   // liga interrupcoes

    // BASE DE TEMPO PRINCIPAL = 62.5E-9 x 64 x 250 = 1ms

    while(1);                // aguarda interrupcoes

    return 0;
}
