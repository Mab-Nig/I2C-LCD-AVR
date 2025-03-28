#include <avr/io.h>
#include <util/delay.h>

#define DELAY_MS 2000

static void init(void);

int main(void) {
  init();
  
  while (1) {
    _delay_ms(DELAY_MS);
  }
  
  __builtin_unreachable();
}

inline void init(void) {}
