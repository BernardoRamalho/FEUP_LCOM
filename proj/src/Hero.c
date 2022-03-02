#include <stdlib.h>
#include "Hero.h"
#include "Images/hero1_up.xpm"
#include "Images/hero1_down.xpm"
#include "Images/hero1_left.xpm"
#include "Images/hero1_SE.xpm"
#include "Images/hero1_NE.xpm"
#include "Images/hero1_NW.xpm"
#include "Images/hero1_SW.xpm"
#include "Images/hero1_right.xpm"
#include "Images/bullet.xpm"


Hero * create_hero(){

  Hero * hero_ptr = malloc(sizeof (Hero));
  xpm_image_t  hero_img;

  convert_xpm_img(hero1_xpm,&hero_img);
  hero_ptr->sprites[0] = hero_img;
  
  convert_xpm_img(hero1_left_xpm,&hero_img);
  hero_ptr->sprites[1] = hero_img;
  
  convert_xpm_img(hero1_right_xpm,&hero_img);
  hero_ptr->sprites[2] = hero_img;
  
  convert_xpm_img(hero1_down_xpm,&hero_img);
  hero_ptr->sprites[3] = hero_img;
  

  convert_xpm_img(hero1_NE_xpm,&hero_img);
  hero_ptr->sprites[4] = hero_img;

  convert_xpm_img(hero1_NW_xpm,&hero_img);
  hero_ptr->sprites[5] = hero_img;

  convert_xpm_img(hero1_SW_xpm,&hero_img);
  hero_ptr->sprites[6] = hero_img;
  
  convert_xpm_img(hero1_SE_xpm,&hero_img);
  hero_ptr->sprites[7] = hero_img;


  hero_ptr->x = HERO_START_POSITION_X;
  hero_ptr->y = HERO_START_POSITION_Y;
  hero_ptr->new_x = HERO_START_POSITION_X;
  hero_ptr->new_y = HERO_START_POSITION_Y;
  hero_ptr->init_x = HERO_START_POSITION_X;
  hero_ptr->init_y = HERO_START_POSITION_Y;
  hero_ptr->x_direction = 0;
  hero_ptr->y_direction = 1;
  hero_ptr->hero_img = (hero_ptr->sprites[0]);
  hero_ptr->bullet = NULL;
  hero_ptr->direction = up;
  hero_ptr->score = 0;
    
  return hero_ptr;
}


Bullet * shoot(Hero * hero){

  Bullet * bullet_ptr = malloc(sizeof (Bullet));

  xpm_image_t bullet_img;
  convert_xpm_img(bullet_xpm, &bullet_img);

  uint16_t bullet_x, bullet_y;

  get_bullet_coords(hero, &bullet_x, &bullet_y);

  bullet_ptr->sprite = create_sprite(bullet_img,bullet_x, bullet_y,10);

  get_bullet_direction(hero, &(bullet_ptr->x_direction), &(bullet_ptr->y_direction));

  draw_xpm(bullet_img, bullet_ptr->sprite->x, bullet_ptr->sprite->y);

  return bullet_ptr;
}


void hero_move_up(Hero * hero) {
  hero->new_y = hero->y - 15;
  hero->y_direction = -1;
  hero->x_direction = 0;
}

void hero_move_down(Hero * hero){
    hero->new_y  = hero->y + 15;
    hero->y_direction = 1;
    hero->x_direction = 0;
}

void hero_move_left(Hero * hero){
    hero->new_x = hero->x - 15;
    hero->x_direction = -1;
    hero->y_direction = 0;
}

void hero_move_right(Hero * hero){
    hero->new_x  = hero->x + 15;
    hero->x_direction = 1;
    hero->y_direction = 0;
}

void reset_hero_position(Hero * hero_ptr){
  hero_ptr->x = hero_ptr->init_x;
  hero_ptr->y = hero_ptr->init_y;
  hero_ptr->new_x = hero_ptr->init_x;
  hero_ptr->new_y = hero_ptr->init_y;
  hero_ptr->x_direction = 0;
  hero_ptr->y_direction = 1;
  hero_ptr->hero_img = (hero_ptr->sprites[0]);
  hero_ptr->bullet = NULL;
  hero_ptr->direction = up;
}

