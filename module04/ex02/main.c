#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LED1 PB0
#define LED2 PB1
#define LED3 PB2
#define LED4 PB4
#define BUTTON_PLUS PD2
#define BUTTON_MOIN PD4

uint8_t counter = 0;
void    switch_led()
{
    if (counter == 0)
    {
        PORTB = 0;
    }
    else if (counter == 1)
    {
        PORTB = (1 << LED1);
    }
    else if (counter == 2)
    {
        PORTB = (1 << LED2);
    }
    else if (counter == 3)
    {
        PORTB = (1 << LED1) | (1 << LED2);
    }
    else if (counter == 4)
    {
        PORTB = (1 << LED3);
    }
    else if (counter == 5)
    {
        PORTB = (1 << LED1) | (1 << LED3);
    }
    else if (counter == 6)
    {
        PORTB = (1 << LED2) | (1 << LED3);
    }
    else if (counter == 7)
    {
        PORTB = (1 << LED1) | (1 << LED2) | (1 << LED3);
    }
    else if (counter == 8)
    {
        PORTB = (1 << LED4);
    }
    else if (counter == 9)
    {
        PORTB = (1 << LED1) | (1 << LED4);
    }
    else if (counter == 10)
    {
        PORTB = (1 << LED2) | (1 << LED4);
    }
    else if (counter == 11)
    {
        PORTB = (1 << LED1) | (1 << LED2) | (1 << LED4);
    }
    else if (counter == 12)
    {
        PORTB = (1 << LED3) | (1 << LED4);
    }
    else if (counter == 13)
    {
        PORTB = (1 << LED1) | (1 << LED3) | (1 << LED4);
    }
    else if (counter == 14)
    {
        PORTB = (1 << LED2) | (1 << LED3) | (1 << LED4);
    }
    else if (counter == 15)
    {
        PORTB = (1 << LED1) | (1 << LED2) | (1 << LED3) | (1 << LED4);
    }
}

// interrup extern pour SW1 => PD2
// Page 79
ISR(INT0_vect)
{
    // verifie suit on appuie
    if (EICRA & (1 << ISC01) && !(EICRA & (1 << ISC00))) {
        if (!(PIND & (1 << BUTTON_PLUS)))
            counter++;
        // active la detection du relachement
        EICRA |= (1 << ISC00);  // ISC01=1, ISC00=1  
    } 
    else if (EICRA & (1 << ISC01) && (EICRA & (1 << ISC00))) {
        // Reconfigure pour detecter l'appuie
        EICRA &= ~(1 << ISC00); // ISC01=1, ISC00=0 
    }
    _delay_ms(200);
    if (counter > 15)
        counter = 0;
    switch_led();
}

// PD4 n'a pas interruption extern INT0/INT1
// Page 82
// Pin Change Interrupt Control Register
ISR(PCINT2_vect)
{
    static uint8_t last_state = 0xFF;  
    uint8_t current_state = PIND;      
    uint8_t changed = current_state ^ last_state; // bits qui ont changer

    if (changed & (1 << PD4)) {
        if (!(current_state & (1 << PD4))) {
            _delay_ms(200);
            if (counter == 0)
                counter = 16; 
            counter--;
            switch_led();
        }
    }
    last_state = current_state;
}

int main(void)
{
    cli(); // desactive les interrupt
    DDRB |= (1 << LED1) | (1 << LED2) | (1 << LED3) | (1 << LED4);         // LED en sortie
    DDRD &= ~((1 << BUTTON_PLUS) | (1 << BUTTON_MOIN));     // Bouton en entree
    PORTD |= (1 << BUTTON_PLUS) | (1 << BUTTON_MOIN);     // Pull-up activee 

    // Configure INT0 
    EICRA |= (1 << ISC01) | (1 << ISC11); // ISC01=1 ISC00=0 => relacher
    EICRA &= ~((1 << ISC00) | (1 << ISC10));

    // Interrupt lier au pin et broche Page 97
    PCICR |= (1 << PCIE2);
    PCMSK2 |= (1 << PCINT20);

    EIMSK |= (1 << INT0);       // Active INT0

    sei(); // Active les interruptions globales

    while (1) {
    }
}
