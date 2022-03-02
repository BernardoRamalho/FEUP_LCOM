 #pragma once
  
 #include <stdbool.h>
 #include <stdint.h>
 #include <minix/driver.h>
 #include <sys/mman.h>
 #include <lcom/lcf.h>

 #define SET_GRAPHIC_MODE 0x4F02
 #define RETURN_VBE_INFO 0x4F00
 #define GET_VBE_MODE_INFO 0x4F01
 #define INDEXED_VIDEO_MODE 0x105
 #define LINEAR_MODE_MASK 1<<14 

 typedef struct {
  uint8_t vbe_signature[4];
  uint8_t vbe_version[2];
  phys_bytes oem_string_ptr;
  uint8_t capabilities[4];
  phys_bytes mode_list_ptr;
  uint16_t total_memory;
  uint16_t oem_software_rev;
  phys_bytes oem_vendor_name_ptr;
  phys_bytes oem_product_name_ptr;
  phys_bytes oem_product_rev_ptr;
  uint8_t reserved[222];
  uint8_t oem_data[256];
} VbeControllerInfo;


 /**
* Returns the value of horizontal resolution
* @return an int representing the horizontal resolution
*/
  int get_hres();

 /**
  *@brief Returns the value of vertical resolution
 **/
  int get_vres();

  /**
   * Calculates de index of the color to be printed
   */
  int calculate_index(int row, int col, int first, int no_rectangles, uint8_t step);

 /**
  * Calculates the red component of the color to be printed
  */
  int calculate_Red_Component(int row, int col, int first, uint8_t step, vbe_mode_info_t *mode_info);

  /**
  * Calculates the green component of the color to be printed
  */
  int calculate_Green_Component(int row, int col, int first, uint8_t step, vbe_mode_info_t *mode_info);

   /**
   * Calculates the blue component of the color to be printed
   */
  int calculate_Blue_Component(int row, int col, int first, uint8_t step, vbe_mode_info_t *mode_info);

  /**
  * Calculates the full RGB  to be printed
  */
  int calculate_RGB(int row, int col, int first, uint8_t step, vbe_mode_info_t *mode_info);

  /**
   * Draws a rectangle on the screen .The left most corner is placed in the coordinates x and y
   * The rectangle as dimensions width and height and is printed with color
   */
  int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

  /**
   * Draws a line on the screen with length len, beggining in the coordinates (x,y)
   */
  int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

  /**
   * Draws a pixel in the coordinates (x,y) and prints it with colour = color
   */
  int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);

  /**
   * Initializes the graphic mode of VBE
   * @param mode - mode to be initialize
   * @param mode_info - information about all the mode
   */
  int graphic_mode_init(uint16_t mode, vbe_mode_info_t *mode_info);

  /**
   * Draws the xpm saved in the img starting in the coordinates (x,y)
   */
  void draw_xpm(xpm_image_t img, uint16_t x, uint16_t y);

  /**
  * Changes the color of the xpm image to trasnparent
  */
  void clean_xpm(xpm_image_t img, uint16_t x, uint16_t y);

  /**
   * Animates an xpm image. The animation starts at coordinates (xi, yi) and ends
   * at coordinates (xf, yf).
   * It travels with speed equal to speed and wih frame rates = to fr_rate
   */
  void animate_xpm(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,int16_t speed, uint8_t fr_rate);

  /**
   * Returns the information about the VBE control
   */
  void return_vbe_ctr_info(vg_vbe_contr_info_t * info);

  /**
   * Saves the VBE mode info into the variable mode_info
   */
  int get_vbe_mode_info(uint16_t mode, vbe_mode_info_t *mode_info);

