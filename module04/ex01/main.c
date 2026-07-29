#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LED PB1 // D2       // Sur Arduino Uno, la LED est sur le port B

uint8_t t = 0;
void    init_timer()
{

    TCCR0A |= (1 << WGM01);
    TIMSK0 |= (1 << OCIE0A);
    TCCR1B |= (1 << WGM12) | (1 << WGM13);  // Mode Fast PMW.
    TCCR1A |= (1 << COM1A1) |(1 << WGM11); // COM1A1=1, COM1A0=0

    ICR1 = 100; // duree totale 
    
    OCR1A = 0; // duree hight

    // 2 * 1024 = 2048
    // F_CPU / 2048 * ICR1 = OCR1A


    OCR0A = 77;
    TCCR0B |= (1 << CS02) | (1 << CS00);
    TCCR1B |= (1 << CS12) | (1 << CS10);
}

ISR(TIMER0_COMPA_vect)
{
    // PORTB ^= (1 << LED);
    if (OCR1A >= 100)
        t = 1;
    if (OCR1A <= 0)
        t = 0;
    if (t == 1)
        OCR1A--;
    else
        OCR1A++;
}

int main(void)
{
    cli();
    // Data Direction Register du port D. Configuration : PD5/3/6 en sortie sans alterer l'etat des autre broche (|=).
    DDRB |= (1 << LED); // sorti
    

    init_timer();

    sei();
    // Boucle vide
    while (1)
    {
    }
}