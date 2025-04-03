#ifndef DHT_H
#define DHT_H

#include <stdint.h>

typedef struct PortPin {
  volatile uint8_t *dir, *in, *out;
  uint8_t pin;
} PortPin;

typedef struct DhtResult {
  float temp, humid;
} DhtResult;

void dht_init(const PortPin *port);
int dht_sense(const PortPin *port, DhtResult *result);
void lcd_flash(void);

#endif // DHT_H
