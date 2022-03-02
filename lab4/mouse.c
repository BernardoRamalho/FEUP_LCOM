#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include "i8042.h"
#include "mouse.h"
#include <stdint.h>

uint8_t status_code, packet_byte;
int x_sum = 0,y_sum = 0, y_max_value;
static int hook_id = 2;
bool read_mouse = true;
enum mouse_states {initial, ascending, vertix, descending} state = initial;

void (mouse_ih)(){

  //Checks if it has any parity or time out error
  if ((status_code & PARITY_BIT) >> 7 | (status_code & TIME_OUT_BIT) >> 6 | !(status_code & AUX_BIT >> 5))
  {

    //if it has, it should discard de byte
    u_int8_t disposable_byte;

    if(util_sys_inb(OUT_BUF_REG, &disposable_byte)){
      printf("Error reading disposable byte.\n");

    }

    read_mouse=false;

    printf("Parity, Timeout Error or Keyboard.\n");
  }
  else{

    read_mouse=true;
    if(util_sys_inb(OUT_BUF_REG, &packet_byte)){
      printf("Error reading scan code.\n");
    }
  }

}

 int mouse_subscribe(uint8_t *bit_no){

   *bit_no = hook_id; 

    if(sys_irqsetpolicy(MOUSE_IRQ ,IRQ_REENABLE |IRQ_EXCLUSIVE, &hook_id)){
      printf("Error subscribing notification.\n");
      return 1;
    }

  return 0;
 }

  int mouse_unsubscribe(){

    if(sys_irqrmpolicy(&hook_id)){
      printf("Error unsubscribing notification.\n");
      return 1;
    }

  return 0;
  }


   void packet_byte_handler(int *position, uint8_t bytes[3]){

    //If it is the first byte and it doens't have bit 3 set to 1 then it can't be in position 0
    if ((*position) == 0 && !(packet_byte & BIT(3))){
      return;
    }

    //Else just save the packet byte into the bytes and add 1 to the positon               
    bytes[(*position)] = packet_byte;
    (*position)+=1;
          
  }
 
  void packet_handler(struct packet *pp, uint8_t packet[3]){
    //Save the packet bytes
    pp->bytes[0] = packet[0];
    pp->bytes[1] = packet[1];
    pp->bytes[2] = packet[2];

    //Save Overflow Values
    pp->x_ov = (packet[0] & BIT(6)) >> 6;
    pp->y_ov = (packet[0] & BIT(7)) >> 7;

    //Save Mouse Button Click
    pp->lb = packet[0] & BIT(0);
    pp->rb = (packet[0] & BIT(1)) >> 1;
    pp->mb = (packet[0] & BIT(2)) >> 2;

    //Save Mouse Movement on the X axis
    if(packet[0] & MSB_X_DELTA_BIT){
      pp->delta_x = (packet[1] | EXTEND_8_TO_16);
    }else{
      pp->delta_x = packet[1];
    }

    //Save Mouse Movement on the Y axis
    if(packet[0] & MSB_Y_DELTA_BIT){
      pp->delta_y = (packet[2] | EXTEND_8_TO_16);
    }else{
      pp->delta_y = packet[2];
    }

  }

  int mouse_send_command(uint8_t command){
    uint8_t ack_byte;

    while(true){

      if(check_if_IBF_full()){
        continue;
      }
      
      //Send the Write Command
      sys_outb(KBC_COMMAND_REG, WRITE_BYTE_TO_MOUSE);

      if(check_if_IBF_full()){
        continue;
      }

      //Sends the Command Argument
      sys_outb(INPUT_BUF_REG, command);

      //Checks the ACK byte
      util_sys_inb(OUT_BUF_REG,&ack_byte);
      if (ack_byte != ACK)
      {
        continue;
      }
      return 0;
    }
    return 1;
}


void mouse_enable_interrupts(){
  sys_irqenable(&hook_id);

}
void mouse_disable_interrupts(){
  sys_irqdisable(&hook_id);
}

