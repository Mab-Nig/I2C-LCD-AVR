#include "i2c.h"

#include <avr/io.h>
#include <stdint.h>
#include <util/twi.h>

#define I2C_FREQ 100000

static int i2c_op_start(void);
static int i2c_op_send_addr_rw(const I2cOp *op);
static void i2c_op_stop(void);

inline void i2c_init() {
  TWBR = (F_CPU / I2C_FREQ - 16) >> 1;
  TWCR |= _BV(TWEN);
}

inline void i2c_exit(void) { TWCR &= ~_BV(TWEN); }

int i2c_op_send(const I2cOp *op) {
  if (i2c_op_start() < 0) {
    return -1;    
  }
  if (i2c_op_send_addr_rw(op) < 0) {
    return -1;    
  }

  const uint8_t *buf = op->buf;
  uint8_t buflen = op->buflen;  
  while (buflen) {
    TWDR = *buf;
    TWCR = _BV(TWINT) | _BV(TWEN);
    while (!(TWCR & _BV(TWINT)))
      ;

    if (TW_STATUS != TW_MT_DATA_ACK) {
      return -1;
    }

    --buflen;
    ++buf;
  }

  i2c_op_stop();
  return 0;
}

int i2c_op_start(void) {
  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
  while (!(TWCR & _BV(TWINT)))
    ;
  return TW_STATUS == TW_START || TW_STATUS == TW_REP_START ? 0 : -1;  
}

int i2c_op_send_addr_rw(const I2cOp *op) {
  TWDR = op->addr_rw;
  TWCR = _BV(TWINT) | _BV(TWEN);
  while (!(TWCR & _BV(TWINT)))
    ;
  return TW_STATUS == TW_MT_SLA_ACK ? 0 : -1;
}

void i2c_op_stop(void) { TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); }
