#include <stdio.h>

#include "driver/ledc.h"
#include "hal/ledc_types.h"

extern "C" void app_main(void){
  ledc_timer_config_t ledc_config = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = LEDC_TIMER_12_BIT,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = 500,
    .clk_cfg = LEDC_USE_APB_CLK,
  };
  ledc_timer_config(&ledc_config);
  
  ledc_channel_config_t channel_config = {
    .gpio_num = GPIO_NUM_10,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .timer_sel = LEDC_TIMER_0,
    .duty = false,
    .hpoint = 0,
    .flags = {
      .output_invert = false
    }
  };
  ledc_channel_config(&channel_config);

  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 4096/2);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}
