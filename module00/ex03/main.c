#include <avr/io.h>

#define LEDPIN PB0       // Sur Arduino Uno, la LED est sur le port B
#define BUTTON PD2
#define ETAT HIGHT

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

    // Configure le bouton en entrée avec pull-up interne
    // ~(1 << PD2) → inverse tous les bits → 0b11111011

    DDRD &= ~(1 << BUTTON); // entree
    
    // pull-up interne. Pull-up signifie que quand le bouton n’est pas appuyé, le pin lit 1.
    // Quand on appuie → PD2 est relié à GND → lit 0.
    PORTD |= (1 << BUTTON); // pull-up interne. 

    // utiliser uint8_t pour l'optimisation de place car tres peut dans un microcontroleur. 
    uint8_t led_state = 0;     // État actuel de la LED : 0 = éteinte, 1 = allumée
    uint8_t last_button = 1;   // Dernier état du bouton (1 = relâché à cause du pull-up)

    while (1) {
        uint8_t current_button = (PIND & (1 << BUTTON)) ? 1 : 0; // lecture bouton, si PD2 = 1 current_button = 1 sinon 0.

        // Détecte la suite suiventer : relâché -> appuyé
        if ((last_button == 1) && (current_button == 0))
        {    
            if (led_state)
            {
                PORTB &= ~(1 << LEDPIN); // éteint
                led_state = 0;
            }
            else
            {
                PORTB |= (1 << LEDPIN);  // allume
                led_state = 1;
            }
        }

        last_button = current_button; // mémorise l'état du bouton
        // Anti-rebon: Les boutons mécaniques rebondissent (~5-20 ms) → plusieurs changements rapides. Il faut ajouter un delai pour filtrer ces rebons.
        my_delay_ms(10);                // anti-rebond
    }
    return 0;
}