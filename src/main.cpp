#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define LED_PIN    (PB5) // Arduino D13 pin / Onboard LED

void initADC() {
  // Set PC0/A0 as an input pin
  DDRC &= ~(1 << DDC0); 
  // Disable Digital Input Buffer on A0
  DIDR0 |= (1<<ADC0D);
  // Set reference voltage to AVcc
  ADMUX = (1 << REFS0); 
  // Right-Adjust Result (ADLAR=0)
  ADMUX  &= ~(1<<ADLAR); 
  // Set ADC prescaler to 128 (ADPS[2:0]="111")
  // 16MHz/128 = 125kHz ADC clock
  ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
  ADCSRA |= (1<<ADEN); // Enable ADC 
  ADCSRA |= (1<<ADSC); // Start the first ADC conversion
  // Wait for ADC conversion to complete
  while (ADCSRA & (1<<ADSC));
}

uint16_t readAnalog( uint8_t channel) { // Read A0 or A1 pin
  // Clear all bits of ADMUX[3:0] (select A0 by default)
  ADMUX &= ~((1<<MUX3)|(1<<MUX2)|(1<<MUX1)|(1<<MUX0));
  if (channel == 1) { // Select A1 input
     ADMUX |= (1<<MUX0); // Set MUX0 bit
  }
  // Start a single ADC conversion
  ADCSRA |= (1<<ADSC);
  // Wait for ADC conversion to complete
  while (ADCSRA & (1<<ADSC));
  // Return the result (10-bit, right-aligned)
  return ADC;
}

int main(void) {
  DDRB |= (1 << LED_PIN);
  initADC(); // Initialize ADC
  while (1) {
    uint16_t a0 = readAnalog(0); // Read A0 input
      if (a0 > 512) { // Compare both inputs
       PORTB |= (1<<LED_PIN); // Output HIGH
    } else {
       PORTB &= ~(1<<LED_PIN); // Output LOW
    }
  }
}