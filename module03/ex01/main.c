#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define RED PD5 // red       // Sur Arduino Uno, la LED est sur le port B
#define BLEU PD3 // bleu
#define GREEN PD6 // green

int main(void)
{
    uint8_t count = 0;
    // Data Direction Register du port D. Configuration : PD5/3/6 en sortie sans alterer l'etat des autre broche (|=).
    DDRD |= (1 << RED) | (1 << BLEU) | (1 << GREEN); 
    // PORTB ^= (1 << RED);
    while (1)
    {
        // XOR inverse l'etat de la led
        if (count == 0)
            PORTD ^= (1 << RED);
        else if(count == 1)
            PORTD ^= (1 << GREEN);
        else if (count == 2)
            PORTD ^= (1 << BLEU);
        else if (count == 3)
            PORTD ^= (1 << RED) | (1 << GREEN);
        else if(count == 4)
            PORTD ^= (1 << GREEN) | (1 << BLEU);
        else if (count == 5)
            PORTD ^= (1 << BLEU) | (1 << RED);
        else
            PORTD ^= (1 << RED) | (1 << GREEN) | (1 << BLEU);
        _delay_ms(1000);
        if (count == 0)
            PORTD ^= (1 << RED);
        else if(count == 1)
            PORTD ^= (1 << GREEN);
        else if (count == 2)
            PORTD ^= (1 << BLEU);
        else if (count == 3)
            PORTD ^= (1 << RED) | (1 << GREEN);
        else if(count == 4)
            PORTD ^= (1 << GREEN) | (1 << BLEU);
        else if (count == 5)
            PORTD ^= (1 << BLEU) | (1 << RED);
        else
            PORTD ^= (1 << RED) | (1 << GREEN) | (1 << BLEU);
        count++;
        if (count > 6)
            count = 0;
    }
}