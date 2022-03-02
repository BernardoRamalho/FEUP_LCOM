/**
 * @file Hero.h
 * @author Bernardo Ramalho & Tiago Alves
 * @brief Responsible for creating, destroying everything related to the Hero.
 * It also is responsible to changing its position.
 * @version 0.1
 * @date 2020-01-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */
 #include "Sprite.h"


 #ifndef _HERO_H_
 #define _HERO_H_

 /** @defgroup HERO HERO
 * @{
 * Responsible for all te operations related to the HERO
 */
 #define HERO_START_POSITION_X 545
 #define HERO_START_POSITION_Y 240
 #define HERO_1_START_POISITION_X 380
 #define HERO_1_START_POISITION_y 400
 #define HERO_2_START_POISITION_X 745
 #define HERO_2_START_POISITION_y 400
/**
 * @brief Struct that saves the information of a bullet
 * 
 */
typedef struct {
  Sprite * sprite; /*!< Information about the bullets sprite */
  uint16_t x_direction; /*!< Specifies the direction in the x axis */
  uint16_t y_direction; /*!< Specifies the direction in the y axis */
  xpm_image_t bullet_img; /*!< Specifies the bullet image to be drawn */
} Bullet;
/**
 * @brief tells where the hero is poiting to
 */
enum direction {up, left, right, down, ne, nw, sw, se};
/**
 * @brief Struct that saves the information of a hero
 * 
 */
typedef struct {
  uint16_t x; /*!< Specifies the current position in the x axis */
  uint16_t y; /*!< Specifies the current position in the y axis */
  uint16_t new_x; /*!< Specifies the next position in the x axis */
  uint16_t new_y; /*!< Specifies the next position in the y axis */
  uint16_t init_x; /*!< Specifies the initial position in the x axis */
  uint16_t init_y; /*!< Specifies the initial position in the y axis */
  uint16_t x_direction; /*!< Specifies the direction in the x axis */
  uint16_t y_direction; /*!< Specifies the direction in the y axis */
  uint16_t score; /*!< Specifies the score of the hero */ 
  xpm_image_t hero_img; /*!< Specifies the image to be drawn */ 
  xpm_image_t  sprites[8]; /*!< Saves all the possible sprites of the hero */
  enum direction direction; /*!< Specifies the direction the hero is facing towards */
  Bullet * bullet; /*!< Specifies the bullet object of the heror */
} Hero;
/****************** HERO RELATED FUNCTION ******************/
/**
 * @brief Create a hero object
 * 
 * @return Hero* - pointer to the hero just created
 */
Hero * create_hero();
/**
 * @brief Destroys the hero
 * 
 * @param hero - hero to be destroyd
 */
void destroy_hero(Hero ** hero);
/**
 * @brief Changes the position values so the Hero moves up
 * 
 * @param hero_ptr - pointer to the hero that moved
 */
void hero_move_up(Hero * hero_ptr);
/**
 * @brief Changes the position values so the Hero moves down
 * 
 * @param hero_ptr - pointer to the hero that moved
 */
void hero_move_down(Hero * hero_ptr);
/**
 * @brief Changes the position values so the Hero moves left
 * 
 * @param hero_ptr - pointer to the hero that moved
 */
void hero_move_left(Hero * hero_ptr);
/**
 * @brief Changes the position values so the Hero moves right
 * 
 * @param hero_ptr - pointer to the hero that moved
 */
void hero_move_right(Hero * hero_ptr);
/**
 * @brief Changes the position values so the Hero goes back to the initial position
 * 
 * @param hero_ptr - pointer to the hero that will get its position reseted
 */
void reset_hero_position(Hero * hero_ptr);
/**
 * @brief Changes the position values so the Hero of the player one starts in the right position
 *
 * @param hero_ptr - pointer to the hero of player 1
 */
void change_hero1_multiplayer_positon(Hero * hero_ptr);
/****************** BULLET RELATED FUNCTION ******************/
/**
 * @brief Creates a bullet object and draws it
 * 
 * @param hero_ptr - pointer to the hero who shot the bullet
 * @return Bullet* - pointer to the bullet just created
 */
Bullet * shoot(Hero * hero_ptr);
/**
 * @brief Destroys the bullet
 * 
 * @param bullet - bullet to be destroyed
 */
void destroy_bullet(Bullet ** bullet);
/**
 * @brief Get the bullet coordinatess based on where the hero is and which direction he is facing
 * 
 * @param hero - hero to whom the bullet belongs
 * @param x - x coordinate of the bullet
 * @param y - y coordinate of the bullet
 */
void get_bullet_coords(Hero* hero, uint16_t * x, uint16_t * y);
/**
 * @brief Get the bullet direction based on which direction the hero is facing
 * 
 * @param hero 
 * @param x 
 * @param y 
 */
void get_bullet_direction(Hero * hero, uint16_t * x, uint16_t * y);
/**
 * @brief Updates the xpm of the hero, mainly used when he changes direction
 * 
 * @param hero - pointer to the hero
 */
void update_xpm(Hero * hero);

/**@}*/

#endif 
