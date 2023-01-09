
#include <Adafruit_GFX.h>
#include "GFX_ESP32Panel.h"

// only works with ESP32-S3

#include <sys/cdefs.h>
#include "esp_pm.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_private/gdma.h"

#include "hal/dma_types.h"

#include "hal/lcd_hal.h"
#include "hal/lcd_ll.h"
#include "hal/gdma_ll.h"
#include "rom/cache.h"


// ===== EXTRAS from https://github.com/espressif/esp-idf/blob/master/components/esp_lcd/src/esp_lcd_panel_rgb.c

// <https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/lcd.html>

// This function is located in ROM (also see esp_rom/${target}/ld/${target}.rom.ld)
// extern int Cache_WriteBack_Addr(uint32_t addr, uint32_t size);

// extract from esp-idf esp_lcd_rgb_panel.c
// will be replaced in version SDK 5.0.0

struct esp_rgb_panel_t {
  esp_lcd_panel_t base;                                         // Base class of generic lcd panel
  int panel_id;                                                 // LCD panel ID
  lcd_hal_context_t hal;                                        // Hal layer object
  size_t data_width;                                            // Number of data lines (e.g. for RGB565, the data width is 16)
  size_t sram_trans_align;                                      // Alignment for framebuffer that allocated in SRAM
  size_t psram_trans_align;                                     // Alignment for framebuffer that allocated in PSRAM
  int disp_gpio_num;                                            // Display control GPIO, which is used to perform action like "disp_off"
  intr_handle_t intr;                                           // LCD peripheral interrupt handle
  esp_pm_lock_handle_t pm_lock;                                 // Power management lock
  size_t num_dma_nodes;                                         // Number of DMA descriptors that used to carry the frame buffer
  uint8_t *fb;                                                  // Frame buffer
  size_t fb_size;                                               // Size of frame buffer
  int data_gpio_nums[SOC_LCD_RGB_DATA_WIDTH];                   // GPIOs used for data lines, we keep these GPIOs for action like "invert_color"
  size_t resolution_hz;                                         // Peripheral clock resolution
  esp_lcd_rgb_timing_t timings;                                 // RGB timing parameters (e.g. pclk, sync pulse, porch width)
  gdma_channel_handle_t dma_chan;                               // DMA channel handle
  esp_lcd_rgb_panel_frame_trans_done_cb_t on_frame_trans_done;  // Callback, invoked after frame trans done
  void *user_ctx;                                               // Reserved user's data of callback functions
  int x_gap;                                                    // Extra gap in x coordinate, it's used when calculate the flush window
  int y_gap;                                                    // Extra gap in y coordinate, it's used when calculate the flush window
  struct
  {
    unsigned int disp_en_level : 1;  // The level which can turn on the screen by `disp_gpio_num`
    unsigned int stream_mode : 1;    // If set, the LCD transfers data continuously, otherwise, it stops refreshing the LCD when transaction done
    unsigned int fb_in_psram : 1;    // Whether the frame buffer is in PSRAM
  } flags;
  dma_descriptor_t dma_nodes[];  // DMA descriptor pool of size `num_dma_nodes`
};

extern void lcd_rgb_panel_start_transmission(esp_rgb_panel_t *rgb_panel);

// =====

// much from:
// <https://github.com/espressif/esp-idf/blob/master/examples/peripherals/lcd/rgb_panel/main/rgb_lcd_example_main.c>
// setup panel configuration see panel manual and
// <https://github.com/espressif/esp-idf/blob/master/components/esp_lcd/include/esp_lcd_panel_rgb.h>

// docu:
// <https://docs.espressif.com/projects/esp-idf/en/v5.0/esp32s3/api-reference/peripherals/lcd.html>

