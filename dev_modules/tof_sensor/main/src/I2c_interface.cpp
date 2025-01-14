#include "../include/I2c_interface.h"

void I2c_sensor_interface::init(i2c_master_bus_handle_t *bus_handler, uint8_t addr, uint32_t clk_freq){
  i2c_device_config_t i2c_device_config = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = addr,
    .scl_speed_hz = clk_freq,
    .flags = {
      .disable_ack_check = false
    }
  };
  i2c_master_bus_add_device(*bus_handler, &i2c_device_config, &this->device_handler);
}

void I2c_sensor_interface::read_reg_single(uint8_t reg, uint8_t *data_out){
  i2c_master_transmit_receive(this->device_handler, &reg, 1, data_out, 1, -1);
}

void I2c_sensor_interface::read_reg_multi(uint8_t start_reg, uint8_t *data_out, uint8_t len){
  i2c_master_transmit_receive(this->device_handler, &start_reg, 1, data_out, len, -1);
}

void I2c_sensor_interface::write_reg_single(uint8_t reg, uint8_t data){
  uint8_t tx_buf[2] = {reg, data};
  ESP_ERROR_CHECK(i2c_master_transmit(this->device_handler, tx_buf, 2, -1));
}

void I2c_sensor_interface::write_reg_multi(uint8_t reg, uint8_t *data_in, uint8_t len){
  uint8_t tx_buf[MAX_I2C_READ_LEN] = {reg};
  for(int i = 0; i < len; i++)
    tx_buf[i+1] = data_in[i];
  ESP_ERROR_CHECK(i2c_master_transmit(this->device_handler, tx_buf, len + 1, -1));
}
