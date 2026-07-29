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
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    // Format : 8 bits, 1 stop, sans parité (8N1)
    // UCSR0C : USART Control and Status Register C (configuration format trame).
    // UCSZ01 (bit 2) et UCSZ00 (bit 1) : ensemble définissent la taille des données.
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Active uniquement la transmission
    // UCSR0B : USART Control and Status Register B.
    // TXEN0 (bit 3) : transmetteur
    // UCSR0B = (1<<TXEN0) active TX et met a 0 les autres bits
    UCSR0B = (1 << TXEN0);
}

void timer1_ctc_init(void)
{
    // Stop le timer pendant la config 
    TCCR1B = 0;
    TCCR1A = 0;

    // Mode CTC 
    TCCR1B |= (1 << WGM12);

    //Prescaler = 16000000 / 1024 = 15625 Hz = 64 us.
    // OCR1A = 15625 * 2 = 31250
    OCR1A = 31250;

    // TIMSK1 : Timer Interrupt Mask Register. du Timer1
    // OCIE1A : bit du registre TIMSK1
    // OCIE1A : interruption sur comparaison avec le registre OCR1A
    TIMSK1 |= (1 << OCIE1A);

    // prescaler = 1024 (CS12 = 1, CS11 = 0, CS10 = 1)
    TCCR1B |= (1 << CS12) | (1 << CS10);
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
    while (count < 11)
    {
        while (!(UCSR0A & (1 << UDRE0)));  // Attente buffer vide.
        UDR0 = '\b';
        count++;
    }
    
}

// routine a executer au moment du match
ISR(TIMER1_COMPA_vect)
{
    uart_printstr_b();
    uart_printstr("Hello Word!\n");
}

int main(void)
{   // Clear Interrupts
    cli();           // desactive interruptions globales pendant init
    uart_init();
    timer1_ctc_init();
    sei(); // Set Interrupts

    while (1)
    {
        // _delay_ms(2000);
    }
}
