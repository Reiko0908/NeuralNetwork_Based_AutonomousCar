#include <stdio.h>
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// #include "I2c_interface.h"

extern "C" void app_main(void)
{
  i2c_config_t i2c_bus_config = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = GPIO_NUM_3,
    .scl_io_num = GPIO_NUM_4,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master = {
      .clk_speed = 100000
    },
    .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
  };
  i2c_param_config(I2C_NUM_0, &i2c_bus_config);
  i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
  //
  // I2c_sensor_interface battery_reader;
  // battery_reader.init(0x0B, I2C_NUM_0);
}
