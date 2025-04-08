#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/task.h"

#include "driver/ledc.h"
#include "hal/ledc_types.h"

float duty_left = 0.1;
float duty_right = 0.5;

void pwm_task(void *arg){
  ledc_timer_config_t ledc_config = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_12_BIT,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 500,
    .clk_cfg = LEDC_USE_APB_CLK,
  };
  ledc_timer_config(&ledc_config);
  
  ledc_channel_config_t channel_left_config = {
    .gpio_num = GPIO_NUM_4,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .timer_sel = LEDC_TIMER_0,
    .duty = false,
    .hpoint = 0,
    .flags = {
      .output_invert = false
    }
  };
  ledc_channel_config(&channel_left_config);

  ledc_channel_config_t channel_right_config = {
    .gpio_num = GPIO_NUM_5,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_1,
    .timer_sel = LEDC_TIMER_0,
    .duty = false,
    .hpoint = 0,
    .flags = {
      .output_invert = false
    }
  };
  ledc_channel_config(&channel_right_config);

  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 4096 * 0.1);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, 4096 * 0.5);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
}

extern "C" void app_main(void){
  xTaskCreatePinnedToCore(
    pwm_task,
    "pwm_task",
    4096,
    NULL,
    0,
    NULL,
    tskNO_AFFINITY);
}
