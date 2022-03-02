#include "drawing.h"
#include "i8042.h"
#include "i8254.h"
#include "math.h"
#include "Terminix.h"
#include <lcom/lcf.h>
#include <stdint.h>

static char *video_mem; /* Process (virtual) address to which VRAM is mapped */
static char *buffer;    /* Process (virtual) address to which VRAM is mapped */

static unsigned h_res;      /* Horizontal resolution in pixels */
static unsigned v_res;      /* Vertical resolution in pixels */
static int bytes_per_pixel; /* Number of VRAM bits per pixel */



int get_hres() { return h_res; }

int get_vres() { return v_res; }

int get_vbe_mode_info(uint16_t mode, vbe_mode_info_t *mode_info) {
  mmap_t map;

  if (lm_alloc(sizeof(vbe_mode_info_t), &map) == NULL) {
    printf("Couldn't allocat memory.\n");
    return 1;
  }

  struct reg86 rg;

  memset(&rg, 0, sizeof(rg));

  rg.ax = GET_VBE_MODE_INFO;
  rg.cx = mode;
  rg.es = PB2BASE(map.phys);
  rg.di = PB2OFF(map.phys);
  rg.intno = 0x10;

  if (sys_int86(&rg) != OK) {
    printf("set_vbe_mode: sys_int86() failed \n");
    return 1;
  }

  *mode_info = *(vbe_mode_info_t *)map.virt;
  lm_free(&map);
  return 0;
}

int(graphic_mode_init)(uint16_t mode) {

  vbe_mode_info_t mode_info;

  if (get_vbe_mode_info(mode, &mode_info)) {
    printf("Error reading vbe mode info.");
    return 1;
  }

  int r;
  struct minix_mem_range mr;
  unsigned int vram_base;
  unsigned int vram_size;
  bytes_per_pixel = ceil(mode_info.BitsPerPixel / (double)8);
  h_res = mode_info.XResolution;
  v_res = mode_info.YResolution;

  vram_base = mode_info.PhysBasePtr;
  vram_size = bytes_per_pixel * mode_info.XResolution * mode_info.YResolution;

  mr.mr_base = (phys_bytes)vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
  buffer = malloc(vram_size);

  if (video_mem == MAP_FAILED)
    panic("couldn't map video memory");

  struct reg86 rg;

  memset(&rg, 0, sizeof(rg));

  rg.ax = SET_GRAPHIC_MODE;
  rg.bx = LINEAR_MODE_MASK | mode;
  rg.intno = 0x10;

  if (sys_int86(&rg) != OK) {
    printf("set_vbe_mode: sys_int86() failed \n");
    return 1;
  }

  return 0;
}

//Para ir para a proxima linha entao so adicionar hres
int(vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {

  if ((x >= h_res || y >= v_res) && (x < 0 || y < 0)) {
    return 1;
  }

  if (color == TRANSPARENCY_COLOR_8_8_8_8) {
    return 0;
  }

  uint32_t *pixel = ((uint32_t *)buffer + (y * h_res + x));

  *pixel = color;

  return 0;
}

int calculate_Red_Component(int row, int col, int first, uint8_t step,
                            vbe_mode_info_t *mode_info) {
  int red_component;

  if (row == 0 && col == 0) {
    return first >> mode_info->RedFieldPosition &
           (BIT(mode_info->RedMaskSize) - 1);
  }

  red_component =
      (calculate_Red_Component(0, 0, first, step, mode_info) + col * step) %
      (1 << mode_info->RedMaskSize);

  return red_component;
}

int calculate_Green_Component(int row, int col, int first, uint8_t step,
                              vbe_mode_info_t *mode_info) {

  if (row == 0 && col == 0) {
    return first >> mode_info->GreenFieldPosition &
           (BIT(mode_info->GreenMaskSize) - 1);
    ;
  }

  int green_component =
      (calculate_Green_Component(0, 0, first, step, mode_info) + row * step) %
      (1 << mode_info->GreenMaskSize);

  return green_component;
}

int calculate_Blue_Component(int row, int col, int first, uint8_t step,
                             vbe_mode_info_t *mode_info) {
  if (row == 0 && col == 0) {
    return first >> mode_info->BlueFieldPosition &
           (BIT(mode_info->BlueMaskSize) - 1);
  }

  int blue_component = (calculate_Blue_Component(0, 0, first, step, mode_info) +
                        (col + row) * step) %
                       (1 << mode_info->BlueMaskSize);

  return blue_component;
}

int calculate_RGB(int row, int col, int first, uint8_t step,
                  vbe_mode_info_t *mode_info) {

  int red_component = calculate_Red_Component(row, col, first, step, mode_info);
  int green_component =
      calculate_Green_Component(row, col, first, step, mode_info);
  int blue_component =
      calculate_Blue_Component(row, col, first, step, mode_info);

  return (red_component << mode_info->RedFieldPosition) |
         (green_component << mode_info->GreenFieldPosition) |
         (blue_component << mode_info->BlueFieldPosition);
}

void draw_xpm(xpm_image_t img, uint16_t x, uint16_t y) {

  int byte_index = 0;

  uint32_t *bytes = (uint32_t *)img.bytes;

  for (uint16_t yi = y; yi < y + img.height; yi++) {

    for (uint16_t xi = x; xi < x + img.width; xi++) {

      vg_draw_pixel(xi, yi, bytes[byte_index]);

      byte_index++;
    }
  }
}

void clean_xpm(xpm_image_t img, uint16_t x, uint16_t y) {

  Terminix *terminix = get_current_terminix();
  uint32_t *bytes = (uint32_t *) terminix->background_xpm.bytes;

  

  int byte_index = 0;
  for (uint16_t yi = y; yi < y + img.height; yi++) {

    for (uint16_t xi = x; xi < x + img.width; xi++) {
      if(*(((uint32_t*)(img.bytes))+byte_index) != TRANSPARENCY_COLOR_8_8_8_8)

        
        vg_draw_pixel(xi, yi, bytes[(yi*(terminix->background_xpm.width) +xi)]);
        byte_index++;
    }
  }
}

void convert_xpm_img(xpm_map_t xpm, xpm_image_t *img) {
  enum xpm_image_type type = XPM_8_8_8_8;
  xpm_load(xpm, type, img);
}



void update_buffer() {
  memcpy(video_mem, buffer, h_res * v_res * bytes_per_pixel);
}
