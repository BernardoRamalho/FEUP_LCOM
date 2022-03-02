#include "Terminix.h"
#include "time.h"
#include "RTC.h"
#include "highscores.h"


static Terminix * terminix = NULL;


Terminix * get_current_terminix() {
  return terminix;
}

Terminix * create_terminix(){

  if(terminix != NULL){
    printf("Estou null\n");
    return terminix;

  }


  terminix = malloc(sizeof (Terminix));
  
  xpm_image_t background_img;
  convert_xpm_img(background_xpm,&background_img);
  draw_xpm(background_img,0,0);
  terminix->background_xpm = background_img;

  load_numbers_xpms();

  read_highscores();


  Hero * hero = create_hero();

  terminix->nr_zombies = 0;
  terminix->nr_dead_zombies = 0;

  terminix->level = 1;
  terminix->max_level = 2;
  terminix->nr_player = 1;
  terminix->nr_dead_players = 0;
  
  vbe_mode_info_t current_mode_info;

  get_vbe_mode_info(RGB_VIDEO_MODE, &current_mode_info);

  terminix->mode_info = current_mode_info;

  Mouse * mouse = create_mouse();

  Menu * menu = create_menu();

  terminix->heroes[0] = hero;
  terminix->heroes[1] = NULL;

  terminix->mouse = mouse;

  terminix->menu = menu;

  terminix->state = MENU_STATE;

  terminix->multiplayer = false;

  terminix->chest = create_chest();

  rtc_set_alarm_in_seconds(rtc_get_sec());


  return terminix;
}


void create_zombies(){
  srand(time(0));
  
  int x, y, init_x = 140, init_y = 40;

  for (int i = 1; i < terminix->nr_zombies + 1; i++){
   
    x = init_x;
    y = init_y;

    terminix->zombies[i - 1] = create_zombie(x,y);

    if(terminix->nr_zombies == 4){
      
      if(i % 2 == 0){
        init_x = 140;
        init_y += 700;
      }
      else{
        init_x += 800;
      }

    }
    else if(terminix->nr_zombies == 8){

      if(i == 3){
        init_x = 140;
        init_y += 350;
      }
      else if(i == 4){
        init_x += 700;
      }
      else if(i == 5){
        init_x = 140;
        init_y += 350;
      }
      else{
        init_x += 400;
      }
    }
  else{
    init_x += 800;
    init_y += 700;
  }

  }

  terminix->nr_dead_zombies = 0;
  
}

Hero * create_second_hero(){
  Hero * second_hero = create_hero();

  second_hero->x = HERO_2_START_POISITION_X;
  second_hero->y = HERO_2_START_POISITION_y;
  second_hero->new_y = HERO_2_START_POISITION_y;
  second_hero->new_x = HERO_2_START_POISITION_X;
  second_hero->init_y = HERO_2_START_POISITION_y;
  second_hero->init_x = HERO_2_START_POISITION_X;

  return second_hero;
}

void destroy_terminix(Terminix ** terminix) {

  if(*terminix == NULL){
    return;
  }
  Menu * menu = (*terminix)->menu;
  
  destroy_menu(&menu);

  for (int i = 0; i < (*terminix)->nr_zombies; i++){
    Zombie * zombie = (*terminix)->zombies[i];
    destroy_zombie(&zombie);
  }

  free(*terminix);

  *terminix = NULL;

}

void reset_terminix(){

  terminix->level = 1;

  terminix->max_level = 2;

  terminix->nr_zombies = 0;
  
  terminix->nr_dead_zombies = 0;

  terminix->nr_dead_players = 0;

  if(terminix->multiplayer){
    if(terminix->heroes[1] != NULL){
      destroy_hero(&(terminix->heroes[1]));
      terminix->heroes[1] = NULL;
    }
    terminix->multiplayer = false;
  }

  if(terminix->heroes[0] == NULL){
    terminix->heroes[0] = create_hero();
  }

}

Chest * create_chest(){
  Chest * chest = malloc(sizeof (Chest));
  xpm_image_t chest_img;

  convert_xpm_img(chest_xpm, &chest_img);

  chest->x = CHEST_X;
  chest->y = CHEST_Y;
  chest->chest_img = chest_img;

  return chest;
}

void draw_chest(){
  draw_xpm(terminix->chest->chest_img, terminix->chest->x, terminix->chest->y);
}
void clean_chest(){
  clean_xpm(terminix->chest->chest_img, terminix->chest->x, terminix->chest->y);
}






