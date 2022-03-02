#include <lcom/lcf.h>
#include <stdint.h>
#include "drawing.h"
#include "math.h"
#include "i8042.h"
#include "i8254.h"

static char *video_mem;		/* Process (virtual) address to which VRAM is mapped */

static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static int bytes_per_pixel; /* Number of VRAM bits per pixel */

extern uint8_t status_code, scan_code;
extern uint32_t global_counter;


int get_hres(){
  return h_res;
}

int get_vres(){
  return v_res;
}

int get_vbe_mode_info(uint16_t mode, vbe_mode_info_t *mode_info){
  mmap_t map;

  if(lm_alloc(sizeof(vbe_mode_info_t), &map) == NULL){
    printf("Couldn't allocat memory.\n");
    return 1;
  }

  struct reg86 rg;

  //Resests the values in all atributes of rg
  memset(&rg, 0, sizeof(rg));

  //Inputs to Function 01h of VBE
  rg.ax = GET_VBE_MODE_INFO;
  rg.cx = mode;
  rg.es = PB2BASE(map.phys);
  rg.di = PB2OFF(map.phys);
  rg.intno = 0x10;

  if(sys_int86(&rg) != OK){
    printf("set_vbe_mode: sys_int86() failed \n");
    return 1;
  }

  *mode_info = *(vbe_mode_info_t *)map.virt;
  lm_free(&map);
  return 0;
}

int (graphic_mode_init)(uint16_t mode, vbe_mode_info_t *mode_info){

  //Save information about the VBE mode in mode_info
  if(get_vbe_mode_info(mode, mode_info)){
    printf("Error reading vbe mode info.");
    return 1;
  }                  

  //Initialize all variables needed
  int r;
  struct minix_mem_range mr;
  unsigned int vram_base;
  unsigned int vram_size;
  bytes_per_pixel = ceil(mode_info->BitsPerPixel / (double)8);
  h_res = mode_info->XResolution;
  v_res = mode_info->YResolution;

  //Saves VRAM address and size
  vram_base = mode_info->PhysBasePtr;
  vram_size = bytes_per_pixel * mode_info->XResolution * mode_info->YResolution;

  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
   panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if(video_mem == MAP_FAILED)
    panic("couldn't map video memory");

  struct reg86 rg;

  memset(&rg, 0, sizeof(rg));

  //Inputs to Function 02h of VBE
  rg.ax = SET_GRAPHIC_MODE;
  rg.bx = LINEAR_MODE_MASK | mode;
  rg.intno = 0x10;

  if(sys_int86(&rg) != OK){
    printf("set_vbe_mode: sys_int86() failed \n");
    return 1;
  }

  return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){

  //Prints len number of pixels
  for(int i = 0; i < len; i++){
    vg_draw_pixel(x + i, y, color);
  }

  return 0;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color){

  //If the x is bigger then the horizontal resolution or smaller then 0,
  //it can't draw the pixel. The same if y is bigger then the vertical resolution
  if ((x >= h_res || y >= v_res) && (x < 0 || y < 0)) {
    return 1;
  }
  
  char * pixel_init = ((char *)video_mem + (y*h_res + x)*bytes_per_pixel);

  for(int i = 0; i < bytes_per_pixel; i++ ){
    *pixel_init = color;  
     pixel_init++;
     color = color >> 8;
    
  }
  
  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){

  //Draws a number of lines equal to height
  for(int i = 0; i <height; i++){
    vg_draw_hline(x, y + i, width, color);
  }

  return 0;
}

int calculate_index(int row, int col, int first, int no_retangles, uint8_t step){
  return (first + (row * no_retangles + col) * step) % (1 << (bytes_per_pixel * 8));
}

int calculate_Red_Component(int row, int col, int first, uint8_t step, vbe_mode_info_t *mode_info){
  int red_component;

  if(row == 0 && col == 0){
    return first >> mode_info->RedFieldPosition & (BIT(mode_info->RedMaskSize)-1); 
  }

  red_component = (calculate_Red_Component(0,0, first, step, mode_info) + col*step) % (1 << mode_info->RedMaskSize);

  return red_component;
}

int calculate_Green_Component(int row, int col, int first, uint8_t step, vbe_mode_info_t *mode_info){

  //First colour
  if(row == 0 && col == 0){
    return first >> mode_info->GreenFieldPosition & (BIT(mode_info->GreenMaskSize) - 1);  ;
  }

  int green_component = (calculate_Green_Component(0, 0, first, step, mode_info) + row * step) % (1 << mode_info->GreenMaskSize);

  return green_component;

}

