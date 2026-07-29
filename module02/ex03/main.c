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

    // Format : 8 bits, 1 stop, sans parite (8N1)
    // UCSR0C : USART Control and Status Register C (configuration format trame).
    // UCSZ01 (bit 2) et UCSZ00 (bit 1) : ensemble définissent la taille des données.
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Active uniquement la transmission
    // UCSR0B : USART Control and Status Register B.
    // TXEN0 (bit 3) : transmetteur
    // UCSR0B = (1<<TXEN0) active TX et met a 0 les autres bits
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Pour que ISR(USART_RX_vect) fonctionne, il faut activer RX Complete Interrupt
    UCSR0B |= (1 << RXCIE0);
}

void uart_tx(char c)
{
    // UCSR0A: USART Control and Status Register A.
    // UDRE0 (bit 5) : USART Data Register Empty. vaut 1 quand le registre UDR0 est vide pour ecrire le prochain octet.
    // UDRE0 et UDR0 appartiennent au registre UCSR0A
    
    while (!(UCSR0A & (1 << UDRE0)));  // Attente buffer vide.
    if (c >= 65 && c <= 90)
        c += 32;
    else if (c >= 97 && c <= 122)
        c -= 32;
    UDR0 = c;
}

// interruption routine a executer au moment du match
ISR(USART_RX_vect)
{
    // uint8_t c = uart_rx();
    uint8_t c = UDR0;
    uart_tx(c);
}

int main(void)
{   // Clear Interrupts
    cli();           // desactive interruptions globales pendant init
    uart_init();
    sei(); // Set Interrupts

    while (1)
    {
        // _delay_ms(2000);
    }
}
