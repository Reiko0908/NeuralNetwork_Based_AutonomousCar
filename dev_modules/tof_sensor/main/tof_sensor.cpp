#include <stdio.h>
#include "Vl53l0x.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define FILTER_SIZE 5
extern "C" void app_main(void)
{
  i2c_config_t i2c_config = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = GPIO_NUM_5,
    .scl_io_num = GPIO_NUM_4,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master = {
      .clk_speed = 100000
    },
    .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
  };

  i2c_param_config(I2C_NUM_0, &i2c_config);
  i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

  Vl53l0x tof;
  bool ret = tof.init(I2C_NUM_0);
  printf("init res: %d\n", ret);


  // lower the return signal rate limit (default is 0.25 MCPS)
  tof.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  tof.setVcselPulsePeriod(Vl53l0x::VcselPeriodPreRange, 18);
  tof.setVcselPulsePeriod(Vl53l0x::VcselPeriodFinalRange, 14);

  tof.setMeasurementTimingBudget(10000);
  uint16_t buffer[FILTER_SIZE];
  int index = 0;

  while (true) {
    buffer[index] = tof.readRangeSingleMillimeters();
    index = (index + 1) % FILTER_SIZE;

    // Calculate average
    uint32_t sum = 0;
    for (int i = 0; i < FILTER_SIZE; i++)
      sum += buffer[i];
    uint16_t filteredData = sum / FILTER_SIZE;

    if (tof.timeoutOccurred()) printf("TIMEOUT\n");
    else printf("Filtered Distance: %d\n", filteredData);

    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

// extern "C" void app_main(void)
// {
//   i2c_config_t i2c_config = {
//     .mode = I2C_MODE_MASTER,
//     .sda_io_num = GPIO_NUM_18,
//     .scl_io_num = GPIO_NUM_8,
//     .sda_pullup_en = GPIO_PULLUP_ENABLE,
//     .scl_pullup_en = GPIO_PULLUP_ENABLE,
//     .master ={
//       .clk_speed = 100000
//     }
//   };
//   i2c_param_config(I2C_NUM_0, &i2c_config);
//   i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
//
//   uint8_t write_buffer = 0xC1;
//   uint8_t read_buffer;
//
//   // Add a delay before the first read
//   vTaskDelay(1000 / portTICK_PERIOD_MS);
//
//   while(true){
//     i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, (0x29 << 1) | I2C_MASTER_WRITE, true);
//     i2c_master_write_byte(cmd, write_buffer, true);
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, (0x29 << 1) | I2C_MASTER_READ, true);
//     i2c_master_read_byte(cmd, &read_buffer, I2C_MASTER_LAST_NACK);
//     i2c_master_stop(cmd);
//     i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
//     i2c_cmd_link_delete(cmd);
//
//     printf("%d\n", read_buffer);
//     vTaskDelay(200 / portTICK_PERIOD_MS);
//   }
// }
