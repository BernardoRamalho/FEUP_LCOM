#include "drawing.h"
#include "Terminix.h"
#include "RTC.h"
#include "highscores.h"
#include "Images/one.xpm"
#include "Images/two.xpm"
#include "Images/three.xpm"
#include "Images/four.xpm"
#include "Images/five.xpm"
#include "Images/six.xpm"
#include "Images/seven.xpm"
#include "Images/eight.xpm"
#include "Images/nine.xpm"
#include "Images/zero.xpm"
#include "Images/slash.xpm"
#include "Images/points.xpm"
#include "Images/date.xpm"
#include "Images/time.xpm"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <lcom/lcf.h>

static xpm_image_t numbers_xpms[10];
Highscore * highscores[5];


void load_numbers_xpms(){

  xpm_image_t one,two,three,four,five,six,seven,eight,nine,zero;
  convert_xpm_img(one_xpm,&one);
  convert_xpm_img(two_xpm,&two);
  convert_xpm_img(ntres_xpm,&three);
  convert_xpm_img(nquatro_xpm,&four);
  convert_xpm_img(ncinco_xpm,&five);
  convert_xpm_img(nseis_xpm,&six);
  convert_xpm_img(nsete_xpm,&seven);
  convert_xpm_img(noito_xpm,&eight);
  convert_xpm_img(nnove_xpm,&nine);
  convert_xpm_img(zero_xpm,&zero);
 

  numbers_xpms[0] = zero;
  numbers_xpms[1] = one;
  numbers_xpms[2] = two;
  numbers_xpms[3] = three;
  numbers_xpms[4] = four;
  numbers_xpms[5] = five;
  numbers_xpms[6] = six;
  numbers_xpms[7] = seven;
  numbers_xpms[8] = eight;
  numbers_xpms[9] = nine;
}

void save_score(int hero_index){

   
    Terminix * terminix = get_current_terminix();

    uint16_t score = terminix->heroes[hero_index]->score;

    FILE *file;

    file = fopen("/home/lcom/labs/proj/src/scores.txt", "w");


    if (file == NULL)  return;

    bool found = false;

    for (int i = 0; i < 5; i++){

        if (highscores[i] != NULL){
        
            if (score > highscores[i]->score){
                Highscore  * highscore = create_highscore(score,rtc_get_day(), rtc_get_month(),rtc_get_year(), rtc_get_hour(), rtc_get_min(), rtc_get_sec());
                if(!found) highscores[i] = highscore;
                found = true;

               
            }

            fprintf(file, "%d %d %d %d %d %d %d\n", highscores[i]->score,highscores[i]->day, highscores[i]->month, highscores[i]->year, highscores[i]->hour, highscores[i]->minute, highscores[i]->sec);
        }
        
    }
    found = false;
    
    
    fclose(file);
    

    
    

}

void print_digit(unsigned num,uint16_t x, uint16_t y){

    unsigned digits = floor(log10(num))+1;
    unsigned inter = num;
    


    for (unsigned i = 0; i < digits; i++){

        int actualNum = inter / pow(10,digits-i-1);
        inter = num % (unsigned)pow(10,digits-i-1);
      
        xpm_image_t image = numbers_xpms[actualNum];
        draw_xpm(image,x+i*40,y);
    }
    

    
}



void read_highscores(){

    FILE * fp;
    int counter = 0;

    fp = fopen("/home/lcom/labs/proj/src/scores.txt", "r");

    if (fp == NULL){
        printf("Error opening file\n");
        return;
    }


    while(counter <= 5){
 
        int day, month, year, hours, mins,secs;
        int score;

        fscanf(fp,"%d %d %d %d %d %d %d",&score,&day, &month, &year, &hours, &mins, &secs);

        highscores[counter] = create_highscore(score,day,month,year,hours,mins,secs);


        counter++;

    }

    fclose(fp);
}

void print_highscores(){

    xpm_image_t slash,points;

    convert_xpm_img(slash_xpm,&slash);
    convert_xpm_img(points_xpm,&points);

    for (int i = 0; i < 5; i++){
        print_digit(highscores[i]->score,140,400+i*70);


        print_digit(highscores[i]->day,400,400+i*70);
        draw_xpm(slash,440,400+i*70);
        print_digit(highscores[i]->month,480,400+i*70);
        draw_xpm(slash,500,400+i*70);
        print_digit(highscores[i]->year,540,400+i*70);

        print_digit(highscores[i]->hour,800,400+i*70);
        draw_xpm(points,860,400+i*70);
        print_digit(highscores[i]->minute,900,400+i*70);
        draw_xpm(points,960,400+i*70);
        print_digit(highscores[i]->sec,1000,400+i*70);




    }
    
}

void drawHighscoresMenu(){

  xpm_image_t date,time;

  convert_xpm_img(date_xpm,&date);
  convert_xpm_img(time_xpm,&time);

  Terminix * terminix = get_current_terminix();
  Menu * menu = terminix->menu;
  draw_xpm(menu->back_img,0,0);
  draw_xpm(menu->score_img,100,300);
  draw_xpm(menu->highscore->img,200,100);
  draw_xpm(date,400,300);
  draw_xpm(date,800,300);
  if(terminix->menu->easterEgg){
      draw_xpm(terminix->menu->easterEgg_img,900,100);
  }
  print_highscores();
  
}

Highscore * create_highscore(int score, int day,int month,int year,int hour,int minute,int second){

    Highscore * highscore_ptr = malloc(sizeof (Highscore));


    highscore_ptr->score = score;
    highscore_ptr->day = day;
    highscore_ptr->month = month;
    highscore_ptr->year = year;
    highscore_ptr->hour = hour;
    highscore_ptr->minute = minute;
    highscore_ptr->sec = second;

  
  return highscore_ptr;
}


void destroy_highscore(Highscore ** highscore) {

  if(*highscore == NULL){
    return;
  }
  free(*highscore);
  *highscore = NULL;
}

