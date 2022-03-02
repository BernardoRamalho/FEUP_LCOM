#include "Serial_port.h"

int sp_hook_id = 4;

int uart_subscribe(uint8_t *bit_no){

  *bit_no = sp_hook_id;

  if(sys_irqsetpolicy(COM1_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE), &sp_hook_id) == OK){
    return 0;
  }

  return 1;
}

int uart_unsubscribe(){

  if(sys_irqrmpolicy(&sp_hook_id) == OK){
    return 0;
  }

  return 1;
}

void uart_config(){

  int start_addr = COM1;

  uint8_t load_byte;

  //Serial Port Config to change bit rate
  load_byte = (LCR_BITS_PER_CHAR_8 | LCR_STOP_BITS_1 | LCR_ODD_PARITY | LCR_DLAB); 
  sys_outb(start_addr + LINE_CONTROL_REG, load_byte);
  
  //Bit Rate Config to 57600 
  sys_outb(start_addr + DL_LSB, 0x02);
  sys_outb(start_addr + DL_MSB, 0x00);

  //Serial Port Config Interrupts
  load_byte = (LCR_BITS_PER_CHAR_8 | LCR_STOP_BITS_1 | LCR_ODD_PARITY); 
  sys_outb(start_addr + LINE_CONTROL_REG, load_byte);

  //Config FIFO

  //Making the UART Interrupt whenever the FIFO gets 1 byte
  load_byte = (FIFO_INTERRUPT_TRIGGER_LEVEL_1 |ENABLE_FIFO | FCR_CLEAR_RECEIVE_FIFO | FCR_CLEAR_TRANSMIT_FIFO);
  sys_outb(start_addr + FIFO_CONTROL_REG, load_byte);

  //Config IER
  load_byte = (IER_ENABLE_RECEIVED_DATA_INTERRUPT | IER_ENABLE_RECEIVER_LINE_STATUS_INTERRUPT);
  sys_outb(start_addr + INTERRUPT_ENABLE_REG, load_byte);

  return;
}

void uart_ih(char ** message, int * size){

  uint8_t iir_byte = 0;

  util_sys_inb(COM1 + INTERRUPT_INDICATION_REG, &iir_byte);

  uint8_t message_part;

  while((iir_byte & INTERRUPT_ORIGIN_RECEIVED_DATA) && !(iir_byte & INTERRUPT_ORIGIN_TRANSMITTER_EMPTY) ){
    
    util_sys_inb(COM1, &message_part);

    (*size)++;

    *message = (char *) realloc(*message, sizeof(char)*(*size));

    (*message)[(*size) - 1] = message_part;

    util_sys_inb(COM1 + INTERRUPT_INDICATION_REG, &iir_byte);

  }

  return;
}

bool uart_check_full_message(char ** message, int * size){

  if(*size < 1){
    return false;
  }

  switch((*message)[0]){
    case 'P':
      if(*size == 2){
        return true;
      }
      return false;
      break;

    case 'S':
      if(*size == 2){
        return true;
      }
      return false;
      break;

    case 'D':
      if(*size == 3){
        return true;
      }
      return false;
      break;

    case 'M':
      if(*size == 3){
        return true;
      }
      return false;
      break;
    default:
      *size = 0;
      free(*message);
      *message = NULL;
      return false;
  }
}

void uart_send_byte(uint8_t info){

  uint8_t lsr_byte;

  util_sys_inb(COM1 + LINE_STATUS_REG, &lsr_byte);

  while(! (lsr_byte & LSR_TRANSMITTER_EMPTY)){
    util_sys_inb(COM1 + LINE_STATUS_REG, &lsr_byte);
  }
  
  sys_outb(COM1, info);
}

void uart_send_message(char * message, int size){

  for(int i = 0; i < size; i++){
    uart_send_byte(message[i]);
  }
  
  return;
}

void uart_clear_buffer(){
  uint8_t uart_status;

  util_sys_inb(COM1 + LINE_STATUS_REG, &uart_status);

  while(uart_status & LSR_RECEIVER_READY){
    uint8_t disposable_byte;
    util_sys_inb(COM1, &disposable_byte);
    util_sys_inb(COM1 + LINE_STATUS_REG, &uart_status);
  }
}

