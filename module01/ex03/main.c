#include <avr/io.h>
#include <util/delay.h>

#define LEDPIN PB1       // Sur Arduino Uno, la LED est sur le port B
#define BUTTON_PLUS PD2
#define BUTTON_MINUS PD4

int main(void)
{
    // Data Direction Register du port B. Configuration : PB0 en sortie sans alterer l'etat des autre broche (|=).
    DDRB |= (1 << LEDPIN); 

    // Configure le bouton en entrée avec pull-up interne
    // ~(1 << PD2) → inverse tous les bits → 0b11111011
    DDRD &= ~((1 << BUTTON_PLUS) | (1 << BUTTON_MINUS)); // entree
    // DDRD &= ~(1 << BUTTON_MINUS);
    
    // pull-up interne. Pull-up signifie que quand le bouton n’est pas appuyé, le pin lit 1.
    // Quand on appuie → PD2 est relié à GND → lit 0.
    PORTD |= (1 << BUTTON_PLUS) | (1 << BUTTON_MINUS); // pull-up interne. 


    // TCCR1B Timer Counter Control Register B
    // Controle un partie de la configuration du mode et definit la vitesse du timer (prescaler).
    // Registre materiel qui controle le fonctionnement du Timer1 (16 bits) compte jusqu'a 2^16.

    // Waveform Generation Mode. Ce sont des bits de configuration qui determinent le mode de fonctionnement du timer.
    TCCR1B |= (1 << WGM12) | (1 << WGM13);  // Mode CTC Fast PMW.
    TCCR1A |= (1 << WGM11);
    // TCCR1A Timer Counter Control Register A 
    // Controle les sorties (OC1A, OC1B) et une partie du mode de fonctionnement (bits WGM10 et WGM11).
    
    // Compare Output Mode for Channel A, bit 1
    // Active le toggle automatique sur OC1A (pin PB1 sur Arduino Uno) et indique l'action a executer lors du match.
    // Mode non-inversé sur OC1A : Clear OC1A on compare, set at BOTTOM
    TCCR1A |= (1 << COM1A1); // COM1A1=1, COM1A0=0

    // Fréquence 1 Hz
    // F_CPU = 16 MHz
    // Le prescaler est un diviseur d’horloge appliqué avant le compteur du timer. Le timer voit donc une fréquence plus lente que la fréquence du CPU (F_CPU).
    // Chaque operation se verra ralenti, comparer a sa vitesse habituelle.
    // Pour obtenir 1 Hz :
    // f_timer = 16 MHz / 1024 = 15625 Hz.
    // TOP = ICR1 = f_timer / 1 - 1 = 15625 - 1 = 15624
    ICR1 = 15624; // duree totale
    // formule pour la valeur a comparer. Prescale * time.
    // Output Compare Register 1 A (OCR1A) registre de Timer1 qui contient la valeur a comparer du minuteur.
    // OCR1A = 15624 * 10% = 1562.
    OCR1A = ICR1 / 10; // duree hight


    // Prescaler 1024
    // Clock Select bits 12 et 10. Ces bits appartiennent au registre TCCR1B (Timer/Counter Control Register B).
    // CS12 | CS11 | CS10 |  Prescale
    //   1  |   0  |   1  |    1024
    TCCR1B |= (1 << CS12) | (1 << CS10); // demmarage du timer.

    // utiliser utiliser unsigned int  pour l'optimisation de place car tres peut dans un microcontroleur.
    unsigned int counter = OCR1A;

    while (1)
    {
        // PIND = registre de lecture des pins du port D,
        // Sert à lire l’état actuel des pins.
        // Teste si PD2 = 1
        if (!(PIND & (1 << BUTTON_PLUS)))
        {
            if (counter < ICR1)
            {
                counter += ICR1 / 10;
                if (counter > ICR1)
                {
                    counter = ICR1;
                }
                OCR1A = counter;
            }
        // Anti-rebon: Les boutons mécaniques rebondissent, plusieurs changements rapides. Il faut ajouter un delai pour filtrer ces rebons.
        _delay_ms(200);             // anti-rebond
        }

        if (!(PIND & (1 << BUTTON_MINUS)))
        {
            if (counter > (ICR1 / 10))
            {
                counter -= ICR1 / 10;
                if (counter < (ICR1 / 10))
                {
                    counter = ICR1 / 10;
                }
                OCR1A = counter;
            }
        // Anti-rebon: Les boutons mécaniques rebondissent,  plusieurs changements rapides. Il faut ajouter un delai pour filtrer ces rebons.
        _delay_ms(200);             // anti-rebond
        }

    }
    return 0;
}