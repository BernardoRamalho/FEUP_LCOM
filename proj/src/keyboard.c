#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include "i8042.h"
#include <stdint.h>
#include "Game.h"

uint8_t status_code, scan_code;
static int hook_id = 1;
extern int count_sysb;

void (kbc_ih)(){


  if (util_sys_inb(STAT_REG, &status_code)){
    printf("Error reading status code");
    return;
  }

  //Checks if it has any parity or time out error
  if ((status_code & PARITY_BIT) >> 7 | (status_code & TIME_OUT_BIT) >> 6)
  {
    printf("Parity or Timeout Error.\n");
  }

  if(util_sys_inb(OUT_BUF_REG, &scan_code)){
    printf("Error reading scan code.\n");
  }

 }

 int keyboard_subscribe(uint8_t *bit_no){

   *bit_no = hook_id; 

    if(sys_irqsetpolicy(KEYBOARD_IRQ , IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id)){
      printf("Error subscribing notification.\n");
      return 1;
    }

  return 0;
 }

  int keyboard_unsubscribe(){

    if(sys_irqrmpolicy(&hook_id)){
      printf("Error unsubscribing notification.\n");
      return 1;
    }

    return 0;
  }

  int got_full_scan_code(){

    //If a scan_code is iqual to 0xE0 then it will have two bytes
    if(scan_code == INCOMPLETE_SCAN_CODE){
        return 1;
    }
    return 0;
  }

  int reenable_keyboard_interrupt(){
    u_int8_t kbc_command_byte;

    //Sending Read Command Byte
    if(sys_outb(KBC_COMMAND_REG,READ_COMMAND_BYTE)){
      printf("Error sending command byte.\n");
      return 1;
    }

    //Reading KBC Command Byte
    if(util_sys_inb(OUT_BUF_REG, &kbc_command_byte)){
      printf("Error reading command byte.\n");
      return 1;
    }

    //INT0 must be = 0 to reenable the keyboard
    kbc_command_byte |= INT0;

    //Sending Write Command Byte
    if(sys_outb(KBC_COMMAND_REG, WRITE_COMMAND_BYTE)){
      printf("Error sending write command byte.\n");
      return 1;
    }

    //Sending Argument for the Write Command Byte
    if(sys_outb(KBC_ARGUMENT_REG, kbc_command_byte)){
      printf("Error sending argument.\n");
      return 1;
    }

    return 0;
  }

  int flush_OBF(){

    //Reads the status code
    if(util_sys_inb(STAT_REG, &status_code)){
      printf("Error reading status code."); 
      return 1;                 
    }
    
   //Checks if the OBF is full and if it is, cleans it
    if(status_code & OBF_BIT) {

     if(util_sys_inb(OUT_BUF_REG, &scan_code)){
      printf("Error reading scan code.");
      return 1;
      }

    } 
   return 0;
}

 int keyboard_handler(){

   Terminix *terminix= get_current_terminix();
   int hero_index = terminix->nr_player - 1;

   
  kbc_ih();

  switch (terminix->state)
  {
  case MENU_STATE:
    break;

  case LEVEL_STATE:

    if(terminix->heroes[hero_index] == NULL){
      break;
    }

    if (scan_code == W_MAKE_CODE){
      hero_move_up(terminix->heroes[hero_index]);

      if(terminix->multiplayer){
      send_movement_info(terminix, 'u');
      }
    }

    if (scan_code == S_MAKE_CODE){
      hero_move_down(terminix->heroes[hero_index]);

      if(terminix->multiplayer){
        send_movement_info(terminix, 'd');
      }
    }

    if (scan_code == A_MAKE_CODE){
      hero_move_left(terminix->heroes[hero_index]);

      if(terminix->multiplayer){
        send_movement_info(terminix, 'l');
      }

    }
    if (scan_code == D_MAKE_CODE){
      hero_move_right(terminix->heroes[hero_index]);

      if(terminix->multiplayer){
        send_movement_info(terminix, 'r');
      }

    }
    break;
  case END_STATE:
    if (scan_code == SPACE_MAKE_CODE){

    }
    
  
  default:
    break;
  }
 
  return 0;
  
}
