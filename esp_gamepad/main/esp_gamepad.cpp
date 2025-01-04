#include <stdio.h>
#include "driver/gpio.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// #include "include/ws2812b.h"

extern "C" void app_main(void){
  Ws2812b default_rgb(GPIO_NUM_48);
  default_rgb.write(100, 50, 20);

  while(true){
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
