#define F_CPU 16000000UL
#define UART_BAUDRATE 115200UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdio.h>

#define F_CPU 16000000UL
#define SCL_CLOCK 100000L
#define AHT20_ADDR 0x38 
uint16_t address;

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

void print_hex(char v)
{
    uint8_t counter = 0;
    char* s = "0123456789ABCDEF";
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
}

// uint8_t eeprom_read_byte(uint16_t addr)
// {
//     while (EECR & (1 << EEPE)); // Attendre la fin d'une éventuelle écriture
//     EEAR = addr;                // Charger l'adresse
//     EECR |= (1 << EERE);        // Déclencher la lecture
//     return EEDR;                // Retourner la donnée lue
// }

void eeprom_dump(void)
{
    char buffer[32];
    for (uint16_t addr = 0; addr < 96; addr += 16)
    {
        // Afficher adresse
        snprintf(buffer, sizeof(buffer), "%03X: ", addr);
        uart_printstr(buffer);

        // Afficher 16 octets
        for (uint8_t i = 0; i < 16; i++)
        {
            if (addr + i < 1024)
            {
                uint8_t val = eeprom_read_byte(addr + i);
                print_hex(val);
                uart_tx(' ');
            }
        }
        uart_printstr("\r\n");
    }
}

int 	main(void)
{
	cli();
	uart_init();
	sei();
	eeprom_dump();
	while(1);
}