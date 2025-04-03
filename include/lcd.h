#ifndef LCD_H
#define LCD_H

#include <stdint.h>

typedef struct LcdOp {
  uint8_t addr;
  uint8_t backlight, clear;
  uint8_t row, col;
  const char *buf;
} LcdOp;

int lcd_op_init(const LcdOp *op);
int lcd_op_send(const LcdOp *op);

#endif // LCD_H
