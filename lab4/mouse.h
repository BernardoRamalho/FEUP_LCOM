 #pragma once
  
 #include <stdbool.h>
 #include <stdint.h>
  

 #define DISABLE_MOUSE_BYTE 0xA7
 #define ENABLE_MOUSE_BYTE 0xA8
 #define CHECK_MOUSE_INTERFACE_BYTE 0xA8
 #define WRITE_BYTE_TO_MOUSE 0xD4
 #define DISABLE_MOUSE_BYTE 0xA7
 #define MOUSE_IRQ 12

 int mouse_subscribe(uint8_t *bit_no);
 int mouse_unsubscribe();
 void packet_byte_handler(int *position, uint8_t bytes[3]);
 void packet_handler(struct packet *pp, uint8_t packet[3]);
 int mouse_send_command(uint8_t command);
 void mouse_enable_interrupts();
 void mouse_disable_interrupts();
 int mouse_flush_OBF();
 int reset_KBC_command_byte(uint8_t byte);
 int check_if_IBF_full();
 void mouse_actions_analyser(bool *finished_inverted_V,struct packet pp,uint8_t x_len, uint8_t tolerance);
