#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define RED PD5 // red       // Sur Arduino Uno, la LED est sur le port B
#define BLEU PD3 // bleu
#define GREEN PD6 // green

uint8_t counter = 0;

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    OCR0B = r; // RED
    OCR0A = g; // GREEN
    OCR2B = b; // BLUE
}

void wheel(uint8_t pos)
{
    pos = 255 - pos;
    if (pos < 85)
    {
        set_rgb(255 - pos * 3, 0, pos * 3);
    }
    else if (pos < 170)
    {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);
    }else
    {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);
    }
}

void    init_rgb()
{
    // Fast PMW - non inverser
    // Timer0 D3 Bleu
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);
    // prescaler 1024
    TCCR0B |= (1 << CS02) | (1 << CS00); 

    // === Timer2 (pour BLUE) ===
    // Fast PWM, non-inverse sur OC2B
    TCCR2A |= (1 << COM2B1) | (1 << WGM20) | (1 << WGM21);
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
}

int main(void)
{
    // Data Direction Register du port D. Configuration : PD5/3/6 en sortie sans alterer l'etat des autre broche (|=).
    DDRD |= (1 << RED) | (1 << BLEU) | (1 << GREEN); 
    
    init_rgb();

    // Boucle vide
    while (1)
    {
        wheel(counter);
        counter++;
        // _delay_ms(10);
        _delay_ms(30);
        if (counter == 256)
            counter = 0;
    }
}