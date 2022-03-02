#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

uint32_t global_counter = 0; 
int hook_id = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  
  
  if(freq > TIMER_FREQ || freq < 19){
    printf("Invalid freq value(freq must be between 19 and TIMER_FREQ).\n");
    return 1;
  }

  uint8_t status;
  if (timer_get_conf(timer,&status)) {
    printf("Error retrieving timer status.\n");
    return 1;
  }

  uint8_t selectedTimer, timerMask; //selectedTimer: Reference to the Timer; timerMask: timer counter bits for the control word

  //Checking which of the 3
  switch(timer){
    case 0:
      selectedTimer = TIMER_0;
      timerMask = TIMER_SEL0;
      break;
    case 1:
      selectedTimer = TIMER_1;
      timerMask = TIMER_SEL1;
      break;
    case 2:
      selectedTimer = TIMER_2;
      timerMask = TIMER_SEL2;
      break;
  }

  //Creating Control Word
  uint8_t mode = (BIT(3) | BIT(2) | BIT(1) ) &  status;
  uint8_t bcd = (BIT(0)) &  status;
  uint8_t controlWord = mode | bcd | timerMask | TIMER_LSB_MSB; 

  //Sending Control Word to the timer controler
  if (sys_outb(TIMER_CTRL,controlWord))
  {
    printf("Error sending control word.\n");
    return 1;
  }

  //Divisor to generate the frequency corresponding to the desired rate
  uint16_t divisor = TIMER_FREQ / freq; 

  //Getting LSB and MSB values
  uint8_t lsb;
  if(util_get_LSB(divisor, &lsb)){
    printf("Error saving value to lsb.\n");
    return 1;
  }

  uint8_t msb;

  if(util_get_MSB(divisor, &msb)){
    printf("Error saving value to msb.\n");
    return 1;
  }

  //Sending LSB and MSB to the selectedTimer
  if(sys_outb(selectedTimer, lsb)){
    printf("Error sending lsb.\n");
    return 1;
  }

  if(sys_outb(selectedTimer, msb)){
    printf("Error sending msb.\n");
    return 1;
  }
  
  return 0;
  
}


int (timer_subscribe_int)(uint8_t *bit_no) {
    
    *bit_no = hook_id; 

    if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id)){
      printf("Error subscribing notification.\n");
      return 1;
    }

  return 0;
}

int (timer_unsubscribe_int)() {
  
  if(sys_irqrmpolicy(&hook_id)){
    printf("Error unsubscribing notification.\n");
    return 1;
  }

  return 0;
}

void (timer_int_handler)() { 
  global_counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  
 
  //Creating and sending Read Back Command to Timer controler
  uint8_t readbackCommand = TIMER_RB_CMD | TIMER_RB_SEL(timer)  |TIMER_RB_COUNT_;

  if (sys_outb(TIMER_CTRL,readbackCommand))
  {
    printf("Error sending readback command.\n");
    return 1;
  }

  //Getting the reference of the timer
  uint8_t selectedTimer;

  switch(timer){
    case 0:
      selectedTimer = TIMER_0;
      break;
    case 1:
      selectedTimer = TIMER_1;
      break;
    case 2:
      selectedTimer = TIMER_2;
      break;
  }

  //Saving configuration in st
  if(util_sys_inb(selectedTimer, st)){
    printf("Error retrieving timer status.\n");
  }

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {

  union timer_status_field_val field_value;

  switch (field)
  {
    case tsf_all :
      field_value.byte = st;
      break;

    case tsf_initial:

      field_value.in_mode = (st & TIMER_LSB_MSB) >> 4;
      break;

    case tsf_mode:
      field_value.count_mode = (st & TIMER_SQR_WAVE) >> 1;
      break;

    case tsf_base:
      field_value.bcd = (st & TIMER_BCD);
      break;
    default:
      printf("Error assigning value to the union.\n");
      break;
 }

  if(timer_print_config(timer, field, field_value)){
    printf("Error printing configuration value.\n");
  }

  return 0;
}