void change_hero1_multiplayer_positon(Hero * hero_ptr){
  hero_ptr->x = HERO_1_START_POISITION_X;
  hero_ptr->y = HERO_1_START_POISITION_y;
  hero_ptr->new_x = HERO_1_START_POISITION_X;
  hero_ptr->new_y = HERO_1_START_POISITION_y;
  hero_ptr->init_x = HERO_1_START_POISITION_X;
  hero_ptr->init_y = HERO_1_START_POISITION_y;
}


void destroy_bullet(Bullet ** bullet) {

  if(*bullet == NULL){
    return;
  }
  free(*bullet);
  *bullet = NULL;
}

void destroy_hero(Hero ** hero) {

  if(*hero == NULL){
    return;
  }
  free(*hero);
  *hero = NULL;
}

void update_xpm(Hero * hero){

    switch(hero->direction){
    case up:
      clean_xpm(hero->hero_img, hero->x, hero->y);

      hero->hero_img = (hero->sprites[0]);

      draw_xpm(hero->hero_img, hero->x, hero->y);
      break;
    case left:
      clean_xpm(hero->hero_img, hero->x, hero->y);

      hero->hero_img = (hero->sprites[1]);

      draw_xpm(hero->hero_img, hero->x, hero->y);
      break;
    case right:
      clean_xpm(hero->hero_img, hero->x, hero->y);

      hero->hero_img = (hero->sprites[2]);

      draw_xpm(hero->hero_img, hero->x, hero->y);
      break;
    case down:
      clean_xpm(hero->hero_img, hero->x, hero->y);

      hero->hero_img = (hero->sprites[3]);

      draw_xpm(hero->hero_img, hero->x, hero->y);
      break;
    case ne:
      clean_xpm(hero->hero_img, hero->x, hero->y);

      hero->hero_img = (hero->sprites[4]);

      draw_xpm(hero->hero_img, hero->x, hero->y);
      break;
    case nw:
      clean_xpm(hero->hero_img, hero->x, hero->y);

      hero->hero_img = (hero->sprites[5]);

      draw_xpm(hero->hero_img, hero->x, hero->y);
      break;
    case sw:
      clean_xpm(hero->hero_img, hero->x, hero->y);

      hero->hero_img = (hero->sprites[6]);

      draw_xpm(hero->hero_img, hero->x, hero->y);
      break;
    case se:
      clean_xpm(hero->hero_img, hero->x, hero->y);

      hero->hero_img = (hero->sprites[7]);

      draw_xpm(hero->hero_img, hero->x, hero->y);
      break;
  }

}

void get_bullet_coords(Hero* hero, uint16_t * x, uint16_t * y){

  uint16_t hero_width = (hero->hero_img).width;
  uint16_t hero_height = (hero->hero_img).height;

  switch(hero->direction){
    case up:
      *x = hero->x + hero_width/2.3;
      *y = hero->y;
      break;
    case left:
      *x = hero->x;
      *y = hero->y + hero_height/3.25;
      break;
    case right:
      *x = hero->x + hero_width;
      *y = hero->y + hero_height/2.3;
      break;
    case down:
      *x = hero->x + hero_width/3.25;
      *y = hero->y + hero_height;
      break;
    case ne:
      *x = hero->x + hero_width;
      *y = hero->y;
      break;
    case nw:
      *x = hero->x;
      *y = hero->y;
      break;
    case sw:
      *x = hero->x;
      *y = hero->y + hero_height;
      break;
    case se:
      *x = hero->x + hero_width;
      *y = hero->y + hero_height;
      break;
    
  }

  return;
}

void get_bullet_direction(Hero * hero, uint16_t * x, uint16_t * y){

  switch(hero->direction){
    case up:
      *x = 0;
      *y = -1;
      break;
    case left:
      *x = -1;
      *y = 0;
      break;
    case right:
      *x = 1;
      *y = 0;
      break;
    case down:
      *x = 0;
      *y = 1;
      break;
    case ne:
      *x = 1;
      *y = -1;
      break;
    case nw:
      *x = -1;
      *y = -1;
      break;
    case sw:
      *x = -1;
      *y = 1;
      break;
    case se:
      *x = 1;
      *y = 1;
      break;
  }
}












