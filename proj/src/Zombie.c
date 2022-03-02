
#include "Zombie.h"
#include "Images/black_zombie_down.xpm"
#include "Images/black_zombie_up.xpm"
#include "Images/black_zombie_left.xpm"
#include "Images/black_zombie_right.xpm"


Zombie * create_zombie(uint16_t x, uint16_t y){

  Zombie * zombie_ptr = malloc(sizeof (Zombie));

  xpm_image_t  zombie_img;

  convert_xpm_img(black_zombie_up_xpm,&zombie_img);
  zombie_ptr->sprites[0] = zombie_img;
  
  convert_xpm_img(black_zombie_left_xpm,&zombie_img);
  zombie_ptr->sprites[1] = zombie_img;
  
  convert_xpm_img(black_zombie_right_xpm,&zombie_img);
  zombie_ptr->sprites[2] = zombie_img;
  
  convert_xpm_img(black_zombie_down_xpm,&zombie_img);
  zombie_ptr->sprites[3] = zombie_img;

  zombie_ptr->sprite = create_sprite(zombie_img,x, y, 1);
  
  zombie_ptr->zombie_img = (zombie_ptr->sprites[0]);
  
  return zombie_ptr;
}


void destroy_zombie(Zombie ** zombie) {

  if(*zombie == NULL){
    return;
  }
  free(*zombie);
  *zombie = NULL;
}
