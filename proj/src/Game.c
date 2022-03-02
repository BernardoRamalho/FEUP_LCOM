
#include "Game.h"
#include "i8042.h"
#include "i8254.h"
#include "mouse.h"
#include "RTC.h"
#include <math.h>
#include "Serial_port.h"
#include "highscores.h"
#include <time.h>
#include <stdint.h>

int subscribe_periphericals(int *timer_irq, int *mouse_irq, int *keyboard_irq,int *rtc_irq, int 
*uart_irq) {



  uint8_t bit_no;
 

  if (mouse_subscribe(&bit_no)) {
    printf("Error subscribing mouse.\n");
    return 1;
  }

  *mouse_irq = BIT(bit_no);

  // Enable Mouse Data Reporting
  mouse_disable_interrupts();
  mouse_send_command(MOUSE_EN_DATA_REPORTING);
  mouse_enable_interrupts();


  if (keyboard_subscribe(&bit_no)) {
    printf("Error subscribing keyboard.\n");
    return 1;
  }
  *keyboard_irq = BIT(bit_no);



  // Subscribing to the timer
  if (timer_subscribe_int(&bit_no)) {
    printf("Error subscribing timer.\n");
    return 1;
  }

  *timer_irq = BIT(bit_no);


  //Subscribing RTC

  if(rtc_subscribe(&bit_no)){
    printf("Error subscribing the RTC\n");
    return 1;
  }

  *rtc_irq = BIT(bit_no);

  rtc_enable_alarm();
  rtc_enable_periodic();
  rtc_update_enable();

  //Subscribing Serial Port

  if(uart_subscribe(&bit_no)){
    printf("Error subscribing the UART.\n");
    return 1;
  }

  *uart_irq = BIT(bit_no);

  return 0;
}



int unsubscribe_periphericals() {


  // Disables Mouse Data Reporting
  mouse_disable_interrupts();
  mouse_send_command(MOUSE_DIS_DATA_REPORTING);
  mouse_enable_interrupts();


  // Unsubscribing to the Mouse
  if (mouse_unsubscribe()) {
    printf("Error unsubscribing mouse.\n");
  }

  if (keyboard_unsubscribe()) {
    printf("Error unsubscribing keyboard.\n");
    return 1;
  }

  
  if (timer_unsubscribe_int()) {
    printf("Error unsubscribing timer.\n");
    return 1;
  }
  rtc_disable_periodic();
  rtc_update_disable();

  rtc_disable_alarm();

  if(rtc_unsubscribe()){
    printf("Error unsubscribing RTC\n");
    return 1;
  }

  if(uart_unsubscribe()){
    printf("Error unsubscribing UART.\n");
    return 1;
  }

  // Checks if the OBF is full and if it is, it cleans it
  if (mouse_flush_OBF()) {
    printf("Error flushing OBF byte \n");
    return 1;
  }


  return 0;
}

void render() {

  Terminix * terminix = get_current_terminix();

  switch (terminix->state) {
  case MENU_STATE:
    drawMenu();
    break;
  case HIGHSCORE_STATE:
    drawHighscoresMenu();
    break;
  case START_LEVEL_STATE:
    update_level();
  case LEVEL_STATE:
    update_positions();
    break;
  case LEADERBOARD_STATE:
    // printf("Leaderboard\n");
    break;

  case WIN:
    draw_xpm(terminix->menu->youWin_img, 0, 0);
    reset_terminix();
    break;

  case END_STATE:
    
    drawGameOver();
    reset_terminix();

  default:
    break;
  }
 
  update_mouse_img(terminix->mouse);
  update_buffer();
}



void drawMenu(){
  Terminix * terminix = get_current_terminix();
  Menu * menu = terminix->menu;
  draw_xpm(menu->menu_img,0,0);
  draw_xpm(menu->highscore->img,menu->highscore->x,menu->highscore->y);
  
}



void drawGameOver(){
  Terminix * terminix = get_current_terminix();
  draw_xpm(terminix->menu->gameOver_img,0,0);
}


void cleanGameOver(){
  Terminix * terminix = get_current_terminix();
  clean_xpm(terminix->menu->gameOver_img,0,0);
}

