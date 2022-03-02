/**
 * @file Menu.h
 * @author Bernardo Ramalho & Tiago Alves
 * @brief responsible for drawing and cleaning the menus
 * @version 0.1
 * @date 2020-01-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "Sprite.h"



 #ifndef _MENU_H_
 #define _MENU_H_

 /** @defgroup menu menu
 * @{
 * Responsible for the logic of the menu
 * Where operations regarding all the periphericals are made
 */


/**
 * @brief Struct that saves all the information related to the Menu
 * 
 */
typedef struct
{

  xpm_image_t menu_img;
  xpm_image_t gameOver_img;
  xpm_image_t youWin_img;
  xpm_image_t waitingScreen_img;
  xpm_image_t back_img;
  xpm_image_t score_img;
  xpm_image_t easterEgg_img;
  Sprite *highscore;

  bool easterEgg;

} Menu;

/**
 * @brief Create a menu object
 * 
 * @return Menu* - pointer to the Menu just created
 */
Menu *create_menu();
/**
 * @brief destroyes the menu
 * 
 * @param menu - menu to be destroyed
 */
void destroy_menu(Menu **menu);
/**
 * @brief Clean the main menu image
 * 
 * @param menu - pointer to the menu containing the image info
 */
void clean_menu(Menu *menu);
/**
 * @brief Clean the game over image
 * 
 * @param menu - pointer to the menu containing the image info
 */
void clean_gameOver(Menu *menu);
/**
 * @brief Clean the win image
 * 
 * @param menu - pointer to the menu containing the image info
 */
void clean_youWin(Menu *menu);
/**
 * @brief Clean the waiting screen image
 * 
 * @param menu - pointer to the menu containing the image info
 */
void clean_waitingScreenImg(Menu *menu);

/**@}*/

#endif