int mouse_flush_OBF(){

  uint8_t byte_To_flush;

    //Reads the status code
    if(util_sys_inb(STAT_REG, &status_code)){
      printf("Error reading status code."); 
      return 1;                 
    }
    
   //Checks if the OBF is full and if it is, cleans it
   if(status_code & OBF_BIT) {

     if(util_sys_inb(OUT_BUF_REG, &byte_To_flush)){
      printf("Error reading scan code.");
      return 1;
    }

  } 
   return 0;
}


  int reset_KBC_command_byte(uint8_t byte){

    while(true){

      if(check_if_IBF_full()){
        continue;
      }
      
      //Send the Command
      sys_outb(KBC_COMMAND_REG, WRITE_COMMAND_BYTE);

      if(check_if_IBF_full()){
        continue;
      }

      //Sends the Command Byte
      sys_outb(INPUT_BUF_REG, byte);     
      return 0;
    }
    return 1;
}

int check_if_IBF_full(){
  
  uint8_t status;

  util_sys_inb(STAT_REG, &status);

  //The IBF is full if the IBF_BIT is set to 1 in the status
  if(status & IBF_BIT){
    return 1;
  }

  return 0;
}


 void mouse_actions_analyser(bool *finished_inverted_V, struct packet pp, uint8_t x_len, uint8_t tolerance){
   
   
   switch (state)
   {
     //Initial State
    case 0:

      /*If the left button is pressed and no other is, then it started the ascending part of the movement
      It also can't have any descending movement (delta_x or delta_y < 0) bigger then the tolerance*/
      if (pp.lb == 1 && pp.rb == 0 && pp.mb == 0 && (pp.delta_x + tolerance) >= 0 && (pp.delta_y + tolerance) >= 0){
        state = ascending;

        //Add any movement that's been made
        x_sum += pp.delta_x;
        y_sum += pp.delta_y;
      }

      break;

    //Ascending State
    case 1:

      //If the left mouse button is release (lb = 0), the movement in the x axis is bigger then the
      //minimum x_len and the slope must be bigger then 1 (the max y value must be bigget then x)
      if (pp.lb == 0 && pp.rb == 0 && pp.mb == 0 && x_sum >= x_len && y_sum >= x_sum) {
        state = vertix;
        x_sum = 0; //Resets the x value because it will start a new line
      }
      //If the left mouse button is still pressed and no other is, then add the x value and the y and resume in ascending state
      else if (pp.lb == 1 && pp.rb == 0 && pp.mb == 0 && (pp.delta_x + tolerance) >= 0 && (pp.delta_y + tolerance) >= 0)      {
        x_sum += pp.delta_x;
        y_sum += pp.delta_y;
      }
      //If none of the other two condition are met then it must begin from the start
      else{
          state = initial;
          x_sum = 0;
          y_sum = 0;
      }

      break;

    //Vertix
    case 2:

      //If the mouse is in the vertix and there are no buttons pressed then it can have some
      //residual movement (the movement must be smaller then the tolerance)
      if(pp.lb == 0 && pp.rb == 0 && pp.mb == 0 && abs(pp.delta_x) <= tolerance && abs(pp.delta_y) <= tolerance){
        break;
     }
     //If the right button is pressed and no other is, then the mouse will start descending
     else if(pp.lb == 0 && pp.rb == 1 && pp.mb == 0 && abs(pp.delta_x) <= tolerance && abs(pp.delta_y) <= tolerance){
       state = descending;
       y_max_value = y_sum; //this is needed to calculate the slope of the descending line
     }
     //If none of the other two condition are met then it must begin from the start
     else{
       state = initial;
       x_sum = 0;
       y_sum = 0;
     }
     break;

     //Descending State
     case 3:

      /*If neither the left mouse button nor the middle mouse button is pressed, the displacement 
      along the x axis is bigger then x_len and the y value is bigger then the x, the inverted V
      is completed*/
      if(pp.lb == 0 && pp.mb == 0  && x_sum >= x_len && y_max_value >= x_sum){
        *finished_inverted_V = true;
      }
      /*If the right button and no other is pressed and the x and y displacement is within the 
      tolerance, then the mouse is still descending*/
      else if(pp.lb == 0 && pp.rb == 1 && pp.mb == 0 && (pp.delta_x + tolerance) >= 0 && (tolerance - pp.delta_y) >= 0){
        x_sum += pp.delta_x;
      }
      //If none of the other two condition are met then it must begin from the start
      else {
        state = initial;
        x_sum = 0;
        y_sum = 0;
      }
      break;
     default:
       break;
   }
 }
