#ifndef I2C_LCD_H
#define I2C_LCD_H

#include <stdint.h>

typedef enum I2C_LCD_Mode {
  START,
  ADDR,
  SEND_BYTE,
  STOP,
} I2C_LCD_Mode;

void I2C_LCD_init(void);
void I2C_LCD_send_bytes(const uint8_t *data, int size);
I2C_LCD_Mode I2C_LCD_get_mode(void);
int8_t I2C_LCD_get_status(void);

#endif // I2C_LCD_H