void update_positions() {

  // Update hero position
  Terminix *terminix = get_current_terminix();

  draw_chest();

  update_heroes_position(terminix);

  if(terminix->state == END_STATE){
    return;
  }

  update_zombies_position(terminix->zombies, terminix->nr_zombies);

  if(terminix->state == END_STATE){
    return;
  }

  update_bullets_position(terminix);

  if(terminix->nr_dead_zombies == terminix->nr_zombies){
    terminix->state = START_LEVEL_STATE;
    terminix->level++;

    if(terminix->level > terminix->max_level){
      terminix->state = WIN;
      save_score(terminix->nr_player - 1);
    }

  }
  
}

void update_hero_position(Terminix * terminix, int hero_index) {


  Hero * hero = terminix->heroes[hero_index];


  clean_xpm(hero->hero_img, hero->x, hero->y);
  

  if(!check_heroe_friendly_collisions(hero, terminix)){
    // Update Hero position
    hero->x = hero->new_x;



    if(hero->x < 10){
      hero->x = 10;
    }
    else if(hero->x > (terminix->mode_info).XResolution - (hero->hero_img).width){
      hero->x = (terminix->mode_info).XResolution - (hero->hero_img).width;
    }

    hero->y = hero->new_y;
    if(hero->y < 10){
      hero->y = 10;
    }
    else if(hero->y > (terminix->mode_info).YResolution - (hero->hero_img).height){
      hero->y = (terminix->mode_info).YResolution - (hero->hero_img).height;
    }

    if(terminix->heroes[terminix->nr_player - 1] == hero){


      check_hero_direction(hero, terminix->mouse);

    }
  }

  draw_xpm(hero->hero_img, hero->x, hero->y);

  if(check_heroe_enemy_collisions(hero, terminix)){

    clean_xpm(hero->hero_img, hero->x, hero->y);

    if(hero_index == terminix->nr_player - 1){
      save_score(hero_index);
    }

    destroy_hero(&hero);
    terminix->heroes[hero_index] = NULL;
    

    terminix->nr_dead_players++;

    if(terminix->multiplayer && terminix->nr_dead_players == 2){
      terminix->state = END_STATE;
    }
    else if(!terminix->multiplayer){
      terminix->state = END_STATE;
      
    }
  }


}

void update_mouse_img(Mouse * mouse) {
  clean_xpm(mouse->mouse_img, mouse->x, mouse->y);

  // Update mouse position
  mouse->x = mouse->new_x;
  mouse->y = mouse->new_y;

  draw_xpm(mouse->mouse_img, mouse->new_x, mouse->new_y);
}

void update_bullet_position(Bullet * bullet, int hero_index) {

  if(bullet == NULL){
    return;
  }
  Terminix * terminix = get_current_terminix();

  clean_xpm(bullet->sprite->img, bullet->sprite->x, bullet->sprite->y);
  // Update mouse position
  

  bullet->sprite->x += (bullet->x_direction) * (bullet->sprite->speed);
  bullet->sprite->y += (bullet->y_direction) * (bullet->sprite->speed);

  if(check_bullet_colisions(bullet, terminix)){
    destroy_bullet(&bullet);
    terminix->heroes[hero_index]->bullet = NULL;
    return;
  }

  if((bullet->sprite->y < 0) || (bullet->sprite->x < 0) || (bullet->sprite->x > (terminix->mode_info).XResolution - (bullet->sprite->img).width) || (bullet->sprite->y > (terminix->mode_info).YResolution - (bullet->sprite->img).height)){
     destroy_bullet(&bullet);
     terminix->heroes[hero_index]->bullet = NULL;

  } else
  {
    draw_xpm(bullet->sprite->img, bullet->sprite->x, bullet->sprite->y);
  }
  
}

void update_bullets_position(Terminix * terminix){

  for(int i = 0; i < 2; i++){

    if(terminix->heroes[i] != NULL){
      update_bullet_position(terminix->heroes[i]->bullet, i);
    }
  }
}


