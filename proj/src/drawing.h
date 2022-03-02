 /**
 * @file drawing.h
 * @author Bernardo Ramalho & Tiago Alves
 * @brief Classe responsible for drawing images into the screen and animating them
 * @date 2020-01-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

 #ifndef _DRAWING_H_
 #define _DRAWING_H_

 #pragma once
  
 #include <stdbool.h>
 #include <stdint.h>
 #include <minix/driver.h>
 #include <sys/mman.h>
 #include <lcom/lcf.h>
 
 
/** @defgroup Drawing Drawing
 * @{
 *
 * Responsible for all the GUI related operations
 */
 #define SET_GRAPHIC_MODE 0x4F02
 #define RETURN_VBE_INFO 0x4F00
 #define GET_VBE_MODE_INFO 0x4F01
 #define INDEXED_VIDEO_MODE 0x105
 #define RGB_VIDEO_MODE 0x14c
 #define LINEAR_MODE_MASK 1<<14 
 #define FRAME_RATE 60

  /**
   * @brief Get the horizontal resolution of the sceen
   * 
   * @return int - represents the horizontal resolution
   */
  int get_hres();

  /**
   * @brief Get the vertical resolution of the sceen
   * 
   * @return int - represents the vertical resolution
   */
  int get_vres();

  /**
   * @brief calculates the red component of a pixel
   * 
   * @param row - row of the pixel
   * @param col - column of the pixel
   * @param first - Color to be used in the first rectangle (the rectangle at the top-left corner of the screen)
   * @param step - Difference between the values of the R/G component in adjacent rectangles in the same row/column
   * @param mode_info - object containing the information of the vbe mode
   * @return int - value of the red component
   */
  int calculate_Red_Component(int row, int col, int first, uint8_t step, vbe_mode_info_t *mode_info);

  /**
   * @brief calculates the green component of a pixel
   * 
   * @param row - row of the pixel
   * @param col - column of the pixel
   * @param first - Color to be used in the first rectangle (the rectangle at the top-left corner of the screen)
   * @param step - Difference between the values of the R/G component in adjacent rectangles in the same row/column
   * @param mode_info - object containing the information of the vbe mode
   * @return int - value of the green component
   */
  int calculate_Green_Component(int row, int col, int first, uint8_t step, vbe_mode_info_t *mode_info);

  /**
   * @brief calculates the blue component of a pixel
   * 
   * @param row - row of the pixel
   * @param col - column of the pixel
   * @param first - Color to be used in the first rectangle (the rectangle at the top-left corner of the screen)
   * @param step - Difference between the values of the R/G component in adjacent rectangles in the same row/column
   * @param mode_info - object containing the information of the vbe mode
   * @return int - value of the blue component
   */
  int calculate_Blue_Component(int row, int col, int first, uint8_t step, vbe_mode_info_t *mode_info);

  /**
   * @brief Calculates the red, blue and green component values of a pixel
   * 
   * @@param row - row of the pixel
   * @param col - column of the pixel
   * @param first - Color to be used in the first rectangle (the rectangle at the top-left corner of the screen)
   * @param step - Difference between the values of the R/G component in adjacent rectangles in the same row/column
   * @param mode_info - object containing the information of the vbe mode
   * @return int - value of the colour of the pixel 
   */
  int calculate_RGB(int row, int col, int first, uint8_t step, vbe_mode_info_t *mode_info);

  /**
   * @brief Draws a pixel in the screen
   * 
   * @param x - position in the x axis of the screen
   * @param y - position in the y axis of the screen
   * @param color - color to be drawn in the screen
   * @return int - 0 if sucess or 1 if error
   */
  int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);

  /**
   * @brief Initializes the graphics card mode
   * 
   * @param mode - mode to be initialize
   * @return int - 0 if sucess or 1 if error
   */
  int graphic_mode_init(uint16_t mode);

  /**
   * @brief draws a given xpm in the screen
   * 
   * @param img - image to be drawn
   * @param x - position in the x axis of the screen
   * @param y - position in the y axis of the screen
   */
  void draw_xpm(xpm_image_t img, uint16_t x, uint16_t y);

  /**
   * @brief Converts an xpm_map_t into a xpm_image_t
   * 
   * @param xpm - xpm to be converted
   * @param img - image that saves the infor of the xpm
   */
  void convert_xpm_img(xpm_map_t xpm,xpm_image_t *img);

  /**
   * @brief Erases a given xpm
   * 
   * @param img - image to be drawn
   * @param x - position in the x axis of the screen
   * @param y - position in the y axis of the screen 
   */
  void clean_xpm(xpm_image_t img, uint16_t x, uint16_t y);

  /**
   * @brief Get the vbe mode info 
   * 
   * @param mode - specifies the mode which to get the information
   * @param mode_info - object to save the information into
   * @return int - 0 if sucess or 1 if error 
   */
  int get_vbe_mode_info(uint16_t mode, vbe_mode_info_t *mode_info);

  /**
   * @brief Updates the buffer
   * 
   */
  void update_buffer();



/**@}*/

#endif 

