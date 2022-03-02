/**
 * @file Zombie.h
 * @author Bernardo Ramalho & Tiago Alves
 * @brief Classe que cria e destroi os Zombies, principais enimigos.
 * @date 2020-01-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "drawing.h"
#include "Sprite.h"

 #ifndef _ZOMBIE_H_
 #define _ZOMBIE_H_

 /** @defgroup Zombie Zombie
 * @{
 * Responsible for all the operations regarding  Zombies
 */

/**
 * @brief 
 * 
 */
typedef struct {

  Sprite * sprite;
  xpm_image_t zombie_img;
  xpm_image_t  sprites[4];
  
} Zombie;

/**
 * @brief Create a zombie object
 * 
 * @param x - position in the x axis where the zombies starts
 * @param y - position in the y axis where the zombies starts
 * @return Zombie* - pointer to the zombie created
 */
Zombie * create_zombie(uint16_t x, uint16_t y);

/**
 * @brief Destroyes a zombie object
 * 
 * @param zombie - zombie to be destroyed
 */
void destroy_zombie(Zombie ** zombie);

/**@}*/

#endif
