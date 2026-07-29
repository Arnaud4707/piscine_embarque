#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LED PB0
#define BUTTON PD2

// interrup extern pour SW1 => PD2
// Page 79
ISR(INT0_vect)
{   // Page 80
    // The External Interrupt Control Register A contains control bits for interrupt sense control
    // verifie si on appuie
    if (EICRA & (1 << ISC01) && !(EICRA & (1 << ISC00))) { // External Interrupt Control Register A
        PORTB |= (1 << LED);
        // active la detection du relachement
        EICRA |= (1 << ISC00);  // ISC01=1, ISC00=1  
    } 
    else if (EICRA & (1 << ISC01) && (EICRA & (1 << ISC00))) {
        PORTB &= ~(1 << LED);
        // Reconfigure pour detecter l'appuie
        EICRA &= ~(1 << ISC00); // ISC01=1, ISC00=0 
    }
}

int main(void)
{
    cli(); // desactive les interrupt
    DDRB |= (1 << LED);         // LED en sortie
    DDRD &= ~(1 << BUTTON);     // Bouton en entree
    PORTD |= (1 << BUTTON);     // Pull-up activee 

    // Configure INT0 p 80
    EICRA |= (1 << ISC01); // ISC01=1 ISC00=0 => relacher
    EICRA &= ~(1 << ISC00);

    // Page 81
    // External Interrupt Mask Register
    EIMSK |= (1 << INT0);       // Active INT0

    sei(); // Active les interruptions globales

    while (1) {
        _delay_ms(10);
    }
}
