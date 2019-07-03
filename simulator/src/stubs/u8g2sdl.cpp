#include "SDL2/SDL.h"
#include "u8g2sdl.h"
#include "u8g2.h"
#include "simulator_context.h"

static void u8g_sdl_set_pixel(int x, int y, bool color)
{
    if(color) {
        SDL_SetRenderDrawColor(thea::simulator_context::context.renderer, 127, 127, 255, SDL_ALPHA_OPAQUE);
    } else {
        SDL_SetRenderDrawColor(thea::simulator_context::context.renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    }
    
    SDL_RenderDrawPoint(thea::simulator_context::context.renderer, x, y);
}

static void u8g_sdl_set_8pixel(int x, int y, uint8_t pixel)
{
  int cnt = 8;
  int bg = 0;
  // if ( (x/8 + y/8)  & 1 )
  //   bg = 4;
  while( cnt > 0 )
  {
    if ( (pixel & 1) == 0 )
    {
      u8g_sdl_set_pixel(x,y,bg);
    }
    else
    {
      u8g_sdl_set_pixel(x,y,3);
    }
    pixel >>= 1;
    y++;
    cnt--;
  }
}

static void u8g_sdl_set_multiple_8pixel(int x, int y, int cnt, uint8_t *pixel)
{
  uint8_t b;
  while( cnt > 0 )
  {
    b = *pixel;
    u8g_sdl_set_8pixel(x, y, b);
    x++;
    pixel++;
    cnt--;
  }
}

static const u8x8_display_info_t u8x8_sdl_128x64_info =
{
  /* chip_enable_level = */ 0,
  /* chip_disable_level = */ 1,
  
  /* post_chip_enable_wait_ns = */ 0,
  /* pre_chip_disable_wait_ns = */ 0,
  /* reset_pulse_width_ms = */ 0, 
  /* post_reset_wait_ms = */ 0, 
  /* sda_setup_time_ns = */ 0,      
  /* sck_pulse_width_ns = */ 0,
  /* sck_clock_hz = */ 4000000UL,   /* since Arduino 1.6.0, the SPI bus speed in Hz. Should be  1000000000/sck_pulse_width_ns */
  /* spi_mode = */ 1,       
  /* i2c_bus_clock_100kHz = */ 0,
  /* data_setup_time_ns = */ 0,
  /* write_pulse_width_ns = */ 0,
  /* tile_width = */ 16,
  /* tile_hight = */ 8,
  /* default_x_offset = */ 0,
  /* flipmode_x_offset = */ 0,
  /* pixel_width = */ 128,
  /* pixel_height = */ 64
};

uint8_t u8x8_d_sdl_128x64(u8x8_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  uint8_t x, y, c;
  uint8_t *ptr;
  switch(msg)
  {
    case U8X8_MSG_DISPLAY_SETUP_MEMORY:
      u8x8_d_helper_display_setup_memory(u8g2, &u8x8_sdl_128x64_info);
      //u8g_sdl_init(128, 64);
      break;
    case U8X8_MSG_DISPLAY_INIT:
      u8x8_d_helper_display_init(u8g2);
      break;
    case U8X8_MSG_DISPLAY_SET_POWER_SAVE:
      break;
    case U8X8_MSG_DISPLAY_SET_FLIP_MODE:
      break;
    case U8X8_MSG_DISPLAY_SET_CONTRAST:
      break;
    case U8X8_MSG_DISPLAY_DRAW_TILE:
      x = ((u8x8_tile_t *)arg_ptr)->x_pos;
      x *= 8;
      x += u8g2->x_offset;
    
      y = ((u8x8_tile_t *)arg_ptr)->y_pos;
      y *= 8;
    
      do
      {
        c = ((u8x8_tile_t *)arg_ptr)->cnt;
        ptr = ((u8x8_tile_t *)arg_ptr)->tile_ptr;
        u8g_sdl_set_multiple_8pixel(x, y, c*8, ptr);
        arg_int--;
        x+=c*8;
      } while( arg_int > 0 );
      
      break;
    default:
      return 0;
  }
  return 1;
}

void u8x8_Setup_SDL_128x64(u8x8_t *u8x8)
{
  /* setup defaults */
  u8x8_SetupDefaults(u8x8);
  
  /* setup specific callbacks */
  u8x8->display_cb = u8x8_d_sdl_128x64;
    
  //u8x8->gpio_and_delay_cb = u8x8_d_sdl_gpio;

  /* setup display info */
  u8x8_SetupMemory(u8x8);  
}

void u8g2_SetupBuffer_SDL_128x64(u8g2_t *u8g2, const u8g2_cb_t *u8g2_cb)
{
  static uint8_t buf[128*8];
  
  u8x8_Setup_SDL_128x64(u8g2_GetU8x8(u8g2));
  u8g2_SetupBuffer(u8g2, buf, 8, u8g2_ll_hvline_vertical_top_lsb, u8g2_cb);
}


U8G2_SDL::U8G2_SDL(const u8g2_cb_t *rotation) : U8G2() {
    u8g2_SetupBuffer_SDL_128x64(&u8g2, rotation);
}