esp_lcd_rgb_panel_config_t panelConfig = {
  .clk_src = LCD_CLK_SRC_PLL160M,
  .timings = {
    .pclk_hz = 16000000,
    .h_res = 800,
    .v_res = 480,
    // The following parameters should refer to LCD spec
    .hsync_pulse_width = 4,
    .hsync_back_porch = 8,
    .hsync_front_porch = 8,
    .vsync_pulse_width = 4,
    .vsync_back_porch = 8,
    .vsync_front_porch = 8,
    .flags = {
      .hsync_idle_low = 1,
      .vsync_idle_low = 1,
      .de_idle_high = 0,
      .pclk_active_neg = 1,
      .pclk_idle_high = 0,
    },
  },

  .data_width = 16,  // 16 bit using RGB 565 mode.
  .sram_trans_align = 0,
  .psram_trans_align = 64,
  // .num_fbs = 1,
  // .refresh_on_demand
  .hsync_gpio_num = 39,
  .vsync_gpio_num = 41,
  .de_gpio_num = 40,
  .pclk_gpio_num = 42,
  .data_gpio_nums = { 8, 3, 46, 9, 1, 5, 6, 7, 15, 16, 4, 45, 48, 47, 21, 14 },
  .disp_gpio_num = -1,

  .on_frame_trans_done = nullptr,
  .user_ctx = nullptr,
  .flags = {
    .disp_active_low = 0,
    .relax_on_idle = 0, /// => lcd_rgb_panel_start_transmission2
    .fb_in_psram = 1,
  }
};

esp_lcd_panel_handle_t panelHandle = nullptr;
esp_rgb_panel_t *_rgbPanel = nullptr;

/**************************************************************************/
/*!
   @brief    Instantiate a GFX 16-bit canvas context for graphics
   @param    w   Display width, in pixels
   @param    h   Display height, in pixels
*/
/**************************************************************************/
GFX_ESP32Panel::GFX_ESP32Panel(uint16_t w, uint16_t h)
  : Adafruit_GFX(w, h) {
  bufferSize = w * h * 2;
  buffer = nullptr;

  // initialize DMA here...
  panelConfig.timings.h_res = w;
  panelConfig.timings.v_res = h;
  esp_err_t err = esp_lcd_new_rgb_panel(&panelConfig, &panelHandle);
  if (err == ESP_OK) {
    esp_lcd_panel_reset(panelHandle);
    esp_lcd_panel_init(panelHandle);

    // get access to the Panel Image buffer
    _rgbPanel = __containerof(panelHandle, esp_rgb_panel_t, base);
    buffer = (uint16_t *)(_rgbPanel->fb);
  }

  if (buffer) {
    memset(buffer, 0, bufferSize);
  }
}

/**************************************************************************/
/*!
   @brief Stop using the Canvas.
*/
/**************************************************************************/
GFX_ESP32Panel::~GFX_ESP32Panel(void) {
  esp_lcd_panel_del(panelHandle);
  // no free(buffer);
}

// from https://github.com/espressif/esp-idf/blob/release/v4.4/components/esp_lcd/src/esp_lcd_rgb_panel.c

// static void lcd_rgb_panel_start_transmission2(esp_rgb_panel_t *rgb_panel)
// {
//     // reset FIFO of DMA and LCD, incase there remains old frame data
//     gdma_reset(rgb_panel->dma_chan);
//     lcd_ll_stop(rgb_panel->hal.dev);
//     lcd_ll_fifo_reset(rgb_panel->hal.dev);
//     gdma_start(rgb_panel->dma_chan, (intptr_t)rgb_panel->dma_nodes);
//     // delay 1us is sufficient for DMA to pass data to LCD FIFO
//     // in fact, this is only needed when LCD pixel clock is set too high
//     esp_rom_delay_us(1);
//     // start LCD engine
//     lcd_ll_start(rgb_panel->hal.dev);
// }

void GFX_ESP32Panel::startWrite(){};
void GFX_ESP32Panel::endWrite() {
  if (buffer) {
    Cache_WriteBack_Addr((uint32_t)buffer, bufferSize);
    // esp_lcd_rgb_panel_refresh(panelHandle);
    // lcd_rgb_panel_start_transmission2(_rgbPanel);
  }
};


