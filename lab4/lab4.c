// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <i8042.h>
#include "mouse.h"
#include <stdint.h>
#include <stdio.h>

extern uint8_t status_code;
extern bool read_mouse;

extern uint32_t global_counter;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {

  uint8_t  bit_no;
  int ipc_status, r, irq_set = 0;
  message msg;
  int position =0;
  uint8_t packet[3];

  if(mouse_subscribe(&bit_no)){
    printf("Error subscribing in lab4.\n");
    return 1;
  }

  irq_set = BIT(bit_no);

  //Enable Mouse Data Reporting
  mouse_disable_interrupts();
  mouse_send_command(MOUSE_EN_DATA_REPORTING);
  mouse_enable_interrupts();
 

  while( cnt > 0) { 
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0){ 
          printf("driver_receive failed with: %d", r);
         

          continue;

      }
      if (is_ipc_notify(ipc_status)) { // received notification
          
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE: /* hardware interrupt notification */	

                  if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                  
                    //Reading status code
                    if(util_sys_inb(STAT_REG, &status_code)){
                      printf("Error reading status code.");                  
                    }

                    //Checks if the OBF is full
                    if(!(status_code & OBF_BIT))
                    {
                      continue;
                    }

                    mouse_ih();

                    if (read_mouse){
                      //Saves the new byte into the packet 
                      packet_byte_handler(&position, packet);

                      //Checking wether we have full scan code or not
                      //If its full it should print the number
                      if (position == 3) {
                        
                        position = 0; //Reset position for the next packet

                        struct packet pp; 

                        packet_handler(&pp,packet); //Save the information into the struct packet

                        mouse_print_packet(&pp);

                        cnt--;
                      }
                    }          
                  } 
                break;

              default:
                break; /* no other notifications expected: do nothing */	
          }
      }
  }
  //Disables Mouse Data Reporting
  mouse_disable_interrupts();
  mouse_send_command(MOUSE_DIS_DATA_REPORTING);
  mouse_enable_interrupts();

  //Unsubscribing to the Mouse
  if(mouse_unsubscribe()){
    printf("Error unsubscribing mouse in lab4.\n");
  }

  //Checks if the OBF is full and if it is, it cleans it
  mouse_flush_OBF();

  return 0;
}


