#ifndef I2C_H
#define I2C_H

#include <stdint.h>

typedef struct I2C_Op {
  uint8_t addr;
  uint8_t buflen;
  volatile uint8_t bufidx;
  const uint8_t *buf;
} I2C_Op;

void I2C_init(void);
void I2C_transmit(I2C_Op *op);
void I2C_wait_transmission(void);

#endif // I2C_H
