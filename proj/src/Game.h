/**
 * @file Game.h
 * @author Bernardo Ramalho & Tiago Alves
 * @brief Containg everything that regards the logic of the game and the way it runs
 * @version 0.1
 * @date 2020-01-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "Terminix.h"

 #ifndef _GAME_H_
 #define _GAME_H_

 /** @defgroup Game Game
 * @{
 * Responsible for the logic of the game
 * Where operations regarding all the periphericals are made
 */

/**
 * @brief Subscribes to all the periphericals used
 * 
 * @param timer_irq - irq of the timer
 * @param mouse_irq - irq of the mouse
 * @param keyboard_irq - irq of the keyboard
 * @param rtc_irq - irq of the rtc
 * @param uart_irq - irq of the uart
 * @return int - 0 if success or 1 if fail
 */
int subscribe_periphericals(int *timer_irq, int *mouse_irq, int *keyboard_irq,int *rtc_irq, int *uart_irq);

/**
 * @brief Unsubscribes to all the periphericals used
 * 
 * @return int - 0 if success or 1 if fail
 */
int unsubscribe_periphericals();

/**
 * @brief Get the current terminix object
 * 
 * @return Terminix* - pointer to the current terminix
 */
Terminix *get_current_terminix();

/**
 * @brief Updates the position of all the game objects
 * 
 */
void update_positions();

/**
 * @brief Updates the hero position while checking for collisions
 * 
 * @param terminix - current terminix
 * @param hero_index - index of the hero to udpate the position
 */
void update_hero_position(Terminix * terminix, int hero_index);

/**
 * @brief Updates the position of a zombie while checking for collisions
 * 
 * @param zombie - Zombie to be updated
 * @param zombie_index - index of the zombie to be updated
 */
void update_zombie_position(Zombie * zombie, int zombie_index);

/**
 * @brief Updates the position of the mouse
 * 
 * @param mouse - mouse to be updated
 */
void update_mouse_img(Mouse * mouse);

/**
 * @brief Updates the bullet position while checking for collisions
 * 
 * @param bullet - bullet to be updated
 * @param hero_index - index of the hero that the bullet belongs to
 */
void update_bullet_position(Bullet * bullet, int hero_index);

/**
 * @brief Calls the update_bullet_position function to all the bullets
 * 
 * @param terminix - current terminix
 */
void update_bullets_position(Terminix * terminix);

/**
 * @brief Call the update_zombie_position function to all the zombies
 * 
 * @param zombies - array containing all the zombies
 * @param n - number of total zombies
 */
void update_zombies_position(Zombie * zombies[6], int n);

/**
 * @brief Call the update_hero_position function to all the heroes
 * 
 * @param terminix - current terminix
 */
void update_heroes_position(Terminix * terminix);

/**
 * @brief Starts the level. Initializes the zombies and puts everything in its original position.
 * 
 */
void update_level();

/**
 * @brief Resets the position of all the heroes
 * 
 * @param terminix - current terminix
 */
void reset_heroes_position(Terminix * terminix);

/**
 * @brief Renders the game into the screen depending on the state
 * 
 */
void render();

/**
 * @brief Checks if the bullet as collided with anything
 * 
 * @param bullet - bullet to check the collision
 * @param terminix - current terminix
 * @return true - if the bullet has collided with anything
 * @return false - if the bullet hasn't collided
 */
bool check_bullet_colisions(Bullet * bullet, Terminix * terminix);

/**
 * @brief Checks if the hero as collided with an enemy
 * 
 * @param hero - hero to check the collision
 * @param terminix - current terminix
 * @return true - if the hero has collided with an enemy
 * @return false - if the hero hasn't collided
 */
bool check_heroe_enemy_collisions(Hero * hero, Terminix * terminix);

/**
 * @brief Checks if the hero as collided with a friendly object
 * 
 * @param hero - hero to check the collision
 * @param terminix - current terminix
 * @return true - if the hero has collided with a friendly object
 * @return false - if the hero hasn't collided
 */
bool check_heroe_friendly_collisions(Hero * hero, Terminix * terminix);

/**
 * @brief Checks if the zombie as collided with anything
 * 
 * @param zombie - zombie to check the collision
 * @param terminix - current terminix
 * @return true - if the zombie has collided with anything
 * @return false - if the zombie hasn't collided
 */
bool check_zombie_collisions(Zombie * zombie, Terminix * terminix, int zombie_index);

/**
 * @brief Changes the hero direction based on the mouse position
 * 
 * @param hero - pointer to the hero to change the direction
 * @param mouse - pointer to the mouse object
 */
void check_hero_direction(Hero * hero, Mouse * mouse);

/**
 * @brief Analyses and acts on the message received by the uart
 * 
 * @param message - message received by the uart
 */
void uart_message_analyse(char * message);

/**
 * @brief Changes the other player's hero direction based on the uart message
 * 
 * @param message - message received by the uart
 * @param hero - pointer to the other player's hero
 */
void change_other_hero_direction(char message, Hero * hero);

/**
 * @brief Changes the other player's hero position based on the uart message
 * 
 * @param message - message received by the uart
 * @param hero - pointer to the other player's hero
 */
void change_other_hero_position(char message, Hero * hero);

/**
 * @brief Send the information about the movement of the hero
 * 
 * @param terminix - current terminix
 * @param movement - char representing the movement made by the hero
 */
void send_movement_info(Terminix * terminix, char movement);

/**
 * @brief Checks the smallest distance between the Zombie and all the other game objects (except other zombies)
 * 
 * @param terminix - current terminix 
 * @param zombie - pointer to the zombie to calculate the smallest distance
 * @return char - representes the object thats the most near 
 */
char check_smallest_distance(Terminix * terminix, Zombie * zombie);

/**
 * @brief Changes the zombies coordinatess based on the output of the check_smallest_distance
 * 
 * @param terminix -current terminix
 * @param zombie - pointer to the zombi to change the coordinatess
 * @param object - represents the object thats the most near to the zombie
 */
void change_zombie_coords(Terminix * terminix, Zombie * zombie, char object);

/**
 * @brief Saves the score of the hero
 * 
 */
void save_score();

/**
 * @brief Draws the menu image
 * 
 */
void drawMenu();

/**
 * @brief Draws the game over image
 * 
 */
void drawGameOver();

/**
 * @brief Cleans the game over image
 * 
 */
void cleanGameOver();


/**@}*/

#endif
