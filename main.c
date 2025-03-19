#include "DHT.h"
#include "I2C_LCD.h"

#include <avr/io.h>
#include <util/delay.h>

#define DELAY_MS 2000

void init(void);

int main(void) {
  init();
  while (1) {
    uint8_t temperature, humidity;
    DHT_read(&temperature, &humidity);
    _delay_ms(DELAY_MS);
  }
  __builtin_unreachable();
}

void init(void) {
  DHT_init();
  I2C_LCD_init();
}