void update_zombie_position(Zombie * zombie, int zombie_index) {

  Terminix * terminix = get_current_terminix();

  uint16_t old_x = zombie->sprite->x;
  uint16_t old_y = zombie->sprite->y;

  clean_xpm(zombie->sprite->img, zombie->sprite->x, zombie->sprite->y);

  
  char object = check_smallest_distance(terminix, zombie);

  change_zombie_coords(terminix, zombie, object);
  
  if(zombie->sprite->y < 0){
    zombie->sprite->y = 5;
  }
  if(zombie->sprite->x < 0){
    zombie->sprite->x = 5;
  }
  if(zombie->sprite->x > (terminix->mode_info).XResolution){
    zombie->sprite->x = (terminix->mode_info).XResolution- (zombie->sprite->img.width) -5;
  }
  if( zombie->sprite->y > ((terminix->mode_info).YResolution - (zombie->sprite->img).height)){
    zombie->sprite->y = ((terminix->mode_info).YResolution - (zombie->sprite->img).height) -5;
  }
  
  if(check_zombie_collisions(zombie, terminix, zombie_index)){
    zombie->sprite->x  = old_x;
    zombie->sprite->y = old_y;
  }

  draw_xpm(zombie->sprite->img, zombie->sprite->x, zombie->sprite->y);
  
}

void update_zombies_position(Zombie * zombies[], int n){
  
  for(int i = 0; i < n; i++){

    if(zombies[i] == NULL){
      continue;
    }

    update_zombie_position(zombies[i], i);

  }
}

void update_heroes_position(Terminix * terminix){

  for(int i = 0; i < 2; i++){

    if(terminix->heroes[i] == NULL){
      continue;
    }


    update_hero_position(terminix, i);


  }
}

void reset_heroes_position(Terminix * terminix){
  
  for(int i = 0; i < 2; i++){

    if(terminix->heroes[i] == NULL){
      continue;
    }

    reset_hero_position(terminix->heroes[i]);
  }  
}


void update_level(){
  Terminix * terminix = get_current_terminix();

  for(int i = 0; i < 2; i++){
    
    if(terminix->heroes[i] != NULL){
      clean_xpm(terminix->heroes[i]->hero_img, terminix->heroes[i]->x, terminix->heroes[i]->y);
    }
  }

  terminix->nr_dead_zombies = 0;

  if(terminix->nr_zombies == 0){
    terminix->nr_zombies = 2;
  }
  else {
    terminix->nr_zombies = terminix->nr_zombies * 2;
  }

  create_zombies();

  reset_heroes_position(terminix);

  sleep(2);

  terminix->state = LEVEL_STATE;
}

