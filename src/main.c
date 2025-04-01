#include "dht.h"
#include "lcd.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include <stddef.h>
#include <stdio.h>

#define DELAY_MS     2000
#define I2C_ADDR     0x27
#define MAX_BUF_SIZE 256

LcdOp op;
PortPin port;
DhtResult result;

static void init(void);

int main(void) {
  init();

  char buf[MAX_BUF_SIZE];

  while (1) {
    dht_sense(&port, &result);

    op.buf = buf;

    sprintf(buf, "Temp:  %d.%d", result.temp_int, result.temp_dec);
    op.row = 0;
    op.col = 0;
    lcd_op_send(&op);

    sprintf(buf, "Humid: %d.%d%%", result.hum_int, result.hum_dec);
    op.row = 1;
    lcd_op_send(&op);

    _delay_ms(DELAY_MS);
  }

  __builtin_unreachable();
}

inline void init(void) {
  op.addr = I2C_ADDR;
  op.backlight = 1;
  lcd_op_init(&op);

  port.dir = &DDRB;
  port.in = &PINB;
  port.out = &PORTB;
  port.pin = PB0;
  dht_init(&port);
}
