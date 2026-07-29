#include <avr/io.h>

#define LEDPIN PB0       // Sur Arduino Uno, la LED est sur le port B
#define BUTTON PD2

int main(void)
{
    // Data Direction Register du port B. Configuration : PB0 en sortie sans alterer l'etat des autre broche (|=).
    DDRB |= (1 << LEDPIN); 

    // Configure le bouton en entrée avec pull-up interne
    // ~(1 << PD2) → inverse tous les bits → 0b11111011

    DDRD &= ~(1 << BUTTON); // entree
    
    // pull-up interne. Pull-up signifie que quand le bouton n’est pas appuyé, le pin lit 1.
    // Quand on appuie → PD2 est relié à GND → lit 0.
    PORTD |= (1 << BUTTON); // pull-up interne. 

    // Boucle permettant de lire les etats en continue.
	while (1)
    {
	// PIND = registre de lecture des pins du port D,
    // Sert à lire l’état actuel des pins.
    // Teste si PD2 = 1
        if (!(PIND & (1 << BUTTON)))
        {
            PORTB |= (1 << LEDPIN);   // LED allumée
        }
        else
        {
            // ~(1 << LEDPIN) → inverse tous les bits → 0b11011111. PORTB &= ~(1 << LEDPIN) → AND entre PORTB et ce masque
            // le bit de la LED devient 0, les autres bits restent inchangés.
            PORTB &= ~(1 << LEDPIN);  // LED éteinte
        }
    }

    return 0;
}
