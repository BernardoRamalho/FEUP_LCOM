#include "RTC.h"
#include "Terminix.h"

int rtc_subscribe(uint8_t *bit_no) {

  *bit_no = hook_id_rtc;

  if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_rtc)) {
    printf("Error setting IRQ policy.\n");
    return 1;
  }

  return 0;
}

int rtc_unsubscribe() {

  if (sys_irqrmpolicy(&hook_id_rtc)) {
    printf("Error unsubscribing notification.\n");
    return 1;
  }

  return 0;
}

int rtc_write(uint8_t data, uint8_t address) {

  if (sys_outb(RTC_ADDR_REG, address)) {
    printf("Failed to write to register in RTC");
    return 1;
  }

  if (sys_outb(RTC_DATA_REG, data)) {
    printf("Failed to write data in RTC");
    return 1;
  }

  return 0;
}

int rtc_read(uint8_t *data, uint8_t address) {

  if (sys_outb(RTC_ADDR_REG, address)) {
    printf("Failed to write to register in RTC");
    return 1;
  }

  if (util_sys_inb(RTC_DATA_REG, data)) {
    printf("Failed to read data from RTC");
    return 1;
  }
  return 0;
}

void rtc_ih() {


  uint8_t data;

  if(rtc_read(&data,RTC_REG_C)){
    printf("Error reading data from REG C\n");
    return;
  }

  if (data & RTC_REG_C_UF) {
   // printf("Esstou a receber dos update interrupts\n");
    // handle_update();
  }
  if (data & RTC_REG_C_AF) {

    Terminix * terminix = get_current_terminix();
    if(terminix->state == HIGHSCORE_STATE){
      terminix->menu->easterEgg = true;
    }
  

  }
  if (data & RTC_REG_C_PF) {
    //printf("Esstou a receber dos periodic interrupts\n");

    // handle_periodic();
  }
}

int rtc_get_month() {
  uint8_t byte = 0;
  rtc_read(&byte, RTC_MONTH);
  return convert_to_decimal(byte);
}

int rtc_get_year() {
  uint8_t byte = 0;
  rtc_read(&byte, RTC_YEAR);

  return convert_to_decimal(byte);
}

int rtc_get_day() {
  uint8_t byte = 0;
  rtc_read(&byte, RTC_DAY);
  return convert_to_decimal(byte);
}

int rtc_get_hour() {
  uint8_t byte = 0;
  rtc_read(&byte, RTC_HOUR);

  return convert_to_decimal(byte);
}

int rtc_get_min() {
  uint8_t byte = 0;
  rtc_read(&byte, RTC_MINT);

  return convert_to_decimal(byte);
}

int rtc_get_sec() {
  uint8_t byte = 0;
  rtc_read(&byte, RTC_SEC);

  return convert_to_decimal(byte);
}

int convert_to_decimal(uint32_t bcd) {
  return (10 * ((bcd & 0xF0) >> 4)) + (bcd & 0x0F);
}


int rtc_enable_alarm() {

  uint8_t data;

  if(rtc_read(&data,RTC_REG_C)){
    printf("Reading discard\n");
    return 1;
  }

  if (rtc_read(&data, RTC_REG_B)) {
    printf("Error reading register b from rtc\n");
    return 1;
  }
  data = data | RTC_REG_B_AIE;

  if (rtc_write(data, RTC_REG_B)) {
    printf("Error enabling alarm\n");
    return 1;
  }

  return 0;
}

int rtc_disable_alarm() {

  uint8_t data;

  if (rtc_read(&data, RTC_REG_B)) {
    printf("Error reading from REG B RTC");
    return 1;
  }

  // Making the AIE Bit 0 again
  data = data & (uint8_t)~RTC_REG_B_AIE;

  if (rtc_write(data, RTC_REG_B)) {
    printf("Error writing data to Register B");
    return 1;
  }

  return 0;
}

int set_alarm_second(uint8_t second) { 
  return rtc_write(second,RTC_ALARM_SEC);
}

int set_alarm_minute(uint8_t minute) {
  return rtc_write(minute, RTC_ALARM_MINT);
}

int set_alarm_hour(uint8_t hour) {
  return rtc_write(hour, RTC_ALARM_HOUR);
}

int rtc_set_alarm_in_seconds(int seconds){
  int actualSecond = rtc_get_sec();
  set_alarm_second(actualSecond);
  set_alarm_minute(RTC_DONT_CARE_ALARM);
  set_alarm_hour(RTC_DONT_CARE_ALARM);

  return 0;
}

int rtc_update_enable(){

    uint8_t data;

    if(rtc_read(&data,RTC_REG_B)){
    	printf("Error reading from register B\n");
      return 1;
    }
    data = data | RTC_REG_B_UIE;

    if (rtc_write(data, RTC_REG_B)) {
      printf("Error writing data to Register B");
      return 1;
    }

    return 0;
}

int rtc_update_disable(){

    uint8_t data;

    if(rtc_read(&data,RTC_REG_B)){
    	printf("Error reading from register B\n");
      return 1;
    }

    
    data &= (uint8_t) ~RTC_REG_B_UIE;

    if (rtc_write(data, RTC_REG_B)) {
      printf("Error writing data to Register B");
      return 1;
    }
    return 0;
}

int rtc_enable_periodic(){

    uint8_t data;


    if(rtc_read(&data,RTC_REG_B)){
    	printf("Error reading from register B\n");
      return 1;
    }

    data |= RTC_REG_B_PIE;

    if (rtc_write(data, RTC_REG_B)) {
      printf("Error writing data to Register B");
      return 1;
    }
    return 0;
}

int rtc_disable_periodic(){

    uint8_t data;

    if(rtc_read(&data,RTC_REG_B)){
    	printf("Error reading from register B\n");
      return 1;
    }

  
    data &= (uint8_t) ~RTC_REG_B_PIE;

    if (rtc_write(data, RTC_REG_B)) {
      printf("Error writing data to Register B");
      return 1;
    }
    return 0;
}



