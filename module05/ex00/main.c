#define F_CPU 16000000UL
#define UART_BAUDRATE 115200UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

void uart_init(void)
{
    // Calcul du baud rate 
    uint16_t ubrr = ((F_CPU / (8UL * UART_BAUDRATE)) - 1);

    // USART Control and Status Register A.
    // Il contient les indicateurs d’etat et flags lies a la transmission et la reception.
    // Mode double vitesse (important à 16 MHz). divise de la vitesse de l'horloge par 8.
    UCSR0A = (1 << U2X0);

    // Baudrate
    // UBRR0H et UBRR0L sont les deux octets du registre UBRR0 (USART Baud Rate Register).
    UBRR0H = (uint8_t)(ubrr >> 8); // poids fort
    UBRR0L = (uint8_t)ubrr; // poids faible

    // Format : 8 bits, 1 stop, sans parité (8N1)
    // UCSR0C : USART Control and Status Register C (configuration format trame).
    // UCSZ01 (bit 2) et UCSZ00 (bit 1) : ensemble definissent la taille des donnees. 1octe
    // UCSR0C | UCSZ02 | UCRZ01 | UCRZ00 |  taille
    //  011   |    0   |    1   |    1   |    8bits
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Active uniquement la transmission
    // UCSR0B : USART Control and Status Register B.
    // TXEN0 (bit 3) : transmetteur
    // UCSR0B = (1<<TXEN0) active TX et met a 0 les autres bits 
    UCSR0B = (1 << TXEN0);
}

// Figure 24-7.

void ADC_init(void) {
    // Référence = AVCC (bit REFS0 = 1, REFS1 = 0)
    // Résolution 8 bits -> ADLAR = 1 (justifier à gauche)
    // Canal ADC0 -> MUX bits = 0000
    // 24.9.1
    // MUX = 0000 = ADC0 = PC0 = RV1
    ADMUX = (1 << REFS0) | (1 << ADLAR) | (0b00000000);

    // Activer l’ADC (ADEN = 1)
    // Choisir un prescaler pour une fréquence ADC ~125 kHz (si F_CPU = 16 MHz → diviseur = 128)
    // Table 24.9.2
    ADCSRA = (1 << ADEN); // ADEN: ADC Enable
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // divise par 128
}

uint8_t ADC_read_RV(void)
{
    // 24.9.2
    // Demarrer la conversion
    ADCSRA |= (1 << ADSC);

    // Attendre la fin de la conversion
    while (ADCSRA & (1 << ADSC));
    // 24.9.3
    // lire ADCH seulement
    return ADCH;
}

void uart_tx(char c)
{
    // UCSR0A: USART Control and Status Register A.
    // UDRE0 (bit 5) : USART Data Register Empty → vaut 1 quand le registre UDR0 est vide pour ecrire le prochain octet.
    while (!(UCSR0A & (1 << UDRE0)));  // Attente buffer vide.
    UDR0 = c;
}

void uart_printstr(char* c)
{
    uint8_t count = 0;
    // UCSR0A: USART Control and Status Register A.
    // UDRE0 (bit 5) : USART Data Register Empty → vaut 1 quand le registre UDR0 est vide pour ecrire le prochain octet.
    while (c[count])
    {
        // UDRE0 et UDR0 appartiennent au registre UCSR0A
        while (!(UCSR0A & (1 << UDRE0)));  // Attente buffer vide.
        UDR0 = c[count];
        count++;
    }
    
}

void uart_printstr_b()
{
    uint8_t count = 0;
    // UCSR0A: USART Control and Status Register A.
    // UDRE0 (bit 5) : USART Data Register Empty → vaut 1 quand le registre UDR0 est vide pour ecrire le prochain octet.
    while (count < 2)
    {
        while (!(UCSR0A & (1 << UDRE0)));  // Attente buffer vide.
        UDR0 = '\b';
        count++;
    }
    
}

void convert(uint8_t v)
{
    uint8_t counter = 0;
    char* s = "0123456789abcdef";
    uint8_t v_haut = v / 16;
    uint8_t v_bas = v % 16;

    while (s[counter])
    {
        if (counter == v_haut)
            uart_tx(s[counter]);
        counter++;
    }
    counter = 0;
    while (s[counter])
    {
        if (counter == v_bas)
            uart_tx(s[counter]);
        counter++;
    }
    uart_tx('\n');
    uart_printstr_b();
}

void init_timer(void) {
    // Mode CTC, prescaler 1024, F_CPU = 16MHz
    // OCR1A = F_CPU / prescaler / freq - 1
    // 16M / 1024 *0.02 = 312
    TCCR1B |= (1 << WGM12); // CTC
    OCR1A = 312;
    TCCR1B |= (1 << CS12) | (1 << CS10); // prescaler 1024
    TIMSK1 |= (1 << OCIE1A); // enable interrupt compare A
}

ISR(TIMER1_COMPA_vect) {
    convert(ADC_read_RV());
}

int main(void)
{
    cli();
    uart_init();
    ADC_init();
    init_timer();
    sei();

    while (1);
}