int calculate_Blue_Component(int row, int col, int first, uint8_t step, vbe_mode_info_t *mode_info){
  if(row == 0 && col == 0){
    return first >> mode_info->BlueFieldPosition & (BIT(mode_info->BlueMaskSize)-1);
  }

  int blue_component = (calculate_Blue_Component(0, 0, first, step, mode_info) + (col + row) * step) % (1 << mode_info->BlueMaskSize);

  return blue_component;
}

int calculate_RGB(int row, int col, int first, uint8_t step, vbe_mode_info_t *mode_info){

  int red_component = calculate_Red_Component(row, col, first, step, mode_info);
  int green_component = calculate_Green_Component(row, col, first, step, mode_info);
  int blue_component = calculate_Blue_Component(row, col, first, step, mode_info);

  return (red_component << mode_info->RedFieldPosition) | (green_component << mode_info ->GreenFieldPosition) | (blue_component << mode_info->BlueFieldPosition); 
}

void draw_xpm(xpm_image_t img, uint16_t x, uint16_t y){

  int byte_index = 0;

    for(uint16_t yi = y; yi < y + img.height; yi++ ){

        for(uint16_t xi = x; xi < x + img.width; xi++){
            vg_draw_pixel(xi, yi, img.bytes[byte_index]);
            byte_index++;
        }
    }
} 

void clean_xpm(xpm_image_t img, uint16_t x, uint16_t y){

  int color = xpm_transparency_color(XPM_INDEXED);

  for(uint16_t yi = y; yi < y + img.height; yi++ ){

    for(uint16_t xi = x; xi < x + img.width; xi++){
      vg_draw_pixel(xi, yi, color);
    }
  }
}

  void animate_xpm(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,int16_t speed, uint8_t fr_rate){

    //Timer && Keyboard Variables
    int ipc_status, r, irq_kbd = 0, irq_timer0 = 0;
    message msg;
    uint8_t bit_no;

    //Drawing Variables
    xpm_image_t img;
    int frame_counter = 0;

    //Gets the pixmap
    uint8_t * map;
    map = xpm_load(xpm, XPM_INDEXED, &img);
    draw_xpm(img, xi, yi);

    //Subscribing to the timer
  if(timer_subscribe_int(&bit_no)){
    printf("Error subs return 1;cribing timer in lab3.\n");
    return;
  }
  irq_timer0 = BIT(bit_no);

  //Subscribing to the keyboard
  if(keyboard_subscribe(&bit_no)){
    printf("Error subscribing keyboard in lab3.\n");
    return;
  }
  irq_kbd = BIT(bit_no);

  while( scan_code != ESC_BREAK_CODE){ 

    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
    }
    
    if (is_ipc_notify(ipc_status)) { // received notification
        
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */	

              if (msg.m_notify.interrupts & irq_kbd) { /* subscribed interrupt */
                
                if(util_sys_inb(STAT_REG, &status_code)){
                  printf("Error reading status code.");
                  return;
                }

                //Calls the keyboard interrupt
                kbc_ih();
              } 
              if (msg.m_notify.interrupts & irq_timer0 && (xf != xi || yf != yi)) { /* subscribed interrupt */

                  timer_int_handler();

                  if(global_counter % (sys_hz() / fr_rate) == 0){

                    if(speed >= 0){
                      clean_xpm(img, xi, yi);

                      xi += speed; 

                      if(xi > xf){
                        xi = xf;
                      }

                      yi += speed;

                      if(yi > yf){
                        yi = yf;
                      }
                    
                      draw_xpm(img, xi, yi);
                    }
                    else{
                      frame_counter++;

                      if(frame_counter == -1*speed){
                        clean_xpm(img, xi, yi);

                        xi += 1; 

                       if(xi > xf){
                         xi = xf;
                        }

                       yi += 1;

                       if(yi > yf){
                         yi = yf;
                      }
                    
                       draw_xpm(img, xi, yi);

                       frame_counter = 0;
                      }
                    }

                  }

              }
              
              break;
            default:
              break; /* no other notifications expected: do nothing */	
        }
    }
  }

  clean_xpm(img, xf, yf);

  //Checks if the OBF is full and if it is cleans it
  if(flush_OBF()){
    printf("Error flushing OBS.\n");
    return;
  }

  if(keyboard_unsubscribe()){
    printf("Error unsubscribing in lab3.\n");
    return;
  }

  if(timer_unsubscribe_int()){
    printf("Error unsubscribing timer in lab3.\n");
    return;
  }


}
