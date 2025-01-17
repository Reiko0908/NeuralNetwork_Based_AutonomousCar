#include <stdio.h>
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "include/Gy87_lib.h"

extern "C" void app_main(void)
{
  i2c_config_t i2c_bus_config = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = GPIO_NUM_18,
    .scl_io_num = GPIO_NUM_8,
    .sda_pullup_en = true,
    .scl_pullup_en = true,
    .master = {
      .clk_speed = 100000
    },
    .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
  };
  i2c_param_config(I2C_NUM_0, &i2c_bus_config);
  i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

  Gy87 imu(true, true, false);

  imu.config(
    DLPF_ACC_44HZ_GYRO_42HZ,
    ACC_AFS_SEL_4G,
    GYRO_FS_SEL_250_DEG_PER_SEC,
    MAG_SAMPLES_AVERAGED_1,
    MAG_DATA_OUTPUT_RATE_75_HZ,
    MAG_FIELD_RANGE_1P3_GA,
    MAG_MODE_CONTINUOUS_MEASUREMENT
  );

  while(true){
    imu.update_reading();
    printf("%f5 %f5 %f5\n", imu.gyro.x, imu.gyro.y, imu.gyro.z);
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

// #include <stdio.h>
// #include "driver/i2c.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
//
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
//
//   i2c_param_config(I2C_NUM_0, &i2c_config);
//   i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
//
//   uint8_t write_buffer = 0x75;
//   uint8_t read_buffer;
//
//   // Add a delay before the first read
//   vTaskDelay(1000 / portTICK_PERIOD_MS);
//
//   while(true){
//     i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, (0x68 << 1) | I2C_MASTER_WRITE, true);
//     i2c_master_write_byte(cmd, write_buffer, true);
//     i2c_master_start(cmd);
//     i2c_master_write_byte(cmd, (0x68 << 1) | I2C_MASTER_READ, true);
//     i2c_master_read_byte(cmd, &read_buffer, I2C_MASTER_LAST_NACK);
//     i2c_master_stop(cmd);
//     i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
//     i2c_cmd_link_delete(cmd);
//
//     printf("%d\n", read_buffer);
//     vTaskDelay(200 / portTICK_PERIOD_MS);
//   }
// }