bool check_bullet_colisions(Bullet * bullet, Terminix * terminix){

  uint16_t x_image, y_image, image_width, image_height, bullet_width, bullet_height, bullet_x, bullet_y;

  bullet_width = bullet->sprite->x + (bullet->sprite->img).width;
  bullet_height = bullet->sprite->y + (bullet->sprite->img).height;
  bullet_x = bullet->sprite->x;
  bullet_y = bullet->sprite->y;

  bool colision_found = false;

  for(int i = 0; i < terminix->nr_zombies; i++){

    if(terminix->zombies[i] == NULL){
      continue;
    }

    x_image = terminix->zombies[i]->sprite->x;
    y_image = terminix->zombies[i]->sprite->y;
    image_width = x_image + (terminix->zombies[i]->sprite->img).width;
    image_height = y_image + (terminix->zombies[i]->sprite->img).height;

    
    if((bullet_x >= x_image && bullet_x <= image_width) || ((bullet_width >= x_image) && (bullet_width <= image_width))){

      if((y_image <= bullet_y && bullet_y <= image_height) || ((y_image <= bullet_height) && bullet_height <= image_height)){
        colision_found = true;
      }
      
    }

    if(colision_found){
      clean_xpm(terminix->zombies[i]->sprite->img, terminix->zombies[i]->sprite->x, terminix->zombies[i]->sprite->y);
      destroy_zombie(&(terminix->zombies[i]));
      terminix->zombies[i] = NULL;
      terminix->nr_dead_zombies++;
      int hero_index = terminix->nr_player - 1;
      terminix->heroes[hero_index]->score += 7;
      return true;
    }
  }

  //Chest Colision
  x_image = terminix->chest->x;
  y_image = terminix->chest->y;
  image_width = x_image + (terminix->chest->chest_img).width;
  image_height = y_image + (terminix->chest->chest_img).height;

  if((bullet_x >= x_image && bullet_x <= image_width) || ((bullet_width >= x_image) && (bullet_width <= image_width))){

      if((y_image <= bullet_y && bullet_y <= image_height) || ((y_image <= bullet_height) && bullet_height <= image_height)){
        int hero_index = terminix->nr_player - 1;
        if(terminix->heroes[hero_index]->bullet == bullet){
          terminix->heroes[hero_index]->score -= 4;
        }
        return true;
      }
      
  }
  int enemy_hero_index;
  if(terminix->nr_player == 1){
    enemy_hero_index = 1;
  }
  else{
    enemy_hero_index = 0;
  }

  //Hero Colision
  if(terminix->multiplayer && terminix->heroes[enemy_hero_index] != NULL){
    int hero_index;
    if( terminix->heroes[0]->bullet != NULL && terminix->heroes[0]->bullet == bullet){
      hero_index = 1;
      x_image = terminix->heroes[1]->x;
      y_image = terminix->heroes[1]->y;
      image_width = x_image + (terminix->heroes[1]->hero_img).width;
      image_height = y_image + (terminix->heroes[1]->hero_img).height;
    }
    else if (terminix->heroes[1]->bullet != NULL && terminix->heroes[1]->bullet == bullet){
      hero_index = 0;
      x_image = terminix->heroes[0]->x;
      y_image = terminix->heroes[0]->y;
      image_width = x_image + (terminix->heroes[0]->hero_img).width;
      image_height = y_image + (terminix->heroes[0]->hero_img).height;
    }

    if((bullet_x >= x_image && bullet_x <= image_width) || ((bullet_width >= x_image) && (bullet_width <= image_width))){

      if((y_image <= bullet_y && bullet_y <= image_height) || ((y_image <= bullet_height) && bullet_height <= image_height)){
        return true;
      }
      
    }

  }



  return false;
}

bool check_heroe_enemy_collisions(Hero * hero, Terminix * terminix){
  uint16_t x_image, y_image, image_width, image_height, hero_width, hero_height, hero_x, hero_y;

  hero_width = hero->x + (hero->hero_img).width;
  hero_height = hero->y + (hero->hero_img).height;
  hero_x = hero->x;
  hero_y = hero->y;

  for(int i = 0; i < terminix->nr_zombies; i++){

    if(terminix->zombies[i] == NULL){
      continue;
    }

    x_image = terminix->zombies[i]->sprite->x;
    y_image = terminix->zombies[i]->sprite->y;
    image_width = x_image + (terminix->zombies[i]->sprite->img).width;
    image_height = y_image + (terminix->zombies[i]->sprite->img).height;

    
    if((hero_x >= x_image && hero_x <= image_width) || ((hero_width >= x_image) && (hero_width <= image_width))){

      if((y_image <= hero_y && hero_y <= image_height) || ((y_image <= hero_height) && hero_height <= image_height)){
        return true;
      }
    }
  }

  return false;
}

