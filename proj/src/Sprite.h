/**
 * @file Sprite.h
 * @author Bernardo Ramalho & Tiago Alves
 * @brief Contains everything related to a sprite
 * @version 0.1
 * @date 2020-01-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once
#include "drawing.h"

 #ifndef _SPRITE_H_
 #define _SPRITE_H_

 /** @defgroup Sprite Sprite
 * @{
 * Responsible for all the operations regarding Sprites
 */

/**
 * @brief Struct containing all the information related to a sprite
 * 
 */
typedef struct
{
  uint16_t x;      /*!< Specifies the current x position of the sprite */
  uint16_t y;      /*!< Specifies the current y position of the sprite */
  uint16_t speed;  /*!< Specifies the speed of the sprite */
  xpm_image_t img; /*!< Information related to the image of the sprite */

} Sprite;
/**
 * @brief Create a sprite object
 * 
 * @param sprite_img - image of the sprite
 * @param x - starting x position of the sprite
 * @param y - starting y position of the sprite
 * @param speed - speed of the sprite
 * @return Sprite* - pointer to the sprite just created
 */
Sprite *create_sprite(xpm_image_t sprite_img, uint16_t x, uint16_t y, uint16_t speed);

/**@}*/

#endif
