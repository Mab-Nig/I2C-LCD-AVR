#include "DHT.h"

#include <avr/io.h>
#include <util/delay.h>

#define DHT_PIN PB0
#define START_US 500
#define BIT0_US 30
#define BIT_START_US 50

static void DHT_send_start_signal(void);
static uint8_t DHT_read_byte(void);
static void DHT_send_end_signal(void);

void DHT_init(void) {}

void DHT_read(uint8_t *temperature, uint8_t *humidity) {
  DHT_send_start_signal();
  
  *temperature = DHT_read_byte();
  DHT_read_byte();
  *humidity = DHT_read_byte();
  DHT_read_byte();
  DHT_read_byte();
  
  DHT_send_end_signal();
}

void DHT_send_start_signal(void) {
  DDRB |= _BV(DHT_PIN);
  PORTB &= ~_BV(DHT_PIN);
  _delay_us(START_US);
  PORTB |= _BV(DHT_PIN);

  DDRB &= ~_BV(DHT_PIN);
  while (PINB & _BV(DHT_PIN))
    ;
  while (!(PINB & _BV(DHT_PIN)))
    ;
}

uint8_t DHT_read_byte(void) {
  uint8_t data = 0;
  for (uint8_t i = 8; i; --i) {
    while (PINB & _BV(DHT_PIN))
      ;
    while (!(PINB & _BV(DHT_PIN)))
      ;
    _delay_us(BIT0_US);
    data <<= 1;
    if (PINB & _BV(DHT_PIN)) {
      data |= 1;
    }
  }
  while (PINB & _BV(DHT_PIN))
    ;
  return data;
}

void DHT_send_end_signal(void) {
  _delay_us(BIT_START_US);
  DDRB |= _BV(DHT_PIN);
  PORTB |= _BV(DHT_PIN);
}
