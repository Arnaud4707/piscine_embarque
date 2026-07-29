#define F_CPU 16000000UL
#define UART_BAUDRATE 115200UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdint.h>

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

void convert(uint16_t v)
{
    uint8_t counter_digit = 0;
    uint16_t res = v;
    
    if (v == 0)
        uart_tx('0');
    while (v > 0)
    {
        counter_digit++;
        v = v / 10;
    }
    v = res;
    while (counter_digit > 0)
    {
        uint8_t i = 1;
        v = res;
        while (i < counter_digit)
        {
            v = v / 10;
            i++;
        }
        uart_tx((v % 10) + '0');
        counter_digit--;
    }
}

void i2c_init(void)
{
    // Formule du datasheet :
    // SCL frequency = F_CPU / (16 + 2*TWBR*Prescaler)
    // On prend Prescaler = 1 (TWPS = 0)
    TWSR = 0x00; // Bits TWPS1..0 = 00 => prescaler = 1

    // Calcul : TWBR = ((F_CPU / SCL_CLOCK) - 16) / 2
    TWBR = ((F_CPU / SCL_CLOCK) - 16) / 2;

    // Activer l'interface TWI
    TWCR = (1 << TWEN);
}

void i2c_start(void)
{
    // 1. Générer un START
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    // 2. Attendre que le START soit transmis
    while (!(TWCR & (1 << TWINT)));

    // 3. Vérifier le statut
    uint8_t status = TWSR & 0xF8;
    if ((status != 0x08) && (status != 0x10))
    {
        return ;
    } // erreur si pas START ou repeated START

    TWDR = address;
    TWCR = (1 << TWINT) | (1 << TWEN);

    // 5. Attendre la fin de transmission
    while (!(TWCR & (1 << TWINT)));
    convert(status);
    uart_printstr(", ");
}

void i2c_stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}

void AHT20_read(float *temperature, float *humidity)
{
    address = (AHT20_ADDR << 1) | 0;
    i2c_start(); // écriture
    i2c_stop();

    _delay_ms(80);
    address = (AHT20_ADDR << 1) | 1;
    i2c_start(); // lecture
    i2c_stop();
}

int main(void)
{
    float temp, hum;

    cli();
    uart_init();
    i2c_init();
    sei();

    _delay_ms(100); // petit délai après mise sous tension

    while (1)
    {
        AHT20_read(&temp, &hum);
        // uart_printstr("Humidity: ");
        // convert((uint16_t)hum);
        // uart_printstr(", Temperature: ");
        // convert((uint16_t)temp);
        // uart_printstr("\b\b ");
        uart_printstr("\n\r");

        _delay_ms(2000);
    }
}