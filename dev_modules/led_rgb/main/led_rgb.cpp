#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "include/ws2812b.h"

extern "C" void app_main(void)
{
  Ws2812b led(GPIO_NUM_48);
  led.write(0, 0, 0);
  while(true){
    printf("Helloworld\n");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
