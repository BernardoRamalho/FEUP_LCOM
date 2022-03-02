#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include "i8042.h"
#include "i8254.h"

#include <stdbool.h>
#include <stdint.h>

#define ESC_BREAK_CODE 0x81

extern uint8_t status_code, scan_code;
extern uint32_t global_counter;

int count_sysb;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {

  int ipc_status, r, irq_set = 0;
  message msg;
  uint8_t  bit_no;
  
  if(keyboard_subscribe(&bit_no)){
    printf("Error subscribing in lab3.\n");
    return 1;
  }

  irq_set = BIT(bit_no);

  while( scan_code != ESC_BREAK_CODE ) { 
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
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

                  //Calling interrupt Handler
                  kbc_ih();

                  //Checking wether we have full scan code or not
                  //If its full it should print the number
                  print_full_scan_code();
              } 
                
              break;
            default:
              break; /* no other notifications expected: do nothing */	
        }
    }
 }

 //Checks if the OBF is full and if it is cleans it
  if(flush_OBF()){
    printf("Error flushing OBS.\n");
    return 1;
  }

 if(keyboard_unsubscribe()){
      printf("Error unsubscribing in lab3.\n");
      return 1;
    }

  kbd_print_no_sysinb(count_sysb);

  return 0;
}

int(kbd_test_poll)() {


  while( scan_code != ESC_BREAK_CODE ) { 

      //Reading Status Code
      if(util_sys_inb(STAT_REG, &status_code)){
        printf("Error reading status code.");
        return 1;
      }

      //Checks if OBF is full 
      if(!(status_code & OBF_BIT)) {
        continue;
      }

      //Checks if the interrupt is from the keyboard or the mouse
      if(status_code & AUX_BIT){
        continue;
      }
      
      //Calls the interrupt Handler
      kbc_ih();

      //Checking wether we have full scan code or not
      //If its full it should print the number
      print_full_scan_code();

      tickdelay(micros_to_ticks(DELAY_US));
      
  }

  //Checks if the OBF is full and if it is cleans it
  if(flush_OBF()){
    printf("Error flushing OBS.\n");
    return 1;
  }

  if(reenable_keyboard_interrupt()){
    printf("Error reenableing keyboard.\n");
    return 1;
  }


  kbd_print_no_sysinb(count_sysb);

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  int ipc_status, r, irq_kbd = 0, irq_timer0 = 0;
  message msg;
  uint8_t bit_no;


  //Subscribing to the timer
  if(timer_subscribe_int(&bit_no)){
    printf("Error subs return 1;cribing timer in lab3.\n");
    return 1;
  }
  irq_timer0 = BIT(bit_no);

  //Subscribing to the keyboard
  if(keyboard_subscribe(&bit_no)){
    printf("Error subscribing keyboard in lab3.\n");
    return 1;
  }
  irq_kbd = BIT(bit_no);

  while( scan_code != ESC_BREAK_CODE && global_counter < n * (uint8_t) sys_hz()) { 

    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    
    if (is_ipc_notify(ipc_status)) { // received notification
        
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */	

              if (msg.m_notify.interrupts & irq_kbd) { /* subscribed interrupt */
                
                if(util_sys_inb(STAT_REG, &status_code)){
                  printf("Error reading status code.");
                  return 1;
                }

                global_counter = 0; //Resets the timer betweent keyboard interrupts

                //Calls the keyboard interrupt
                kbc_ih();

                print_full_scan_code();
              } 
              if (msg.m_notify.interrupts & irq_timer0) { /* subscribed interrupt */
                  timer_int_handler();
              }
              
              break;
            default:
              break; /* no other notifications expected: do nothing */	
        }
    }
  }

  //Checks if the OBF is full and if it is cleans it
  if(flush_OBF()){
    printf("Error flushing OBS.\n");
    return 1;
  }

  if(keyboard_unsubscribe()){
    printf("Error unsubscribing in lab3.\n");
    return 1;
  }

  if(timer_unsubscribe_int()){
    printf("Error unsubscribing timer in lab3.\n");
    return 1;
  }

  
  
  return 0;
}
