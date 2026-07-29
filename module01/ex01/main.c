#include <avr/io.h>

#define LEDPIN PB1       // Sur Arduino Uno, la LED est sur le port B

int main(void)
{
    // Data Direction Register du port B. Configuration : PB0 en sortie sans alterer l'etat des autre broche (|=).
    DDRB |= (1 << LEDPIN); 
    
    // TCCR1B Timer Counter Control Register B
    // Controle un partie de la configuration du mode et definit la vitesse du timer (prescaler).
    // Registre materiel qui controle le fonctionnement du Timer1 (16 bits) compte jusqu'a 2^16.

    // Waveform Generation Mode. Ce sont des bits de configuration qui determinent le mode de fonctionnement du timer.
    TCCR1B |= (1 << WGM12);  // WGM12 Mode CTC (Clear Timer on Compare Match).

    // TCCR1A Timer Counter Control Register A 
    // Controle les sorties (OC1A, OC1B) et une partie du mode de fonctionnement (bits WGM10 et WGM11).
    
    // Compare Output Mode for Channel A, bit 0
    // Active le toggle automatique sur OC1A (pin PB1 sur Arduino Uno) et indique l'action a executer lors du match.
    TCCR1A |= (1 << COM1A0); //Mode toggle OC1A sur comparaison. 01 bascule. commence a low, a la comparaison passe a hight

    // Fréquence 1 Hz
    // F_CPU = 16 MHz
    // Le prescaler est un diviseur d’horloge appliqué avant le compteur du timer. Le timer voit donc une fréquence plus lente que la fréquence du CPU (F_CPU).
    // Chaque operation se verra ralenti, comparer a sa vitesse habituelle.
    // Prescaler = 16000000 / 1024 = 15625 Hz = 64 us.
    
    // formule pour la valeur a comparer. Prescale * time.
    // Output Compare Register 1 A (OCR1A) registre de Timer1 qui contient la valeur a comparer du minuteur.
    // OCR1A = 15625 * 0.5 = 7812
    OCR1A = 7812;

    // Prescaler 1024
    // Clock Select bits 12 et 10. Ces bits appartiennent au registre TCCR1B (Timer/Counter Control Register B).
    // CS12 | CS11 | CS10 |  Prescale
    //   1  |   0  |   1  |    1024
    TCCR1B |= (1 << CS12) | (1 << CS10); // demmarage du timer.

    // Boucle vide
    while (1){   
    }
}