bool check_heroe_friendly_collisions(Hero * hero, Terminix * terminix){
  
  uint16_t x_image, y_image, image_width, image_height, hero_width, hero_height, hero_x, hero_y;
  
  //Hero to check
  hero_width = hero->new_x + (hero->hero_img).width;
  hero_height = hero->new_y + (hero->hero_img).height;
  hero_x = hero->new_x;
  hero_y = hero->new_y;

  
  //Chest coords
  x_image = terminix->chest->x;
  y_image = terminix->chest->y;
  image_width = x_image + (terminix->chest->chest_img).width;
  image_height = y_image + (terminix->chest->chest_img).height;

  if((hero_x >= x_image && hero_x <= image_width) || ((hero_width >= x_image) && (hero_width <= image_width))){

    if((y_image <= hero_y && hero_y <= image_height) || ((y_image <= hero_height) && hero_height <= image_height)){
      return true;
    }

    if((hero_y <= y_image && y_image <= hero_height) || ((hero_y <= image_height) && image_height <= hero_height)){
      return true;
    }
  }
  else if((x_image >= hero_x && x_image <= hero_width) || ((image_width >= hero_x) && (image_width <= hero_width))){
    
    if((hero_y <= y_image && y_image <= hero_height) || ((hero_y <= image_height) && image_height <= hero_height)){
      return true;
    }
  }
  //Check Colision with other Hero
  if(terminix->multiplayer){
      bool hero_dead = false;
      if(terminix->heroes[0] == hero){

        if(terminix->heroes[1] != NULL){
          x_image = terminix->heroes[1]->x;
          y_image = terminix->heroes[1]->y;
          image_width = x_image + (terminix->heroes[1]->hero_img).width;
          image_height = y_image + (terminix->heroes[1]->hero_img).height;
        }
        else{
          hero_dead = true;
        }
        
      }
      else{
        if(terminix->heroes[0] != NULL){
          x_image = terminix->heroes[0]->x;
          y_image = terminix->heroes[0]->y;
          image_width = x_image + (terminix->heroes[0]->hero_img).width;
          image_height = y_image + (terminix->heroes[0]->hero_img).height;     
        }
        else{
          hero_dead = true;
        }
           
      }

    if(((hero_x >= x_image && hero_x <= image_width) || ((hero_width >= x_image) && (hero_width <= image_width))) && !hero_dead){

      if((y_image <= hero_y && hero_y <= image_height) || ((y_image <= hero_height) && hero_height <= image_height)){
        return true;
      }
    }
  }
  return false;
}


bool check_zombie_collisions(Zombie * zombie, Terminix * terminix, int zombie_index){
  uint16_t x_image, y_image, image_width, image_height, zombie_width, zombie_height, zombie_x, zombie_y;

  //zombie to check
  zombie_width = zombie->sprite->x + (zombie->sprite->img).width;
  zombie_height = zombie->sprite->y + (zombie->sprite->img).height;
  zombie_x = zombie->sprite->x;
  zombie_y = zombie->sprite->y;

  for(int i = zombie_index; i < terminix->nr_zombies; i++){

    if(terminix->zombies[i] == NULL || terminix->zombies[i] == zombie){
      continue;
    }

    x_image = terminix->zombies[i]->sprite->x;
    y_image = terminix->zombies[i]->sprite->y;
    image_width = x_image + (terminix->zombies[i]->sprite->img).width;
    image_height = y_image + (terminix->zombies[i]->sprite->img).height;

    
    if((zombie_x >= x_image && zombie_x <= image_width) || ((zombie_width >= x_image) && (zombie_width <= image_width))){

      if((y_image <= zombie_y && zombie_y <= image_height) || ((y_image <= zombie_height) && zombie_height <= image_height)){
        return true;
      }
    }
  }

  //Chest coords
  x_image = terminix->chest->x;
  y_image = terminix->chest->y;
  image_width = x_image + (terminix->chest->chest_img).width;
  image_height = y_image + (terminix->chest->chest_img).height;
  int hero_index = terminix->nr_player - 1;
  if((zombie_x >= x_image && zombie_x <= image_width) || ((zombie_width >= x_image) && (zombie_width <= image_width))){

    if((y_image <= zombie_y && zombie_y <= image_height) || ((y_image <= zombie_height) && zombie_height <= image_height)){
      save_score(hero_index);
      terminix->state = END_STATE;
      return true;
    }

    if((zombie_y <= y_image && y_image <= zombie_height) || ((zombie_y <= image_height) && image_height <= zombie_height)){
      terminix->state = END_STATE;
      save_score(hero_index);

      return true;
    }
  }
  else if((x_image >= zombie_x && x_image <= zombie_width) || ((image_width >= zombie_x) && (image_width <= zombie_width))){
    
    if((zombie_y <= y_image && y_image <= zombie_height) || ((zombie_y <= image_height) && image_height <= zombie_height)){
      terminix->state = END_STATE;
      save_score(hero_index);

      return true;
    }
  }

  return false;
}

