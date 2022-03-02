#include "Sprite.h"

Sprite * create_sprite (xpm_image_t  sprite_img, uint16_t x, uint16_t y, uint16_t speed){
 
  Sprite * sprite = malloc(sizeof (Sprite));

  sprite->x = x;
  sprite->y = y;
  sprite->speed = speed;
  sprite->img = sprite_img;
  
  return sprite;
}
