 #pragma once
  
 #include <stdbool.h>
 #include <stdint.h>
  
 #define STAT_REG 0x64
 #define KBC_COMMAND_REG 0x64
 #define KBC_ARGUMENT_REG 0x60
 #define WRITE_COMMAND_BYTE 0x60
 #define READ_COMMAND_BYTE 0x20
 #define OUT_BUF_REG 0x60
 #define OBF_BIT BIT(0)
 #define INT0 BIT(0)
 #define PARITY_BIT BIT(7)
 #define TIME_OUT_BIT BIT(6)
 #define AUX_BIT BIT(5)
 #define DELAY_US 15
 #define INCOMPLETE_SCAN_CODE 0xE0
 #define ESC_BREAK_CODE 0x81


 int keyboard_subscribe(uint8_t *bit_no);

 int keyboard_unsubscribe();

 int reenable_keyboard_interrupt();

 int flush_OBF();

 int got_full_scan_code();

 void print_full_scan_code();

 int run_until_ESC_key();

