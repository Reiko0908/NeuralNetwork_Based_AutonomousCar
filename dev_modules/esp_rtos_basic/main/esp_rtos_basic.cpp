#include <stdio.h>
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_timer.h"

enum {
  BT_L_LEFT = (int)GPIO_NUM_46,
  BT_L_UP = (int)GPIO_NUM_7,
  BT_L_RIGHT = (int)GPIO_NUM_9,
  BT_L_DOWN = (int)GPIO_NUM_14,
  BT_L_CORNER = (int)GPIO_NUM_8,
  BT_R_LEFT = (int)GPIO_NUM_45,
  BT_R_UP = (int)GPIO_NUM_42,
  BT_R_RIGHT = (int)GPIO_NUM_48,
  BT_R_DOWN = (int)GPIO_NUM_47,
  BT_R_CORNER = (int)GPIO_NUM_2,
  BT_C_UP = (int)GPIO_NUM_3,
  BT_C_DOWN = (int)GPIO_NUM_39,
} button_ios;

#define GPIO_INPUT_PIN_SEL 1ULL << BT_L_LEFT | 1ULL << BT_L_UP | 1ULL << BT_L_RIGHT | 1ULL << BT_L_DOWN | 1ULL << BT_L_CORNER | \
                           1ULL << BT_R_LEFT | 1ULL << BT_R_UP | 1ULL << BT_R_RIGHT | 1ULL << BT_R_DOWN | 1ULL << BT_R_CORNER | \
                           1ULL << BT_C_UP | 1ULL << BT_C_DOWN

#define BUTTON_TIMING_CONSTRAINT_US 1000 // get rid of multiple rise/fall edge in single press/release

static QueueHandle_t gpio_evt_queue = NULL;
uint64_t last_rise_edge_time[49] = {0}, last_fall_edge_time[49] = {0};

void left_adc_task(void *arg){
  adc_oneshot_unit_handle_t x_adc_handler, y_adc_handler;
  adc_oneshot_unit_init_cfg_t
    x_adc_config = {
      .unit_id = ADC_UNIT_1,
      .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
      .ulp_mode = ADC_ULP_MODE_DISABLE,
    },
    y_adc_config = {
      .unit_id = ADC_UNIT_1,
      .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
      .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
  adc_oneshot_new_unit(&x_adc_config, &x_adc_handler);
  adc_oneshot_new_unit(&y_adc_config, &y_adc_handler);

  adc_oneshot_chan_cfg_t channel_config = {
    .atten = ADC_ATTEN_DB_12,
    .bitwidth = ADC_BITWIDTH_12,
  };
  adc_oneshot_config_channel(x_adc_handler, ADC_CHANNEL_4, &channel_config); //GPIO5
  adc_oneshot_config_channel(y_adc_handler, ADC_CHANNEL_5, &channel_config); //GPIO6
  
  int adc_x, adc_y;
  while(true){
    // printf("hi: task 0 on core %d\n", esp_cpu_get_core_id());
    adc_oneshot_read(x_adc_handler, ADC_CHANNEL_4, &adc_x);
    adc_oneshot_read(y_adc_handler, ADC_CHANNEL_5, &adc_y);
    printf("%d %d\n", adc_x, adc_y);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
  vTaskDelete(NULL);
}

static void IRAM_ATTR gpio_isr_handler(void* arg) {
  uint32_t io_num = (uint32_t) arg;
  xQueueSendFromISR(gpio_evt_queue, &io_num, NULL);
}

void button_task(void *arg){
  gpio_config_t io_conf = {
    .pin_bit_mask = GPIO_INPUT_PIN_SEL,
    .mode = GPIO_MODE_INPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_ANYEDGE
  };
  gpio_config(&io_conf);

  gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
  gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);

  gpio_isr_handler_add(GPIO_NUM_7, gpio_isr_handler, (void*)7);
  gpio_isr_handler_add(GPIO_NUM_9, gpio_isr_handler, (void*)9);
  gpio_isr_handler_add(GPIO_NUM_14, gpio_isr_handler, (void*)14);
  gpio_isr_handler_add(GPIO_NUM_46, gpio_isr_handler, (void*)46);
  gpio_isr_handler_add(GPIO_NUM_42, gpio_isr_handler, (void*)42);
  gpio_isr_handler_add(GPIO_NUM_45, gpio_isr_handler, (void*)45);
  gpio_isr_handler_add(GPIO_NUM_48, gpio_isr_handler, (void*)48);
  gpio_isr_handler_add(GPIO_NUM_47, gpio_isr_handler, (void*)47);
  gpio_isr_handler_add(GPIO_NUM_8, gpio_isr_handler, (void*)8);
  gpio_isr_handler_add(GPIO_NUM_3, gpio_isr_handler, (void*)3);
  gpio_isr_handler_add(GPIO_NUM_39, gpio_isr_handler, (void*)39);
  gpio_isr_handler_add(GPIO_NUM_2, gpio_isr_handler, (void*)2);

  gpio_num_t io_num;
  uint64_t *ptr;
  while(true)
    if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)){
      uint64_t current_time = esp_timer_get_time();
      ptr = gpio_get_level(io_num) ? last_rise_edge_time : last_fall_edge_time;
      if(current_time - *(ptr + io_num) > BUTTON_TIMING_CONSTRAINT_US){
        *(ptr + io_num) = current_time;
        printf("GPIO[%d]\n", io_num);
      }
      // if(gpio_get_level(io_num)){ //rise edge
      //   if(current_time - last_rise_edge_time[io_num] > BUTTON_TIMING_CONSTRAINT_US){
      //     last_rise_edge_time[io_num] = current_time;
      //     // event execution
      //     printf("GPIO[%d] intr, val: 1\n", io_num);
      //   }
      // }else{ //fall edge
      //   if(current_time - last_fall_edge_time[io_num] > BUTTON_TIMING_CONSTRAINT_US){
      //     last_fall_edge_time[io_num] = current_time;
      //     // event execution
      //     printf("GPIO[%d] intr, val: 0\n", io_num);
      //   }
      // }
    }
  vTaskDelete(NULL);
}

extern "C" void app_main(void){
  // xTaskCreatePinnedToCore(left_adc_task, "Left ADC task", 4096, NULL, 0, NULL, tskNO_AFFINITY);
  xTaskCreatePinnedToCore(button_task, "Button task", 4096, NULL, 0, NULL, tskNO_AFFINITY);
  return;
}
