#include "Menu.h"
#include "Images/main_menu.xpm"
#include "Images/game_over.xpm"
#include "Images/youLost.xpm"
#include "Images/youWin.xpm"
#include "Images/waitingScreen.xpm"
#include "Images/highscores1.xpm"
#include "Images/menu_back.xpm"
#include "Images/score.xpm"
#include "Images/garf.xpm"


#include <stdlib.h>

Menu * create_menu() {

  Menu *menu = malloc(sizeof(Menu));

  xpm_image_t menu_img,gameOverImg, youWinImg, waitingScreenImg,highscoresImg,menu_back_img,score_img,easter_egg;

  convert_xpm_img(main_menu_xpm, &menu_img);
  convert_xpm_img(youLost_xpm, &gameOverImg);
  convert_xpm_img(youWin_xpm, &youWinImg);
  convert_xpm_img(waitingScreen_xpm, &waitingScreenImg);
  convert_xpm_img(highscores1_xpm, &highscoresImg);
  convert_xpm_img(menu_background_xpm, &menu_back_img);
  convert_xpm_img(score_xpm, &score_img);
  convert_xpm_img(garf_xpm, &easter_egg);

  menu->menu_img = menu_img;
  menu->gameOver_img = gameOverImg;
  menu->youWin_img = youWinImg;
  menu->waitingScreen_img = waitingScreenImg;
  menu->back_img = menu_back_img;
  menu->score_img = score_img;
  menu->easterEgg_img = easter_egg;
  menu->easterEgg = false;
  menu->highscore = create_sprite(highscoresImg,300, 300,0);

  return menu;
}

void destroy_menu(Menu ** menu) {

  if(*menu == NULL){
    return;
  }
  free(*menu);
  *menu = NULL;
  
}

void clean_menu(Menu * menu){
  clean_xpm(menu->menu_img,0, 0);
}

void clean_gameOver(Menu * menu){
  clean_xpm(menu->gameOver_img,0, 0);
}

void clean_youWin(Menu * menu){
  clean_xpm(menu->youWin_img,0, 0);
}

void clean_waitingScreenImg(Menu * menu){
  clean_xpm(menu->waitingScreen_img,0, 0);
}


