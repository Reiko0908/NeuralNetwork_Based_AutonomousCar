#include <stdio.h>
#include <cstring>
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_partition.h"

#include "include/Gy87_lib.h"

extern "C" void app_main(void)
{
  i2c_config_t i2c_bus_config = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = GPIO_NUM_18,
    .scl_io_num = GPIO_NUM_8,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master = {
      .clk_speed = 100000
    },
    .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
  };
  i2c_param_config(I2C_NUM_0, &i2c_bus_config);
  i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

  const esp_partition_t* partition = esp_partition_find_first(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, "calib"); 

  Gy87 imu(true, true, false);

  imu.init_i2c(I2C_NUM_0);
  imu.init_flash(partition);
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
