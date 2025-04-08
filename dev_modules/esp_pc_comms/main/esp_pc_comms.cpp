#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void example_task(void *arg){
  while(true){
    printf("helloworld\n");
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

extern "C" void app_main(void)
{
  xTaskCreatePinnedToCore(example_task, "Example task", 4096, NULL, 0, NULL, tskNO_AFFINITY);
  return;
}
