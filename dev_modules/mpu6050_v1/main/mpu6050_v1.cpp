#include <stdio.h>
#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "include/Gy87_lib.h"

extern "C" void app_main(void)
{
  i2c_master_bus_config_t i2c_bus_config = {
    .i2c_port = I2C_NUM_0,
    .sda_io_num = GPIO_NUM_8,
    .scl_io_num = GPIO_NUM_18,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
    .intr_priority = 0,
    .trans_queue_depth = 64,
    .flags = {
      .enable_internal_pullup = true
    }
  };
  i2c_master_bus_handle_t i2c_bus_handler;
  i2c_new_master_bus(&i2c_bus_config, &i2c_bus_handler);


  Gy87 imu(true, true, false);
  imu.init_i2c(&i2c_bus_handler, 100000);

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

  // i2c_device_config_t i2c_device_config = {
  //   .dev_addr_length = I2C_ADDR_BIT_LEN_7,
  //   .device_address = 0x68,
  //   .scl_speed_hz = 100000,
  //   .flags = {
  //     .disable_ack_check = false
  //   }
  // };
  // i2c_master_dev_handle_t device_handler;
  // i2c_master_bus_add_device(i2c_bus_handler, &i2c_device_config, &device_handler);

  // Gy87 imu(true, true, false);
  // imu.init_i2c(&device_handler);
