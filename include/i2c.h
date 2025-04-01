#ifndef I2C_H
#define I2C_H

#include <stdint.h>

typedef struct I2cOp {
  uint8_t addr_rw;
  const uint8_t *buf;
  uint8_t buflen;
} I2cOp;

void i2c_init(void);
void i2c_exit(void);

int i2c_op_send(const I2cOp *op);
// int i2c_op_receive(const I2cOp *op);

#endif // I2C_OP_H