void check_hero_direction(Hero * hero, Mouse * mouse){

  enum direction old_direction = hero->direction; 

  float mouse_middle_x, mouse_middle_y, hero_middle_x, hero_middle_y;

  mouse_middle_x = (mouse->x + (mouse->mouse_img).width) / 2.0;
  mouse_middle_y = (mouse->y + (mouse->mouse_img).height) / 2.0;
  hero_middle_x = (hero->x + (hero->hero_img).width) / 2.0;
  hero_middle_y = (hero->y + (hero->hero_img).height) / 2.0;
  
  float slope = -1.0 * (mouse_middle_y - hero_middle_y) / (mouse_middle_x - hero_middle_x);

  if(mouse_middle_x == hero_middle_x){
    if(mouse_middle_y > hero_middle_y){
      hero->direction = down;
    }
    else{
      hero->direction = up;
    }
  }
  else if(mouse_middle_y == hero_middle_y){
    if(mouse_middle_x > hero_middle_x){
      hero->direction = right;
    }
    else{
      hero->direction = left;
    }
  }
  else if(slope >= 0.3 && slope <= 3){
    if(mouse_middle_x > hero_middle_x){
      hero->direction = ne;
    }
    else{
      hero->direction = sw;
    }
  }
  else if(slope > -0.3 && slope < 0.3){

    if(mouse_middle_x > hero_middle_x){
      hero->direction = right;
    }
    else{
      hero->direction = left;
    }

  }
  else if(slope < -3 || slope > 3){

    if(mouse_middle_y > hero_middle_y){
      hero->direction = down;
    }
    else{
      hero->direction = up;
    }   
  }
  else{
    if(mouse_middle_x > hero_middle_x){
      hero->direction = se;
    }
    else{
      hero->direction = nw;
    }
  }

  Terminix * terminix = get_current_terminix();

  update_xpm(hero);

  if(check_heroe_friendly_collisions(hero, terminix)){
    hero->direction = old_direction;
    update_xpm(hero);
  }




  if(old_direction != hero->direction && terminix->multiplayer){
    char message[3];

    message[0] = 'D';
    if(terminix->nr_player == 1){
      message[1] = '1';
    }
    else {
      message[1] = '2';
    }

    if(hero->direction == up){
      message[2] = 'u';
    }
    else if(hero->direction == down){
      message[2] = 'd';
    }
    else if(hero->direction == left){
      message[2] = 'l';
    }
    else if(hero->direction == right){
      message[2] = 'r';
    }
    else if(hero->direction == ne){
      message[2] = 'n';
    }
    else if(hero->direction == nw){
      message[2] = 'w';
    }
    else if(hero->direction == sw){
      message[2] = 's';
    }
    else if(hero->direction == se){
      message[2] = 'e';
    }

    uart_send_message(message, 3);
  }
  
}


char check_smallest_distance(Terminix * terminix, Zombie * zombie){
  uint16_t zombie_x = zombie->sprite->x, zombie_y = zombie->sprite->y;

  int distance_to_chest =sqrt(pow(zombie_x - terminix->chest->x, 2) + pow(zombie_y - terminix->chest->y, 2));
  int distance_to_hero1 = 1200, distance_to_hero2 = 1200;

  if(terminix->heroes[0] != NULL){
    distance_to_hero1 =sqrt(pow(zombie_x - terminix->heroes[0]->x, 2) + pow(zombie_y - terminix->heroes[0]->y, 2));
  }

  if(terminix->heroes[1] != NULL){
    distance_to_hero2 =sqrt(pow(zombie_x - terminix->heroes[1]->x, 2) + pow(zombie_y - terminix->heroes[1]->y, 2));
  }

  if(distance_to_chest < distance_to_hero1 && distance_to_chest < distance_to_hero2){
    return 'c';
  }
  else if(distance_to_hero1< distance_to_chest && distance_to_hero1 < distance_to_hero2){
    return 'h';
  }
  else {
    return 'm';
  }

}

