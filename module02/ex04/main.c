#define F_CPU 16000000UL
#define UART_BAUDRATE 115200UL
#define LEDPIN1 PB0
#define LEDPIN2 PB1
#define LEDPIN3 PB2

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

uint8_t login[21];
uint8_t password[21];
char* login_to_check = "amugisha";
char* password_to_check = "123456";
uint8_t counter = 0;
uint8_t stat = 0;
uint8_t ok = 0;

void uart_init()
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
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Pour que ISR(USART_RX_vect) fonctionne, il faut activer RX Complete Interrupt
    UCSR0B |= (1 << RXCIE0);
}

void timer1_ctc_init()
{
    // Stop le timer pendant la config 
    TCCR1B = 0;
    TCCR1A = 0;

    // Mode CTC 
    TCCR1B |= (1 << WGM12);

    //Prescaler = 16000000 / 1024 = 15625 Hz = 64 us.
    // OCR1A = 15625 * 1 = 15625.
    OCR1A = 15625;

    // TIMSK1 : Timer Interrupt Mask Register. du Timer1
    // OCIE1A : bit du registre TIMSK1
    // OCIE1A : interruption sur comparaison avec le registre OCR1A
    TIMSK1 |= (1 << OCIE1A);

    // prescaler = 1024 (CS12 = 1, CS11 = 0, CS10 = 1)
    TCCR1B |= (1 << CS12) | (1 << CS10);
}

ISR(TIMER1_COMPA_vect)
{
    if (ok == 1)
    {
        PORTB ^= (1 << LEDPIN1) | (1 << LEDPIN2) | (1 << LEDPIN3);
    }
}

void uart_printstr_b(uint8_t i)
{
    uint8_t count = 0;
    // UCSR0A: USART Control and Status Register A.
    // UDRE0 (bit 5) : USART Data Register Empty → vaut 1 quand le registre UDR0 est vide pour ecrire le prochain octet.
    while (count < i)
    {
        while (!(UCSR0A & (1 << UDRE0)));  // Attente buffer vide.
        UDR0 = '\b';
        count++;
    }
    
}

void uart_tx(char c)
{
    // UCSR0A: USART Control and Status Register A.
    // UDRE0 (bit 5) : USART Data Register Empty. vaut 1 quand le registre UDR0 est vide pour ecrire le prochain octet.
    // UDRE0 et UDR0 appartiennent au registre UCSR0A
    
    while (!(UCSR0A & (1 << UDRE0)));  // Attente buffer vide.
    UDR0 = c;
}

void uart_printstr(char* c, uint8_t i)
{
    uint8_t count = 0;
    // UCSR0A: USART Control and Status Register A.
    // UDRE0 (bit 5) : USART Data Register Empty. vaut 1 quand le registre UDR0 est vide pour ecrire le prochain octet.
    while (count < i)
    {
        // UDRE0 et UDR0 appartiennent au registre UCSR0A
        while (!(UCSR0A & (1 << UDRE0)));  // Attente buffer vide.
        UDR0 = c[count];
        count++;
    }
    
}

uint8_t ft_strlen(char* c)
{
    uint8_t i = 0;

    while (c[i])
        i++;
    return i;
}

void check_login_and_password()
{
    uint8_t i = 0;
    uint8_t j = 0;
    password[counter] = '\0';
    uint8_t len_login = ft_strlen(login);
    uint8_t len_password = ft_strlen(password);
    uint8_t len_login_to_check = ft_strlen(login_to_check);
    uint8_t len_password_to_check = ft_strlen(password_to_check);
    while (login[i] && (login[i] == login_to_check[i]))
        i++;
    while (password[j] && (password[j] == password_to_check[j]))
        j++;
    if ((i == len_login_to_check && j == len_password_to_check) && (len_login == len_login_to_check && len_password == len_password_to_check))
    {
        uart_tx('\n');
        uart_printstr_b(15 + len_password);
        uart_printstr("Match !", 7);
        uart_tx('\n');
        uart_printstr_b(7);
        uart_printstr("Welcom ", 7);
        uart_printstr(login, 8);
        uart_printstr("!", 1);
        uart_tx('\n');
        uart_printstr_b(8 + 7 + 1);
        stat++;
        ok = 1;
    }
    else
    {
        uart_tx('\n');
        uart_printstr_b(15 + len_password);
        uart_printstr("No Match !", 10);
        uart_tx('\n');
        uart_printstr_b(10);
        uart_printstr("Enter your login:", 17);
        uart_tx('\n');
        uart_printstr_b(17);
        uart_printstr("     Username: ", 15);
        stat = 0;
    }
    counter = 0;
}

void    suppression_caractere()
{
    if (counter > 0)
    {
        counter--;
        if (stat == 0)
            login[counter] = '\0';
        else
            password[counter] = '\0';
        uart_printstr("\b \b", 3);
    }
}

void    pass_to_password(uint8_t c)
{
    uart_tx('\n');
    uart_printstr_b(10 + counter);
    uart_printstr("Password: ", 10);
    login[counter] = '\0';
    counter = 0;
    stat = 1;
}

void    write_caractere(c)
{
    if (stat == 1)
        uart_tx('*');
    else
        uart_tx(c);
    counter++;
}

// interruption routine a executer au moment du match
ISR(USART_RX_vect)
{
    uint8_t c = UDR0;

    if (c == 127)
    {
        suppression_caractere();
    }
    else
    {
        if (counter < 20)
        {
            if (stat == 0)
                login[counter] = c;
            else
                password[counter] = c;
        }

        if (c == '\r' && stat == 0)
            pass_to_password(c);
        else if (c == '\r' && stat == 1)
            check_login_and_password();
        else if (stat < 2 && counter < 20)
            write_caractere(c);
    }
}

int main(void)
{   
    DDRB |= (1 << LEDPIN1) | (1 << LEDPIN2) | (1 << LEDPIN3); 
    // Clear Interrupts
    cli();           // desactive interruptions globales pendant init
    uart_init();
        uart_printstr("Enter your login:", 17);
        uart_tx('\n');
        uart_printstr_b(17);
        uart_printstr("     Username: ", 15);
    sei(); // Set Interrupts
    timer1_ctc_init();
    while (1)
    {
    }
}
