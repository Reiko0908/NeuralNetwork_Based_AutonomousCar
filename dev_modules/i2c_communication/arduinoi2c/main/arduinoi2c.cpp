#include <stdio.h>
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_SDA_IO GPIO_NUM_1
#define I2C_MASTER_SCL_IO GPIO_NUM_2
#define TEST_I2C_PORT GPIO_NUM_0


extern "C" void app_main(void)
{
  i2c_master_bus_config_t i2c_mst_config = {
    .i2c_port = TEST_I2C_PORT,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
    .intr_priority = 0,
    .trans_queue_depth = 64,
    .flags = {
      .enable_internal_pullup = true
    },
};

i2c_master_bus_handle_t bus_handle;
ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = 0x58, //9 is the number of the address of the slave device (for ex: wire.begin(9) in arduino)
    .scl_speed_hz = 100000,
    .scl_wait_us = 0,
    .flags = {
      .disable_ack_check = false
    }
};


i2c_master_dev_handle_t dev_handle;
i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle);

while(true){
  //lenh truyen
  uint8_t buf[5] = {0x01,0x02, 0x03, 0x04, 0x05};
  i2c_master_transmit(dev_handle, buf, 5, -1);
  vTaskDelay(500/portTICK_PERIOD_MS);
}

}
