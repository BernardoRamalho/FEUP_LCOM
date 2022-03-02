// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include "i8042.h"
#include "i8254.h"
#include "drawing.h"
#include "lcom/vbe.h"

#include <stdint.h>
#include <stdio.h>

extern uint32_t global_counter;
extern uint8_t status_code, scan_code;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {

  vbe_mode_info_t mode_info;

  graphic_mode_init(mode, &mode_info);

  sleep(delay);


  vg_exit();

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {

  vbe_mode_info_t mode_info;

  graphic_mode_init(mode, &mode_info);

  vg_draw_rectangle(x, y, width, height, color);

  run_until_ESC_key();

  vg_exit();
  
  return 1;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {

  vbe_mode_info_t  mode_info;

  graphic_mode_init(mode, &mode_info);

  int widht = get_hres() / no_rectangles;
  int height = get_vres() / no_rectangles;

  int color = first;

  for (int i = 0; i < no_rectangles; i++){

    for (int j = 0; j < no_rectangles; j++){


      if(mode == 0x105){
        color = calculate_index(j, i, first, no_rectangles, step);
      }
      else {
        color = calculate_RGB(j, i, first, step, &mode_info);
      }

      vg_draw_rectangle(i*widht , j*height ,widht , height, color);

    }   
  }

  run_until_ESC_key();

  vg_exit();


  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  
  vbe_mode_info_t  mode_info;

  //Initializes the graphic mode
  graphic_mode_init(INDEXED_VIDEO_MODE, &mode_info);

  //Loads the pixmap from the XPM Image
  xpm_image_t img;
  uint8_t * map;
  map = xpm_load(xpm, XPM_INDEXED, &img);

  draw_xpm(img, x, y);

  run_until_ESC_key();

  vg_exit();

  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,int16_t speed, uint8_t fr_rate) {
  
   vbe_mode_info_t  mode_info;

   //Initializes the graphic mode
   graphic_mode_init(INDEXED_VIDEO_MODE, &mode_info);

  animate_xpm(xpm,xi,yi, xf, yf, speed, fr_rate);

  vg_exit();
  
  return 0;

}

int(video_test_controller)() {

  return 1;
}
