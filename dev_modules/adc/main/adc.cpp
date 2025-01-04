#include <stdio.h>
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main(void)
{
  adc_oneshot_unit_handle_t adc_handler;
  adc_oneshot_unit_init_cfg_t adc_config = {
    .unit_id = ADC_UNIT_1,
    .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
    .ulp_mode = ADC_ULP_MODE_DISABLE,
  };
  adc_oneshot_new_unit(&adc_config, &adc_handler);

  adc_oneshot_chan_cfg_t channel_config = {
    .atten = ADC_ATTEN_DB_12,
    .bitwidth = ADC_BITWIDTH_12,
  };
  adc_oneshot_config_channel(adc_handler, ADC_CHANNEL_9, &channel_config);

  int adc_raw;
  while(true){
    adc_oneshot_read(adc_handler, ADC_CHANNEL_9, &adc_raw);
    printf("%f5\n", adc_raw / 4095 * 3.3);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