/**************************************************************************/
/*!
    @brief  Draw a pixel to the canvas framebuffer
    @param  x   x coordinate
    @param  y   y coordinate
    @param  color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void GFX_ESP32Panel::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (buffer) {
    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
      return;

    uint16_t *b = buffer;
    switch (rotation) {
      case 0:
        b += x + y * WIDTH;
        break;
      case 1:
        b += (WIDTH - 1 - y) + x * WIDTH;
        break;
      case 2:
        b += (WIDTH - 1 - x) + (HEIGHT - 1 - y) * WIDTH;
        break;
      case 3:
        b += y + (HEIGHT - 1 - x) * WIDTH;
        break;
    }
    *b = color;
  }
}

/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given coordinate
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's 16-bit 5-6-5 color value
*/
/**********************************************************************/
uint16_t GFX_ESP32Panel::getPixel(int16_t x, int16_t y) const {
  int16_t t;
  switch (rotation) {
    case 1:
      t = x;
      x = WIDTH - 1 - y;
      y = t;
      break;
    case 2:
      x = WIDTH - 1 - x;
      y = HEIGHT - 1 - y;
      break;
    case 3:
      t = x;
      x = y;
      y = HEIGHT - 1 - t;
      break;
  }
  return getRawPixel(x, y);
}

/**********************************************************************/
/*!
        @brief    Get the pixel color value at a given, un-rotated coordinate.
              This method is intended for hardware drivers to get pixel value
              in physical coordinates.
        @param    x   x coordinate
        @param    y   y coordinate
        @returns  The desired pixel's 16-bit 5-6-5 color value
*/
/**********************************************************************/
uint16_t GFX_ESP32Panel::getRawPixel(int16_t x, int16_t y) const {
  if (buffer) {
    if ((x >= 0) || (y >= 0) || (x < WIDTH) || (y < HEIGHT))
      return buffer[x + y * WIDTH];
  }
  return 0;
}

/**************************************************************************/
/*!
    @brief  Fill the framebuffer completely with one color
    @param  color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void GFX_ESP32Panel::fillScreen(uint16_t color) {
  if (buffer) {
    uint8_t hi = color >> 8, lo = color & 0xFF;
    if (hi == lo) {
      memset(buffer, lo, WIDTH * HEIGHT * 2);
    } else {
      uint32_t i, pixels = WIDTH * HEIGHT;
      for (i = 0; i < pixels; i++)
        buffer[i] = color;
    }
  }
}

/**************************************************************************/
/*!
    @brief  Reverses the "endian-ness" of each 16-bit pixel within the
            canvas; little-endian to big-endian, or big-endian to little.
            Most microcontrollers (such as SAMD) are little-endian, while
            most displays tend toward big-endianness. All the drawing
            functions (including RGB bitmap drawing) take care of this
            automatically, but some specialized code (usually involving
            DMA) can benefit from having pixel data already in the
            display-native order. Note that this does NOT convert to a
            SPECIFIC endian-ness, it just flips the bytes within each word.
*/
/**************************************************************************/
void GFX_ESP32Panel::byteSwap(void) {
  if (buffer) {
    uint32_t i, pixels = WIDTH * HEIGHT;
    for (i = 0; i < pixels; i++)
      buffer[i] = __builtin_bswap16(buffer[i]);
  }
}

