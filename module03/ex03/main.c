#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define RED PD5 // red       // Sur Arduino Uno, la LED est sur le port B
#define BLEU PD3 // bleu
#define GREEN PD6 // green
#define F_CPU 16000000UL
#define UART_BAUDRATE 115200UL

uint8_t ok = 0;
uint8_t index_rgb = 0;
char    rgb[8];
uint8_t esc_seq;                                            // detecter les suites de caractere speciaux

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

void uart_printstr_b(uint8_t i)                         // remet le curseur a gauche
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

void uart_printstr(char* c)                             // print la chaine de caractere
{
    uint8_t count = -1;
    // UCSR0A: USART Control and Status Register A.
    // UDRE0 (bit 5) : USART Data Register Empty → vaut 1 quand le registre UDR0 est vide pour ecrire le prochain octet.
    while (c[++count])
        uart_tx(c[count]);
}

void    suppression_caractere()                         // supprime le caractere
{
    if (index_rgb > 0)
    {
        index_rgb--;
        rgb[index_rgb] = '\0';
        uart_printstr("\b \b");
    }
}

uint8_t ft_strlen(char* c)                              // ft_strlen
{
    uint8_t i = 0;

    while (c[i])
        i++;
    return i;
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)           // set le rgb
{
    OCR0B = r; // RED
    OCR0A = g; // GREEN
    OCR2B = b; // BLUE
}

void    init_rgb()
{
    // Fast PMW - non inverser
    // Timer0 D3 Bleu
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);
    // prescaler 1024
    TCCR0B = (1 << CS02) | (1 << CS00); 

    // === Timer2 (pour BLUE) ===
    // Fast PWM, non-inverse sur OC2B
    TCCR2A = (1 << COM2B1) | (1 << WGM20) | (1 << WGM21);
    TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);

}

uint8_t is_in_base(uint8_t c)                           // check si le caractere est dans la base
{
    char*   base_hexa_up = "0123456789ABCDEF";
    char*   base_hexa_low = "0123456789abcdef";
    uint8_t i = 0;
    uint8_t ok = 0;

    while (base_hexa_up[i])
    {
        if (base_hexa_up[i] == c)
            ok = 1;
        i++;
    }
    i = 0;
    while (base_hexa_low[i])
    {
        if (base_hexa_low[i] == c)
            ok = 1;
        i++;
    }
    return (ok);
}

void    print_error()                                   // print l'erreur et redonne l'entrer
{
    uart_printstr("Not the good format! Try again!\n");
    uart_printstr_b(ft_strlen("Not the good format! Try again!"));
    uart_printstr("Enter a color in hexa: ");
}

uint8_t hex_to_dec(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
}

void    convert_hex_to_dec()                           // converti l'hexa en decimale
{
    uint8_t r;
    uint8_t g;
    uint8_t b;

    r = hex_to_dec(rgb[1]) * 16 + hex_to_dec(rgb[2]);
    g = hex_to_dec(rgb[3]) * 16 + hex_to_dec(rgb[4]);
    b = hex_to_dec(rgb[5]) * 16 + hex_to_dec(rgb[6]);
    set_rgb(r,g,b);
    uart_printstr("Enter a color in hexa: ");
}

void    print_charactere(uint8_t c)                             
{
    uart_tx(rgb[c]);
    uart_tx('\b');
    uart_tx('\n');
}

void check(void)
{
    uint8_t i = 1;
    if (rgb[0] == '#')
    {
        while (rgb[i])
        {
            if (is_in_base(rgb[i]))
                i++;
            else
            {
                print_charactere(i);
                print_error();
                break ;
            }
        }
        if (i == 7)
            convert_hex_to_dec();
    }
    else
    {
        print_charactere(0);
        print_error();
    }
}

uint8_t not_save_arrow(uint8_t c)
{
    // touche esc
    if (c == 27) {
        esc_seq = 1; // prochain caractere sera '['
        return 0;
    }

    // sequence d'echappement
    if (esc_seq == 1) {
        if (c == '[') {
            esc_seq = 2; // le prochain sera A/B/C/D
            return 0;
        } else {
            esc_seq = 0; // ce n'etait pas une fleche
        }
    }

    // lettre finale de la fleche
    if (esc_seq == 2) {
        if (c == 'A' || c == 'B' || c == 'C' || c == 'D') {
            esc_seq = 0;
            return 0;
        } else {
            esc_seq = 0;
        }
    }
    return (-1);
}

ISR(USART_RX_vect)
{
    uint8_t c = UDR0;
    uint8_t len_rgb;

    if (c == 127)
    {
        suppression_caractere();
    }
    else
    {
        if (c == '\r')
        {
            rgb[index_rgb] = '\0';
            len_rgb = ft_strlen(rgb);
            uart_tx('\n');
            uart_printstr_b(ft_strlen("Enter a color in hexa: ") + len_rgb);
            if (len_rgb != 7)
                print_error();
            else
                check();
            index_rgb = 0;
        }
        else
        {
            if (index_rgb < 7)
            {
                if (not_save_arrow(c) != 0) // ici pour eviter d'enregistrer les touches fleuche etc
                {
                    rgb[index_rgb] = c;
                    index_rgb++;
                    uart_tx(c);
                }
            }
        }
    }
}

int main(void)
{
    // Data Direction Register du port D. Configuration : PB5/3/6
    DDRD |= (1 << RED) | (1 << BLEU) | (1 << GREEN);
    cli();
    uart_init();
    sei();
    init_rgb();
    set_rgb(255,94,00);
    uart_printstr("Enter a color in hexa: ");
    // Boucle vide
    while (1)
    {
    }
}