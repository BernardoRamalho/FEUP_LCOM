/**
 * @file i8042.h
 * @author Bernardo Ramalho & Tiago Alves
 * @brief Responsible for everything related to the keyboard
 * @version 0.1
 * @date 2020-01-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

 #ifndef _KEYBOARD_H_
 #define _KEYBOARD_H_

 /** @defgroup Keyboard Keyboard
 * @{
 * Responsible for all the operations related to the keyboard
 */

#define STAT_REG 0x64
#define KBC_COMMAND_REG 0x64
#define KBC_ARGUMENT_REG 0x60
#define WRITE_COMMAND_BYTE 0x60
#define READ_COMMAND_BYTE 0x20
#define OUT_BUF_REG 0x60
#define INPUT_BUF_REG 0x60
#define OBF_BIT BIT(0)
#define INT0 BIT(0)
#define PARITY_BIT BIT(7)
#define TIME_OUT_BIT BIT(6)
#define AUX_BIT BIT(5)
#define DELAY_US 15
#define INCOMPLETE_SCAN_CODE 0xE0
#define IBF_BIT BIT(1)
#define ESC_BREAK_CODE 0x81
#define W_MAKE_CODE 0x11
#define W_BREAK_CODE 0x91
#define A_MAKE_CODE 0x1E
#define A_BREAK_CODE 0x9E
#define D_MAKE_CODE 0x20
#define D_BREAK_CODE 0xA0
#define S_MAKE_CODE 0x1F
#define S_BREAK_CODE 0x9F
#define SPACE_MAKE_CODE 0x39
#define SPACE_BREAK_CODE 0xb9
//Mouse Related Macros
#define MOUSE_DIS_DATA_REPORTING 0xF5
#define MOUSE_EN_DATA_REPORTING 0xF4
#define MOUSE_READ_DATA 0xEB
#define MOUSE_SET_STREAM_MODE 0xEA
#define ACK 0xFA
#define WRITE_BYTE_TO_MOUSE 0xD4
#define MOUSE_SET_REMOTE_MODE 0xF0
#define MSB_X_DELTA_BIT BIT(4)
#define MSB_Y_DELTA_BIT BIT(5)
#define EXTEND_8_TO_16 0XFF00
/**
  * @brief Subscribe the interrupts of the keyboard
  * 
  * @param bit_no - bit number of the keyboard
  * @return int - 0 if success or 1 if error
  */
int keyboard_subscribe(uint8_t *bit_no);
/**
  * @brief Unsubscribe the interrupts of the keyboard 
  * 
  * @return int - 0 if success or 1 if error 
  */
int keyboard_unsubscribe();
/**
  * @brief Reenables the default keyboards interrupts
  * 
  * @return int - 0 if success or 1 if error 
  */
int reenable_keyboard_interrupt();
/**
  * @brief Flushs the KBC OBF
  * 
  * @return int - 0 if success or 1 if error 
  */
int flush_OBF();
/**
  * @brief Checks whether we have a full scan code or not
  * 
  * @return int - 0 if yes or 1 if no 
  */
int got_full_scan_code();
/**
  * @brief Function responsible for analysing and acting on the information receive from the keyboard
  * 
  * @return int - 0 if success or 1 if error 
  */
int keyboard_handler();

/**@}*/

#endif
