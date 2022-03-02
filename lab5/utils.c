#include <lcom/lcf.h>

#include <stdint.h>

extern int count_sysb;

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {

    uint16_t lsbMask = 0x00ff;
    *lsb = (uint8_t) (val & lsbMask);

  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  uint16_t msbMask = 0xff00;
  *msb = (uint8_t) ((val & msbMask) >> 8);

  return 0;
}

int (util_sys_inb)(int port, uint8_t * value) {

  uint32_t x;
  sys_inb(port,&x);
  *value = (uint8_t) x;

  //Counts the number os sys_inb calls
  #ifdef LAB3
  count_sysb++;
  #endif

  return 0;
}


