# 16x2 LCD driver for AVR MCUs

## Overview

This contains generic I2C and LCD1602 drivers for ease of working on AVR chipset. See **main.c** to grasp a view on how to use the library.

### DHT22 Humidity and Temporature Sensor Driver

The sensor requires 1 open-drain pin. We must set up its port's **in**, **out**, and **direction** register.
```{c}
typedef struct PortPin {
  volatile uint8_t *dir, *in, *out;
  uint8_t pin;
} PortPin;
```

Sensored results are stored in a struct.
```{c}
typedef struct DhtResult {
  float temp, humid;
} DhtResult;
```

## I2C Master Driver

The slave address, the operation mode (read or write), and the buffer to send/receive need to be specified.
```{c}
typedef struct I2cOp {
  uint8_t addr_rw;
  const uint8_t *buf;
  uint8_t buflen;
} I2cOp;
```

## LCD Driver

We can control a LCD through its instruction set. The initialization sequence and instructions are handled by the library, so the user can just care about the **backlight**, the **cursor's position**, and the **display string**.

```{c}
typedef struct I2cOp {
  uint8_t addr_rw;
  const uint8_t *buf;
  uint8_t buflen;
} I2cOp;
```

## Build and Installation

### Configure CMake

```{text}
cmake -S. -Bbuild
```

### Build Targets

Targets can be built:
- ELF
- HEX
- EEPROM

```{text}
cmake --build build --target <target>
```

### Burn to Flash

```{text}
cmake --build build --target flash
```
