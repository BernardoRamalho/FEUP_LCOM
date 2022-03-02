#include <stdlib.h>
#include "i8042.h"
#include "i8254.h"
#include "Hero.h"
#include "Zombie.h"
#include "Menu.h"
#include "mouse.h"
#include "Images/new_background.xpm"
#include "Images/chest.xpm"

#define MENU_STATE 0
#define HIGHSCORE_STATE 1
#define START_LEVEL_STATE 2
#define LEVEL_STATE 3
#define LEADERBOARD_STATE 4
#define END_STATE 5
#define WAITING_FOR_PLAYER 6
#define WIN 7

#define CHEST_X 549
#define CHEST_Y 399

 #ifndef _TERMINIX_H_
 #define _TERMINIX_H_

/** @defgroup Terminix Terminix
 * @{
 * Responsible creating everythin related to the game
 * Decides what and how many charachters there are
 * Holds the gamse_states enum, thet determines the state of the game
 * There is only one Terminx in each game
 */


enum game_states {menu,hardcore_state, start_level, level, learderboard, end, waiting_for_player, win};

typedef struct{
  int x; /*!< Specifies the current position in the x axis */
  int y; /*!< Specifies the current position in the y axis */
  xpm_image_t chest_img; /*!< Information about the chest image */
} Chest;

typedef struct {

  Mouse * mouse; /*!< Pointer to the mouse object */
  Chest * chest; /*!< Pointer to the chest object */
  Hero * heroes[2]; /*!< Array containing the information of the heroes */
  Zombie * zombies[8]; /*!< Array containing the information of the zombies */

  Menu * menu; /*!< Pointer to the menu object */
  
  bool multiplayer; /*!< Tells if the game is being played in multiplayer or not */

  int nr_zombies; /*!< Specifies the number of zombies that exist in the beggining of the level */
  int nr_dead_zombies; /*!< Specifies the number of deade zombies */
  int nr_dead_players; /*!< Specifies the number of deade players*/

  int level; /*!< Specifies the current level */
  int max_level; /*!< Specifies the max level */

  int nr_player; /*!< Specifies player number */

  enum game_states state; /*!< Specifies the state of the game */

  xpm_image_t  background_xpm; /*!< Information about the background image */

  vbe_mode_info_t mode_info; /*!< Information about the current vbe mode */

} Terminix;

/**
 * @brief Create a terminix object
 * 
 * @return Terminix* - pointer to the terminix just created
 */
Terminix * create_terminix();

/**
 * @brief Destroys the terminix
 * 
 * @param terminix - terminix to be destroyed
 */
void destroy_terminix(Terminix ** terminix);

/**
 * @brief Resets the current terminix
 * 
 */
void reset_terminix();

/**
 * @brief Get the current terminix object
 * 
 * @return Terminix* - pointer to the current terminix
 */
Terminix * get_current_terminix();

/**
 * @brief Create a zombies in a number related to the level
 * 
 */
void create_zombies();

/**
 * @brief Create a second hero
 * 
 * @return Hero* - pointer to the second hero created
 */
Hero * create_second_hero();

/**
 * @brief Create a chest object
 * 
 * @return Chest* - pointer to the chest just created
 */
Chest * create_chest();

/**
 * @brief Destroys a chest object
 * 
 * @param chest - chest to be destroyed
 */
void destroy_chest(Chest ** chest);

/**
 * @brief Draws the chest image into the screen
 * 
 */
void draw_chest();

/**
 * @brief Clean the chest image in the screen
 * 
 */
void clean_chest();

/**@}*/

#endif

