#define F_CPU 16000000UL
#define UART_BAUDRATE 115200UL
#define TOS 314.0   // Offset typique à 25°C
#define K   1.22    // Slope typique (LSB/°C)
#define RED PD5 // red       // Sur Arduino Uno, la LED est sur le port B
#define BLEU PD3 // bleu
#define GREEN PD6 // green

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

// Figure 24-7.
void ADC_init(void)
{
    // Activer l’ADC (ADEN = 1)
    // Choisir un prescaler pour une fréquence ADC ~125 kHz (si F_CPU = 16 MHz → diviseur = 128)
    // Table 24.9.2
    ADCSRA = (1 << ADEN); // ADEN: ADC Enable
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // divise par 128
}

uint16_t ADC_read_RV(void) {
    // Référence = AVCC (bit REFS0 = 1, REFS1 = 0)
    // Résolution 8 bits -> ADLAR = 1 (justifier à gauche)
    // Canal ADC0 -> MUX bits = 0000
    // 24.9.1
    // MUX = 0000 = ADC0 = PC0 = RV1
    ADMUX = (1 << REFS0) | (1 << ADLAR) | (0b00000000);
    // 24.9.2
    // Demarrer la conversion
    ADCSRA |= (1 << ADSC);

    // Attendre la fin de la conversion
    while (ADCSRA & (1 << ADSC));
    // 24.9.3
    // lire ADCH seulement
    return ADCH;
}

void    init_timer_rgb()
{
    // Fast PMW - non inverser
    // Timer0 D3 Bleu
    TCCR0A |= (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00) | (1 << WGM01);
    // prescaler 1024
    TCCR0B |= (1 << CS02) | (1 << CS00); 

    // === Timer2 (pour BLUE) ===
    // Fast PWM, non-inverse sur OC2B
    TCCR2A |= (1 << COM2B1) | (1 << WGM20) | (1 << WGM21);
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
}

void init_timer1(void) {
    // Mode CTC, prescaler 1024, F_CPU = 16MHz
    // OCR1A = F_CPU / prescaler / freq - 1
    // 16M / 1024 *0.02 = 312
    TCCR1B |= (1 << WGM12); // CTC
    OCR1A = 312;
    TCCR1B |= (1 << CS12) | (1 << CS10); // prescaler 1024
    TIMSK1 |= (1 << OCIE1A); // enable interrupt compare A
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    OCR0B = r; // RED
    OCR0A = g; // GREEN
    OCR2B = b; // BLUE
}

void wheel(uint8_t pos)
{
    pos = 255 - pos;
    if (pos < 85)
    {
        set_rgb(255 - pos * 3, 0, pos * 3);
    }
    else if (pos < 170)
    {
        pos = pos - 85;
        set_rgb(0, pos * 3, 255 - pos * 3);
    }else
    {
        pos = pos - 170;
        set_rgb(pos * 3, 255 - pos * 3, 0);
    }
}

ISR(TIMER1_COMPA_vect) {
    uint8_t rest = ADC_read_RV();
    wheel(rest);

    if (rest < 255 / 4)
        PORTB = 0b00000000;
    else if (rest < 255 / 2)
        PORTB = 0b00000001;
    else if (rest < (255 / 4 * 3))
        PORTB = 0b00000011;
    else if (rest < 255)
        PORTB = 0b00000111;
    else
        PORTB = 0b00010111;
}

int main(void)
{
    cli();
    ADC_init();
    init_timer_rgb();
    init_timer1();
    sei();

    DDRD |= (1 << RED) | (1 << BLEU) | (1 << GREEN);
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);

    while (1);
}
