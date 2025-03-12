#include <stdio.h>
#include "display/lv_display.h"
#include "driver/spi_master.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/task.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_panel_st7789.h"
#include "esp_lcd_types.h"
#include "esp_lcd_panel_ops.h"
#include "esp_heap_caps.h"
#include "lvgl.h"
#include "tick/lv_tick.h"

//=========================
// * DEFINES
//=========================
#define LV_USE_ST7789 1
#define BYTES_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565)) 
#define ST7789_SCK GPIO_NUM_36 
#define ST7789_MOSI GPIO_NUM_35 
#define ST7789_RST GPIO_NUM_38 
#define ST7789_DC GPIO_NUM_40 
#define ST7789_H_RES 240 
#define ST7789_V_RES 240 
#define ST7789_FREQ (27 * 1000 * 1000) 
#define ST7789_CMD_BITS 8 
#define ST7789_PARAM_BITS 8 
#define ST7789_SPI_HOST SPI2_HOST 

//========================= 
// * GLOBAL VARS 
//========================= 
esp_lcd_panel_handle_t panel_handle;

//========================= 
// * GLOBAL FUNCS 
//========================= 

void display_init(){ 
  spi_bus_config_t bus_config = { 
    .mosi_io_num = ST7789_MOSI, 
    .miso_io_num = -1,
    .sclk_io_num = ST7789_SCK,
    .quadwp_io_num = -1, 
    .quadhd_io_num = -1, 
  };
  ESP_ERROR_CHECK(spi_bus_initialize(ST7789_SPI_HOST, &bus_config, SPI_DMA_CH_AUTO));
  printf("SPI Bus initialized\n");

  esp_lcd_panel_io_handle_t io_handle = NULL;
  esp_lcd_panel_io_spi_config_t io_config = { .cs_gpio_num = -1, .dc_gpio_num = ST7789_DC, .spi_mode = 0, .pclk_hz = ST7789_FREQ, .trans_queue_depth = 10, .lcd_cmd_bits = ST7789_CMD_BITS, .lcd_param_bits = ST7789_PARAM_BITS, };
  ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)ST7789_SPI_HOST, &io_config, &io_handle));
  printf("Panel IO initialized\n");

  esp_lcd_panel_dev_config_t panel_config = {
    .reset_gpio_num = ST7789_RST,
    .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
    .bits_per_pixel= BYTES_PER_PIXEL * 8,
    .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,
  };
  ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
  printf("Panel initialized\n");
  ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
  ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, false));
  ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, false, false));
  ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
  printf("Display initialized and turned ON\n");
}

void display_flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *px_map){
  esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, (void*)px_map);
  printf("Flushing display\n");
  lv_display_flush_ready(display);
}

void display_lvgl_init(){
  lv_display_t *display = lv_display_create(ST7789_H_RES, ST7789_V_RES);
  lv_display_set_flush_cb(display, display_flush_cb);
  static uint8_t buf[ST7789_H_RES * ST7789_H_RES * BYTES_PER_PIXEL / 10];
  // static uint8_t *buf = (uint8_t*)heap_caps_malloc(ST7789_H_RES * ST7789_H_RES * BYTES_PER_PIXEL / 10, MALLOC_CAP_DMA);
  lv_display_set_buffers(display, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
}

//=========================
// * TASKS
//=========================
void st7789_task(void *args){
  lv_init();
  lv_tick_set_cb([]() -> uint32_t {
    return esp_timer_get_time() / 1000;
  });
  display_init();
  display_lvgl_init();
  printf("hello %d\n", BYTES_PER_PIXEL);
  while(true){
    lv_timer_handler();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

//=========================
// * MAIN FUNC
//=========================

extern "C" void app_main(void){
  xTaskCreatePinnedToCore(st7789_task, "st7789_task", 4096, NULL, 0, NULL, 0);
}
