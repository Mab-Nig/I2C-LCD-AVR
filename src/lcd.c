#include "lcd.h"
#include "i2c.h"

#include <stdint.h>
#include <util/delay.h>
#include <util/twi.h>

#define POWER_ON_DELAY_MS    100
#define INIT1_DELAY_US       4200
#define INIT2_DELAY_US       150
#define NORMAL_CMD_DELAY_US  50
#define SPECIAL_CMD_DELAY_US 1600

#define RS_PIN 0
#define RW_PIN 1
#define E_PIN  2
#define BL_PIN 3

#define CLEAR                  0x01
#define RETURN_HOME            0x02
#define ENTRY_MODE_SET(ID, S)  (_BV(2) | ((ID) << 1) | (S))
#define DISPLAY_CTRL(D, C, B)  (_BV(3) | ((D) << 2) | ((C) << 1) | (B))
#define FUNCTION_SET(DL, N, F) (_BV(5) | ((DL) << 4) | ((N) << 3) | ((F) << 2))
#define SET_DDRAM_ADDR(addr)   (_BV(7) | (addr))

typedef struct LcdCmd {
  uint8_t addr, code, rs, rw, bl;
} LcdCmd;

static int lcd_send_cmd(const LcdCmd *cmd);
static int lcd_send_pins(uint8_t addr, uint8_t pins);

int lcd_op_init(const LcdOp *op) {
  i2c_init();

  _delay_ms(POWER_ON_DELAY_MS);

  uint8_t pins = 0x30 | (op->backlight << BL_PIN);
  if (lcd_send_pins(op->addr, pins) < 0) {
    return -1;
  }
  _delay_us(INIT1_DELAY_US);

  if (lcd_send_pins(op->addr, pins) < 0) {
    return -1;
  }
  _delay_us(INIT2_DELAY_US);

  if (lcd_send_pins(op->addr, pins) < 0) {
    return -1;
  }
  _delay_us(NORMAL_CMD_DELAY_US);

  pins = 0x20 | (op->backlight << BL_PIN);
  if (lcd_send_pins(op->addr, pins) < 0) {
    return -1;
  }
  _delay_us(NORMAL_CMD_DELAY_US);

  LcdCmd cmd = {.addr = op->addr, .rs = 0, .rw = 0, .bl = op->backlight};
  uint8_t codes[] = {
    FUNCTION_SET(0, 1, 0), DISPLAY_CTRL(0, 0, 0), CLEAR, ENTRY_MODE_SET(1, 0)
  };
  for (uint8_t i = 0; i < sizeof(codes); ++i) {
    cmd.code = codes[i];
    if (lcd_send_cmd(&cmd) < 0) {
      return -1;
    }
  }
  return 0;
}

int lcd_op_send(const LcdOp *op) {
  LcdCmd cmd = {.addr = op->addr, .rs = 0, .rw = 0, .bl = op->backlight};

  if (op->clear) {
    cmd.code = CLEAR;
    if (lcd_send_cmd(&cmd) < 0) {
      return -1;
    }
  }

  if (!op->buf) {
    return lcd_send_pins(cmd.addr, op->backlight << BL_PIN);
  }

  {
    uint8_t ddram_addr = (op->row << 6) | op->col;
    uint8_t codes[] = {DISPLAY_CTRL(0, 0, 0), SET_DDRAM_ADDR(ddram_addr)};

    for (uint8_t i = 0; i < sizeof(codes); ++i) {
      cmd.code = codes[i];
      if (lcd_send_cmd(&cmd) < 0) {
        return -1;
      }
    }
  }

  cmd.rs = 1;
  const char *buf = op->buf;

  while (*buf) {
    cmd.code = *buf;
    if (lcd_send_cmd(&cmd) < 0) {
      return -1;
    }
    ++buf;
  }

  cmd.rs = 0;
  cmd.code = DISPLAY_CTRL(1, 0, 0);
  if (lcd_send_cmd(&cmd) < 0) {
    return -1;
  }

  return 0;
}

int lcd_send_cmd(const LcdCmd *cmd) {
  uint8_t high = (cmd->code & 0xF0);
  uint8_t low = (cmd->code << 4);
  uint8_t pins =
    (cmd->bl << BL_PIN) | (cmd->rw << RW_PIN) | (cmd->rs << RS_PIN);

  if (lcd_send_pins(cmd->addr, high | pins) < 0) {
    return -1;
  }
  if (lcd_send_pins(cmd->addr, low | pins) < 0) {
    return -1;
  }

  if (cmd->code == CLEAR || cmd->code == RETURN_HOME) {
    _delay_us(SPECIAL_CMD_DELAY_US);
  } else {
    _delay_us(NORMAL_CMD_DELAY_US);
  }
  return 0;
}

int lcd_send_pins(uint8_t addr, uint8_t pins) {
  I2cOp op = {(addr << 1) | TW_WRITE, &pins, 1};

  pins |= _BV(E_PIN);
  if (i2c_op_send(&op) < 0) {
    return -1;
  }
  _delay_us(1);

  pins &= ~_BV(E_PIN);
  if (i2c_op_send(&op) < 0) {
    return -1;
  }
  _delay_us(1);

  return 0;
}
