#include <avr/io.h>

#define LEDPIN PB0       // Sur Arduino Uno, la LED est sur le port B, bit 0

int main(void)
{
    // Data Direction Register du port B. Définit si chaque broche du port B est entrée ou sortie.
	// Configuration : PB0 en sortie sans alterer l'etat des autre broche (|=).
	// 0 entre. 1 sortie.
	// (1 << LEDPIN) → crée un masque binaire : 0b00000001
    DDRB |= (1 << LEDPIN);

	// Registre pour contrôler l’état logique des pins de sortie ou activer les pull-ups sur les entrées.
	// Allume la LED du portB correspondant sans alterer l'etat des autres broches.
	PORTB |= (1 << LEDPIN);

    return 0;
}