/**************************************************************************/
/*!
   @brief    Speed optimized vertical line drawing
   @param    x   Line horizontal start point
   @param    y   Line vertical start point
   @param    h   length of vertical line to be drawn, including first point
   @param    color   color 16-bit 5-6-5 Color to draw line with
*/
/**************************************************************************/
void GFX_ESP32Panel::drawFastVLine(int16_t x, int16_t y, int16_t h,
                                   uint16_t color) {
  if (h < 0) {  // Convert negative heights to positive equivalent
    h *= -1;
    y -= h - 1;
    if (y < 0) {
      h += y;
      y = 0;
    }
  }

  // Edge rejection (no-draw if totally off canvas)
  if ((x < 0) || (x >= width()) || (y >= height()) || ((y + h - 1) < 0)) {
    return;
  }

  if (y < 0) {  // Clip top
    h += y;
    y = 0;
  }
  if (y + h > height()) {  // Clip bottom
    h = height() - y;
  }

  if (getRotation() == 0) {
    drawFastRawVLine(x, y, h, color);
  } else if (getRotation() == 1) {
    int16_t t = x;
    x = WIDTH - 1 - y;
    y = t;
    x -= h - 1;
    drawFastRawHLine(x, y, h, color);
  } else if (getRotation() == 2) {
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;

    y -= h - 1;
    drawFastRawVLine(x, y, h, color);
  } else if (getRotation() == 3) {
    int16_t t = x;
    x = y;
    y = HEIGHT - 1 - t;
    drawFastRawHLine(x, y, h, color);
  }
}

/**************************************************************************/
/*!
   @brief  Speed optimized horizontal line drawing
   @param  x      Line horizontal start point
   @param  y      Line vertical start point
   @param  w      Length of horizontal line to be drawn, including 1st point
   @param  color  Color 16-bit 5-6-5 Color to draw line with
*/
/**************************************************************************/
void GFX_ESP32Panel::drawFastHLine(int16_t x, int16_t y, int16_t w,
                                   uint16_t color) {
  if (w < 0) {  // Convert negative widths to positive equivalent
    w *= -1;
    x -= w - 1;
    if (x < 0) {
      w += x;
      x = 0;
    }
  }

  // Edge rejection (no-draw if totally off canvas)
  if ((y < 0) || (y >= height()) || (x >= width()) || ((x + w - 1) < 0)) {
    return;
  }

  if (x < 0) {  // Clip left
    w += x;
    x = 0;
  }
  if (x + w >= width()) {  // Clip right
    w = width() - x;
  }

  if (getRotation() == 0) {
    drawFastRawHLine(x, y, w, color);
  } else if (getRotation() == 1) {
    int16_t t = x;
    x = WIDTH - 1 - y;
    y = t;
    drawFastRawVLine(x, y, w, color);
  } else if (getRotation() == 2) {
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;

    x -= w - 1;
    drawFastRawHLine(x, y, w, color);
  } else if (getRotation() == 3) {
    int16_t t = x;
    x = y;
    y = HEIGHT - 1 - t;
    y -= w - 1;
    drawFastRawVLine(x, y, w, color);
  }
}

/**************************************************************************/
/*!
   @brief    Speed optimized vertical line drawing into the raw canvas buffer
   @param    x   Line horizontal start point
   @param    y   Line vertical start point
   @param    h   length of vertical line to be drawn, including first point
   @param    color   color 16-bit 5-6-5 Color to draw line with
*/
/**************************************************************************/
void GFX_ESP32Panel::drawFastRawVLine(int16_t x, int16_t y, int16_t h,
                                      uint16_t color) {
  // x & y already in raw (rotation 0) coordinates, no need to transform.
  uint16_t *buffer_ptr = buffer + y * WIDTH + x;
  for (int16_t i = 0; i < h; i++) {
    (*buffer_ptr) = color;
    buffer_ptr += WIDTH;
  }
}

/**************************************************************************/
/*!
   @brief    Speed optimized horizontal line drawing into the raw canvas buffer
   @param    x   Line horizontal start point
   @param    y   Line vertical start point
   @param    w   length of horizontal line to be drawn, including first point
   @param    color   color 16-bit 5-6-5 Color to draw line with
*/
/**************************************************************************/
void GFX_ESP32Panel::drawFastRawHLine(int16_t x, int16_t y, int16_t w,
                                      uint16_t color) {
  // x & y already in raw (rotation 0) coordinates, no need to transform.
  uint32_t buffer_index = y * WIDTH + x;
  for (uint32_t i = buffer_index; i < buffer_index + w; i++) {
    buffer[i] = color;
  }
}
