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

#define DHT_PORT_IN  PINB
#define DHT_PORT_OUT PORTB
#define DHT_PORT_DIR DDRB
#define DHT_PIN      PB0

LcdOp op;
PortPin port;
DhtResult result;

static void init(void);

int main(void) {
  init();

  char buf[MAX_BUF_SIZE];

  while (1) {
    op.buf = NULL;
    op.clear = 1;

    if (dht_sense(&port, &result) < 0) {
      lcd_flash();
    }

    op.buf = buf;
    op.clear = 0;

    sprintf(
      buf, "Temp:  %d.%d%cC", (int)result.temp, (int)(result.temp * 10) % 10,
      0xDF
    );
    op.row = 0;
    op.col = 0;
    lcd_op_send(&op);

    sprintf(
      buf, "Humid: %d.%d%%", (int)result.humid, (int)(result.humid * 10) % 10
    );
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

  port.dir = &DHT_PORT_DIR;
  port.in = &DHT_PORT_IN;
  port.out = &DHT_PORT_OUT;
  port.pin = DHT_PIN;
  dht_init(&port);
}
