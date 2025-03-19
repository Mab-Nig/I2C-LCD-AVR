#include "I2C_LCD.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#define SDA_PIN PC4
#define SCL_PIN PC5
#define SLA_W 0x7F

static I2C_LCD_Mode s_mode;
static int8_t s_status;
static const uint8_t *s_data;
static int s_size;

static void I2C_LCD_start(void);
static void I2C_LCD_send_addr(void);
static void I2C_LCD_send_byte(void);
static void I2C_LCD_stop(void);

ISR(TWI_vect) {
  switch (s_mode) {
  case START:
    s_status = TWSR == 0x08 ? 0 : -1;
    I2C_LCD_send_addr();
    break;
  case ADDR:
    s_status = TWSR == 0x18 ? 0 : -1;
    I2C_LCD_send_byte();
    break;
  case SEND_BYTE:
    s_status = TWSR == 0x28 ? 0 : -1;
    if (s_size--) {
      ++s_data;
      I2C_LCD_send_byte();
    } else {
      I2C_LCD_stop();
    }
    break;
  default:
    break;
  }
}

inline void I2C_LCD_init(void) {
  TWCR = _BV(TWEN) | _BV(TWIE);
  sei();
}

inline void I2C_LCD_send_bytes(const uint8_t *data, int size) {
  s_data = data;
  s_size = size;
  I2C_LCD_start();
}

inline void I2C_LCD_start(void) {
  s_mode = START;
  TWCR |= _BV(TWINT) | _BV(TWSTA);
}

inline void I2C_LCD_send_addr(void) {
  s_mode = ADDR;
  TWDR = SLA_W;
  TWCR |= _BV(TWINT);
}

void I2C_LCD_send_byte(void) {
  s_mode = SEND_BYTE;
  TWDR = *s_data;
  TWCR |= _BV(TWINT);
}

inline void I2C_LCD_stop(void) {
  s_mode = STOP;
  TWCR |= _BV(TWINT) | _BV(TWSTO);
}

I2C_LCD_Mode I2C_LCD_get_mode(void) { return s_mode; }

int8_t I2C_LCD_get_status(void) { return s_status; }
