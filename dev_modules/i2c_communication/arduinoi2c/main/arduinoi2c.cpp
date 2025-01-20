#include <stdio.h>
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_MASTER_SCL_IO 22         
#define I2C_MASTER_SDA_IO 21  
#define I2C_MASTER_NUM I2C_NUM_0     
#define I2C_MASTER_FREQ_HZ 100000    
#define I2C_MASTER_TX_BUF_DISABLE 0  
#define I2C_MASTER_RX_BUF_DISABLE 0  
#define SLAVE_ADDR 0x58              

extern "C" void app_main(void)
{
  i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master = {
      .clk_speed = 100000,
    },
    .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
  };
    
  i2c_param_config(I2C_MASTER_NUM, &conf);
  i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
  
  uint8_t buf[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
  while (true) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SLAVE_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, buf, 5, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, pdMS_TO_TICKS(1000));
    i2c_cmd_link_delete(cmd);
    vTaskDelay(100/ portTICK_PERIOD_MS); // 500 ms delay
    }
}

