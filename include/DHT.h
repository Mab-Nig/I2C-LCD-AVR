#ifndef DHT_H
#define DHT_H

#include <stdint.h>

void DHT_init(void);
void DHT_read(uint8_t *temperature, uint8_t *humidity);

#endif // DHT_H
