
/**
 * @file mouse.h
 * @author Bernardo Ramalho & Tiago Alves
 * @brief responsible for everything related to the mouse
 * @version 0.1
 * @date 2020-01-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once
#include "Sprite.h"
#include <stdbool.h>
#include <stdint.h>


 #ifndef _MOUSE_H_
 #define _MOUSE_H_

 /** @defgroup Mouse Mouse
 * @{
 * Responsible for all the operations regarding the Mouse
 */

#define DISABLE_MOUSE_BYTE 0xA7
#define MOUSE_START_POSITION 300
#define ENABLE_MOUSE_BYTE 0xA8
#define CHECK_MOUSE_INTERFACE_BYTE 0xA8
#define WRITE_BYTE_TO_MOUSE 0xD4
#define DISABLE_MOUSE_BYTE 0xA7
#define MOUSE_IRQ 12
#define MOUSE_EVENT BIT(0)
/**
 * @brief Struct containg all the information related to the mouse
 * 
 */
typedef struct
{
  __int16_t x;           /*!< Specifies the current x position of the mouse */
  __int16_t y;           /*!< Specifies the current y position of the mouse */
  __int16_t new_x;       /*!< Specifies the new x position of the mouse */
  __int16_t new_y;       /*!< Specifies the new y position of the mouse */
  xpm_image_t mouse_img; /*!< Information about the mouse image */
} Mouse;

/**
 * @brief Create a mouse object
 * 
 * @return Mouse* - pointer to the mouse just created
 */
Mouse *create_mouse();
/**
  * @brief Subscribe the interrupts of the mouse
  * 
  * @param bit_no - bit number of the mouse
  * @return int - 0 if success or 1 if error
  */
int mouse_subscribe(uint8_t *bit_no);
/**
  * @brief Unsubscribe the interrupts of the mouse
  * 
  * @param bit_no - bit number of the mouse
  * @return int - 0 if success or 1 if error
  */
int mouse_unsubscribe();
/**
 * @brief Saves the byte received from the mouse into an array of bytes
 * 
 * @param position - index of the array to save the byte
 * @param bytes - array to save the byte in
 */
void packet_byte_handler(int *position, uint8_t bytes[3]);
/**
 * @brief Saves the informatin of a packet sent by the mouse into a struct 
 * 
 * @param pp - struct to save the information of the packet
 * @param packet - array containg the 3 bytes of the packet
 */
void packet_handler(struct packet *pp, uint8_t packet[3]);
/**
 * @brief Sends a command to the mouse controller
 * 
 * @param command - command to be sent
 * @return int - 0 if sucess or 1 if error
 */
int mouse_send_command(uint8_t command);
/**
 * @brief Enables the mouse interrupts
 * 
 */
void mouse_enable_interrupts();
/**
 * @brief Disables the mouse interrupts
 * 
 */
void mouse_disable_interrupts();
/**
 * @brief Flushes the mouse OBF
 * 
 * @return int - 0 if sucess or 1 if error
 */
int mouse_flush_OBF();
/**
 * @brief Resets the controller command byte
 * 
 * @param byte - byte to send to the controller
 * @return int - 0 if sucess of 1 if error
 */
int reset_KBC_command_byte(uint8_t byte);
/**
 * @brief Checks if the IBF is full
 * 
 * @return int - 0 if no of 1 if yes
 */
int check_if_IBF_full();
/**
 * @brief Analyses the mouse events and acts on them
 * 
 * @param mouse_event - struct containing the information of the mouse event
 */
void mouse_actions_analyser(struct mouse_ev *mouse_event);
/**
 * @brief Handles the mouse interrupts and acts on them
 * 
 * @param position - index of the packet byte in the array
 * @param packet - array containing the packets bytes
 */
void mouse_handler(int *position, uint8_t packet[3]);
/**
 * @brief updates the mouse position based on the information received
 * 
 * @param pp - struct containing the information sent be the mouse
 */
void update_mouse_position(struct packet pp);
/**
 * @brief Detects wheter an event as occured or not, based on the information sent bye th mouse
 * 
 * @param pp - struct containing the information sent be the mouse
 * @param event - represents place to save the mouse event that occured
 * @return struct mouse_ev - struct containg all the information of the mouse event
 */
struct mouse_ev mouse_event_detector(struct packet *pp, uint8_t *event);
/**
 * @brief Checks if the mouse is over a given image
 * 
 * @param sprite - pointer to the sprite containg the information of the image
 * @return true - if the mouse is over the given image
 * @return false - if the mouse is not over the given image
 */
bool mouse_over_img(Sprite *sprite);
/**
 * @brief Checks if the mouse is over the Co-Op option in the menu
 * 
 * @return true - if the mouse is over the co-op option
 * @return false - if the mouse is not over the co-op option
 */
bool mouse_over_coop_option();
/**
 * @brief Checks if the mouse is over the 1 Player option in the menu
 * 
 * @return true - if the mouse is over the 1 Player option
 * @return false - if the mouse is not over the 1 Player option
 */
bool mouse_over_1player_option();
/**
 * @brief Destroy mouse object
 * 
 * @param mouse - object to be destroyed
 */
void destroy_mouse(Mouse **mouse);



/**@}*/

#endif
