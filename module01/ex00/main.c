#include <avr/io.h>

#define LEDPIN PB1       // Sur Arduino Uno, la LED est sur le port B

// Fonction de délai simple (approximation grossière)
void my_delay_ms(unsigned int ms)
{
    // Cette boucle n'est pas précise, dépend de F_CPU
    for (unsigned int i = 0; i < ms; i++)
    {
        for (unsigned int j = 0; j < 4000; j++)
        {
            __asm__ __volatile__("nop"); // commande assambleur pour "ne rien faire", pour ralentir le programme.
        }
    }
}

int main(void)
{
    // Data Direction Register du port B. Configuration : PB0 en sortie sans alterer l'etat des autre broche (|=).
    DDRB |= (1 << LEDPIN); 
    
    while (1)
    {
        // XOR inverse l'etat de la led
        PORTB ^= (1 << LEDPIN);
        my_delay_ms(500);                // minuteur
    }
}