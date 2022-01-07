// PB0 Button
// PB1 RED
// PB2 YELLOW
// PB3 GREEN
// PB4 RED
// PB5 GREEN

#define F_CPU 16000000ul
#include <avr/io.h>
#include <util/delay.h>

enum States
{
    ST_IDLE   = 0,
    ST_GREEN  = 1,
    ST_YELLOW = 2,
    ST_RED    = 3
};

int main(void)
{
  int currentTime  = 0;
  int currentState = ST_IDLE;
  const int TIMER_GREEN  = 9000;
  const int TIMER_YELLOW = 1400;
  const int TIMER_RED    = 5000;

  // Define PB1..PB5 as output
  DDRB |= (1<<PB1) | (1<<PB2) | (1<<PB3) | (1<<PB4) | (1<<PB5);

  // Test if all LEDs are ok
  PORTB |= (1<<PB1) | (1<<PB2) | (1<<PB3) | (1<<PB4) | (1<<PB5);
  //sleep
  _delay_ms(1500);
  PORTB &= ~((1<<PB1) | (1<<PB2) | (1<<PB3) | (1<<PB4) | (1<<PB5));

  while(1)
  {
      if(!(PINB & (1<<PB0)))
      {
          if((currentTime >= 4000) && (currentState == ST_GREEN))
          {
              currentState = ST_YELLOW;
              currentTime = 0;
          }
      }
      _delay_ms(200);
      currentTime = currentTime + 200;

      switch(currentState)
      {
          case ST_IDLE:
              currentState = ST_GREEN;
              break;
          case ST_GREEN:
              PORTB |= (1<<PB4) | (1<<PB3);
              PORTB &= ~((1<<PB1) | (1<<PB2) | (1<<PB5));
              if(currentTime == TIMER_GREEN)
              {
                  currentState = ST_YELLOW;
                  currentTime = 0;
              }
              break;
          case ST_YELLOW:
              PORTB |= (1<<PB4) | (1<<PB2);
              PORTB &= ~((1<<PB1) | (1<<PB3) | (1<<PB5));
              if(currentTime == TIMER_YELLOW)
              {
                  currentState = ST_RED;
                  currentTime = 0;
              }
              break;
          case ST_RED:
              PORTB |= (1<<PB5) | (1<<PB1);
              PORTB &= ~((1<<PB2) | (1<<PB3) | (1<<PB4));
              if(currentTime == TIMER_RED)
              {
                  currentState = ST_GREEN;
                  currentTime = 0;
              }
              break;
      }
  }

  return 0;
}