int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    int position =0;
    uint8_t packet[3];

    while(cnt > 0){

      //If the position is 0 then we must request the mouse to send a new packet data
      if(position == 0){
        mouse_send_command(MOUSE_READ_DATA);
      }

      //Reading status code
      if(util_sys_inb(STAT_REG, &status_code)){
        printf("Error reading status code.");                  
      }

      //Checks if the OBF is full
      if(!(status_code & OBF_BIT))
      {
        continue;
      }

      mouse_ih();

      if (read_mouse){
        //Saves the new byte into the packet 
        packet_byte_handler(&position, packet);

        //Checking wether we have full scan code or not
        //If its full it should print the number
        if (position == 3) {
          
          position = 0; //Reset position for the next packet

          struct packet pp; 

          packet_handler(&pp,packet); //Save the information into the struct packet

          mouse_print_packet(&pp);

          cnt--;
        }
      }
  }

  //Reenable Streaming Mode and Disable Mouse Data Reporting
  mouse_send_command(MOUSE_SET_STREAM_MODE);
  mouse_send_command(MOUSE_DIS_DATA_REPORTING);

  //Resets the KBC command to the default command byte
  uint8_t default_KBC_byte = minix_get_dflt_kbc_cmd_byte();
  reset_KBC_command_byte(default_KBC_byte) ;

  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    
  uint8_t  bit_no;
  int ipc_status, r, irq_mouse = 0, irq_timer0 = 0;
  message msg;
  int position =0;
  uint8_t packet[3];

  //Subscribing to the timer
  if(timer_subscribe_int(&bit_no)){
    printf("Error subs return 1;cribing timer in lab3.\n");
    return 1;
  }

  irq_timer0 = BIT(bit_no);

  //Subscribe to the Mouse
  if(mouse_subscribe(&bit_no)){
    printf("Error subscribing in lab4.\n");
    return 1;
  }

  irq_mouse = BIT(bit_no);

  //Enable Mouse Data Reporting
  mouse_disable_interrupts();
  mouse_send_command(MOUSE_EN_DATA_REPORTING);
  mouse_enable_interrupts();
 

  while(global_counter < idle_time * (uint8_t) sys_hz()) { 

      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0){ 
          printf("driver_receive failed with: %d", r);
          continue;
      }

      if (is_ipc_notify(ipc_status)) { // received notification
          
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE: /* hardware interrupt notification */	

                  //Handles Mouse Interrupt
                  if (msg.m_notify.interrupts & irq_mouse) { /* subscribed interrupt */
                  
                    //Reading status code
                    if(util_sys_inb(STAT_REG, &status_code)){
                      printf("Error reading status code.");                  
                    }

                    //Check if OBF is full
                    if(!(status_code & OBF_BIT))
                    {
                      continue;
                    }

                    global_counter = 0; //Resets the timer betweent keyboard interrupts

                    mouse_ih();

                    if (read_mouse){
                      //Saves the new byte into the packet 
                      packet_byte_handler(&position, packet);

                      //Checking wether we have full scan code or not
                      //If its full it should print the number
                      if (position == 3) {
                        
                        position = 0; //Reset position for the next packet

                        struct packet pp; 

                        packet_handler(&pp,packet); //Save the information into the struct packet

                        mouse_print_packet(&pp);
                      }
                    }             
                  }

                  //Handles Timer Interrupt
                  if (msg.m_notify.interrupts & irq_timer0) { /* subscribed interrupt */
                    timer_int_handler();
                  } 
                  
                break;
              default:
                break; /* no other notifications expected: do nothing */	
          }
      }
  }

  //Disable Mouse Data Reporting
  mouse_disable_interrupts();
  mouse_send_command(MOUSE_DIS_DATA_REPORTING);
  mouse_enable_interrupts();

  //Unsubscribing to the Mouse
  if(mouse_unsubscribe()){
    printf("Error unsubscribing mouse in lab4.\n");
  }

  //Unsubscribing to the Timer
  if(timer_unsubscribe_int()){
    printf("Error unsubscribing timer in lab3.\n");
    return 1;
  }

  //Checks if the OBF is full and if it is, it cleans it
  mouse_flush_OBF();
  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  uint8_t  bit_no;
  int ipc_status, r, irq_set = 0;
  message msg;
  int position =0;
  uint8_t packet[3];
  bool finished_inverted_V = false;

  if(mouse_subscribe(&bit_no)){
    printf("Error subscribing in lab4.\n");
    return 1;
  }

  irq_set = BIT(bit_no);

  //Enable Mouse Data Reporting
  mouse_disable_interrupts();
  mouse_send_command(MOUSE_EN_DATA_REPORTING);
  mouse_enable_interrupts();
 

  while(!finished_inverted_V) { 
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0){ 
          printf("driver_receive failed with: %d", r);
         

          continue;

      }
      if (is_ipc_notify(ipc_status)) { // received notification
          
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE: /* hardware interrupt notification */	

                  if (msg.m_notify.interrupts & irq_set) { /* subscribed interrupt */
                  
                    //Reading status code
                    if(util_sys_inb(STAT_REG, &status_code)){
                      printf("Error reading status code.");                  
                    }

                    //Checks if the OBF is full
                    if(!(status_code & OBF_BIT))
                    {
                      continue;
                    }

                    mouse_ih();

                    if (read_mouse){
                      //Saves the new byte into the packet 
                      packet_byte_handler(&position, packet);

                      //Checking wether we have full scan code or not
                      //If its full it should print the number
                      if (position == 3) {
                        
                        position = 0; //Reset position for the next packet

                        struct packet pp; 

                        packet_handler(&pp,packet); //Save the information into the struct packet

                        mouse_print_packet(&pp);

                        mouse_actions_analyser(&finished_inverted_V,pp,x_len,tolerance); //Analyses the movement of the mouse
                      }
                    }          
                  } 
                break;

              default:
                break; /* no other notifications expected: do nothing */	
          }
      }
  }
  //Disables Mouse Data Reporting
  mouse_disable_interrupts();
  mouse_send_command(MOUSE_DIS_DATA_REPORTING);
  mouse_enable_interrupts();

  //Unsubscribing to the Mouse
  if(mouse_unsubscribe()){
    printf("Error unsubscribing mouse in lab4.\n");
  }

  //Checks if the OBF is full and if it is, it cleans it
  mouse_flush_OBF();

  return 0;
}

