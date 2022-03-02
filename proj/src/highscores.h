
#include <stdbool.h>
#include <stdint.h>
#include <minix/driver.h>
#include <sys/mman.h>
#include <lcom/lcf.h>


 #ifndef _HIGHSCORE_H_
 #define _HIGHSCORE_H_

 /** @defgroup Highscore Highscore
 * @{
 * All the operations regarding the Highscores
 * Menus and printing text to the creen
 */

/**
 * @brief Struct that saves the information of a Highscore
 * 
 */
typedef struct
{
    int score;
    int day;
    int month;
    int year;
    int hour;
    int minute;
    int sec;

} Highscore;

/**
 * @brief Loads all the xpms, corresponding to numbers and 
 * @brief insters it on an array, in which each position holds the 
 * @brief XPM corresponding to the position number
 */
void load_numbers_xpms();

/**
 * @brief Saves the score to the scores.txt file
 * @param hero_index - Hero index we want to save the score
 */
void save_score(int hero_index);

/**
 * @brief Given a number, prints to the screen the corresponding number
 * @param num - Number to print
 * @param x - position in the x axis to print the image
 * @param y - position in the y axis to print the image
 */
void print_digit(unsigned num, uint16_t x, uint16_t y);

/**
 * @brief Reads the Highscores from the scores.txt file
 */
void read_highscores();

/**
 * @brief Prints all the highscores in a formatted way
 */
void print_highscores();

/**
 * @brief Draws the Highscores Menu
 */
void drawHighscoresMenu();

/**
 * @brief Creates the struct Highscore according to the paramters
 * @param score - Score of the user in the game
 * @param day - day the user made the score
 * @param month - month the user made the score
 * @param year - year the user made the score
 * @param hour - hour the user made the score
 * @param minute - minute the user made the score
 * @param second - second the user made the score
 */
Highscore *create_highscore(int score, int day, int month, int year, int hour, int minute, int second);

/**
 * @brief Destroys the Highscores Struct
 */
void destroy_highscore(Highscore **highscore);

/**@}*/

#endif 
