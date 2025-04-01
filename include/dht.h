#ifndef DHT_H
#define DHT_H

#include <stdint.h>

typedef struct PortPin {
  volatile uint8_t *dir, *in, *out;
  uint8_t pin;
} PortPin;

typedef struct DhtResult {
  uint8_t temp_int, temp_dec, hum_int, hum_dec;
} DhtResult;

void dht_init(const PortPin *port);
int dht_sense(const PortPin *port, DhtResult *result);

#endif // DHT_H
