#include "dht.h"
#include "lcd.h"

#include <avr/io.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <util/delay.h>

#define DHT_INIT_MS          1000
#define DHT_START_MS         18
#define DHT_WAIT_RESPONSE_US 40
#define DHT_RESPONSE_US      80
#define DHT_PREPARE_DATA_US  60
#define DHT_STOP_US          50
#define DHT_BIT0_US          30
#define DHT_BIT1_US          70

#define DHT_DATA_NBYTE 5

inline void dht_init(const PortPin *port) {
  *port->dir |= _BV(port->pin);
  *port->out |= _BV(port->pin);
  _delay_ms(DHT_INIT_MS);
}

int dht_sense(const PortPin *port, DhtResult *result) {
  uint8_t time;

  *port->dir |= _BV(port->pin);
  *port->out |= _BV(port->pin);
  _delay_us(100);

  *port->out &= ~_BV(port->pin);
  _delay_ms(DHT_START_MS);

  time = 0;
  *port->out |= _BV(port->pin);
  *port->dir &= ~_BV(port->pin);
  while (*port->in & _BV(port->pin)) {
    if (time > DHT_WAIT_RESPONSE_US) {
      return -1;
    }
    _delay_us(2);
    time += 2;
  }

  time = 0;
  while (!(*port->in & _BV(port->pin))) {
    if (time > DHT_RESPONSE_US) {
      return -1;
    }
    _delay_us(2);
    time += 2;
  }

  time = 0;
  while (*port->in & _BV(port->pin)) {
    if (time > DHT_RESPONSE_US) {
      return -1;
    }
    _delay_us(2);
    time += 2;
  }

  uint8_t data[5] = {};

  for (uint8_t nbyte = 0; nbyte < DHT_DATA_NBYTE; ++nbyte) {
    for (uint8_t i = 8; i; --i) {
      time = 0;
      while (!(*port->in & _BV(port->pin))) {
        if (time > DHT_PREPARE_DATA_US) {
          return -1;
        }
        _delay_us(2);
        time += 2;
      }

      _delay_us(DHT_BIT0_US);
      if (*port->in & _BV(port->pin)) {
        data[nbyte] |= _BV(i - 1);
      }

      time = 0;
      while (*port->in & _BV(port->pin)) {
        if (time > DHT_BIT1_US) {
          return -1;
        }
        _delay_us(2);
        time += 2;
      }
    }
  }

  _delay_us(DHT_STOP_US);
  *port->dir |= _BV(port->pin);
  *port->out |= _BV(port->pin);

  result->humid = (float)(((uint16_t)data[0] << 8) | data[1]) / 10;
  result->temp = (float)(((int16_t)data[2] << 8) | data[3]) / 10;

  return (((uint16_t)data[0] + data[1] + data[2] + data[3]) & 0xFF) == data[4]
    ? 0
    : -1;
}

void lcd_flash(void) {
  LcdOp lcd_op = {.addr = 0x27, .clear = 0, .buf = NULL};
  lcd_op.backlight = 0;
  lcd_op_send(&lcd_op);
  _delay_ms(50);
  lcd_op.backlight = 1;
  lcd_op_send(&lcd_op);
  _delay_ms(50);
}
