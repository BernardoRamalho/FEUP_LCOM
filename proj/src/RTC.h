#pragma once
#include <lcom/lcf.h>


 #ifndef _RTC_H_
 #define _RTC_H_

 /** @defgroup RTC RTC
 * @{
 * Responsible for all the operations regarding the RTC
 */

#define RTC_IRQ 8
#define RTC_ADDR_REG 0x70 //Writing the address of the register 
#define RTC_DATA_REG 0x71 //Reading/writing one byte from/to


//Register A
#define RTC_REG_A 0x0A	
#define RTC_REG_A_UIP BIT(7) //If set to one, update in progress


//Register B
#define RTC_REG_B 0x0B
#define RTC_REG_B_SET BIT(7) //Set to one to inhibit updates of time registers
#define RTC_REG_B_PIE BIT(6) //Set to one to enable the correspondent interrupt source
#define RTC_REG_B_AIE BIT(5)
#define RTC_REG_B_UIE BIT(4)
#define RTC_REG_B_SQWE BIT(3) //set to one to enale square wave generation
#define RTC_REG_B_DM BIT(2)   //Set to 1 to set time alarm and date registers in binary
                              // set to one for BCD DO NOT CHANGE
#define RTC_REG_B_2412 BIT(1) //set to 1 to set the hours between 0 to 23

#define RTC_DONT_CARE_ALARM BIT(6) | BIT(7) 

//Register C
#define RTC_REG_C 0x0C
#define RTC_REG_C_PF BIT(6)
#define RTC_REG_C_AF BIT(5)
#define RTC_REG_C_UF BIT(4)

#define RTC_SEC 0x00	
#define RTC_ALARM_SEC 0x01	
#define RTC_MINT 0x02	
#define RTC_ALARM_MINT 0x03	
#define RTC_HOUR 0x04	
#define RTC_ALARM_HOUR 0x05
#define RTC_DAY 0x07
#define RTC_MONTH 0x08	
#define RTC_YEAR 0x09	


static int hook_id_rtc = 8;

//  The first 10 registers are used to access clock-related information, such as date, time or alarm time. The next 4 registers are used for controlling that functionality. The remaining registers can be used as non-volatile memory for arbitrary purposes.


/**
 * @brief Subscribes RTC interrupts
 * @return 0 if the operation wasn't succssessfull  
 */
int rtc_subscribe(uint8_t *bit_no);


/**
 * @brief Handles RTC interrupts
 * Shows an easter egg if the user is in the Highscore Menu when the alarm rings
 * Handles periodic, update and alarm interrupts
 */
void rtc_ih();

/**
 * @brief Unsubscribes RTC interrupts
 * @return Returns 0 if successful, not 0 otherwise
 */
int rtc_unsubscribe();

/**
 * @brief Gets the current year
 * @return returns the year 
 */
int rtc_get_year();

/**
 * @brief Gets the current month
 * @return returns the month 
 */
int rtc_get_month();

/**
 * @brief Gets the current day
 * @return returns the day 
 */
int rtc_get_day();

/**
 * @brief Gets the current hour
 * @return returns the hour 
 */
int rtc_get_hour();

/**
 * @brief Gets the current minute
 * @return returns the minute 
 */
int rtc_get_min();

/**
 * @brief Gets the current second
 * @return returns the second 
 */
int rtc_get_sec();

/**
 * @brief Reads data from the RTC_DATA_REG
 * @param data - Data to read from the register
 * @param address - Address to write data to
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_read(uint8_t * data,uint8_t address);

/**
 * @brief Write a value to a register
 * @param address Address of register to change
 * @param data Data to write
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_write(uint8_t data,uint8_t address);

/**
 * @brief Enable alarm interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_enable_alarm();

/**
 * @brief Disable alarm interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_disable_alarm();

/**
 * @brief Set alarm second value
 * @param second - Value to write to the register
 * @return Return 0 upon success and non-zero otherwise
 */
int set_alarm_second(uint8_t second);

/**
 * @brief Set alarm minute value
 * @param minute - Value to write to the register
 * @return Return 0 upon success and non-zero otherwise
 */
int set_alarm_minute(uint8_t minute);

/**
 * @brief Set alarm hour value
 * @param hour - Value to write to the register
 * @return Return 0 upon success and non-zero otherwise
 */
int set_alarm_hour(uint8_t hour);

/**
 * @brief Set an alarm for the second "seconds", every minute
 * @param second - second the alarm rings
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_set_alarm_in_seconds(int seconds);

/**
 * @brief Enable update interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_update_enable();

/**
 * @brief Disable update interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_update_disable();


/**
 * @brief Enable periodic interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_enable_periodic();

/**
 * @brief Disable alarm interrupts
 * @return Return 0 upon success and non-zero otherwise
 */
int rtc_disable_periodic();

/**
 * @brief Converts BCD to decimal
 * @param bcd Byte to convert from BCD to decimal
 * @return bcd conversion to decimal
 */
int convert_to_decimal (uint32_t bcd);

/**@}*/

#endif