void change_zombie_coords(Terminix * terminix, Zombie * zombie, char object){

  switch (object)
  {
  case 'h':
    if (terminix->heroes[0]->x > zombie->sprite->x){
      zombie->sprite->x += (zombie->sprite->speed);
      zombie->sprite->img = zombie->sprites[2];
    }
    if (terminix->heroes[0]->x < zombie->sprite->x){
      zombie->sprite->x -= (zombie->sprite->speed);
      zombie->sprite->img = zombie->sprites[1];

    }
    if (terminix->heroes[0]->y > zombie->sprite->y){
      zombie->sprite->y += (zombie->sprite->speed);
      zombie->sprite->img = zombie->sprites[3];

    }
    if (terminix->heroes[0]->y < zombie->sprite->y){
      zombie->sprite->y -= (zombie->sprite->speed);
      zombie->sprite->img = zombie->sprites[0];
    }
    break;
  
  case 'm':
    if (terminix->heroes[1]->x > zombie->sprite->x){
      zombie->sprite->x += (zombie->sprite->speed);
      zombie->sprite->img = zombie->sprites[2];
    }
    if (terminix->heroes[1]->x < zombie->sprite->x){
      zombie->sprite->x -= (zombie->sprite->speed);
      zombie->sprite->img = zombie->sprites[1];

    }
    if (terminix->heroes[1]->y > zombie->sprite->y){
      zombie->sprite->y += (zombie->sprite->speed);
      zombie->sprite->img = zombie->sprites[3];

    }
    if (terminix->heroes[1]->y < zombie->sprite->y){
      zombie->sprite->y -= (zombie->sprite->speed);
      zombie->sprite->img = zombie->sprites[0];
    }
    break;
  
  case 'c':
    if (terminix->chest->x > zombie->sprite->x){
      zombie->sprite->x += (zombie->sprite->speed);
      zombie->sprite->img = zombie->sprites[2];
    }
    if (terminix->chest->x < zombie->sprite->x){
      zombie->sprite->x -= (zombie->sprite->speed);
      zombie->sprite->img = zombie->sprites[1];

    }
    if (terminix->chest->y > zombie->sprite->y){
      zombie->sprite->y += (zombie->sprite->speed);
      zombie->sprite->img = zombie->sprites[3];

    }
    if (terminix->chest->y < zombie->sprite->y){
      zombie->sprite->y -= (zombie->sprite->speed);
      zombie->sprite->img = zombie->sprites[0];
    }
    break;
    
  default:
    break;
  }
}


void change_other_hero_direction(char message, Hero * hero){
  switch (message)
  {
  case 'u':
    hero->direction = up;
    break;
  case 'd':
    hero->direction = down;
    break;
  case 'r':
    hero->direction = right;
    break;
  case 'l':
    hero->direction = left;
    break;
  case 'n':
    hero->direction = ne;
    break;
  case 'w':
    hero->direction = nw;
    break;
  case 's':
    hero->direction = sw;
    break;
  case 'e':
    hero->direction = se;
    break;
  default:
    break;
  }

  update_xpm(hero);
}

void change_other_hero_position(char message, Hero * hero){
    switch (message)
  {
  case 'u':
    hero_move_up(hero);
    break;
  case 'd':
    hero_move_down(hero);
    break;
  case 'r':
    hero_move_right(hero);
    break;
  case 'l':
    hero_move_left(hero);
    break;
  default:
    break;
  }


}


void send_movement_info(Terminix * terminix, char movement){
  char message[3];

  message[0] = 'M';

  if(terminix->nr_player == 1){
    message[1] = '1';
  }
  else {
    message[1] = '2';
  }  
  message[2] = movement;

  uart_send_message(message, 3);

}



void uart_message_analyse(char * message){
  Terminix * terminix = get_current_terminix();

  switch (message[0])
  {
  case 'P':
    if(message[1] == '1'){
      terminix->nr_player = 2;
    }
    else{
      terminix->state = START_LEVEL_STATE;
      clean_waitingScreenImg(terminix->menu);
      draw_xpm(terminix->background_xpm,0,0);
    }
    break;

  case 'S':

    if(message[1] == '1'){
      terminix->heroes[0]->bullet = shoot(terminix->heroes[0]);
    }
    else{
      terminix->heroes[1]->bullet = shoot(terminix->heroes[1]);
    }
    break;

  case 'D':
    if(message[1] == '1'){
      change_other_hero_direction(message[2],terminix->heroes[0]);
    }
    else{
      change_other_hero_direction(message[2],terminix->heroes[1]);
    }
    break;  
  
  case 'M':

    if(message[1] == '1'){
      change_other_hero_position(message[2],terminix->heroes[0]);
    }
    else{
      change_other_hero_position(message[2],terminix->heroes[1]);
    }
    break; 
  default:
    break;
  }
}
