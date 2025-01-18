#ifndef ESP_IDF_I2C_INTERFACE_H
#define ESP_IDF_I2C_INTERFACE_H

#include "driver/i2c.h"

class I2c_sensor_interface{
  public:
    uint8_t address;
    i2c_port_t i2c_port;

  public:
    void init(uint8_t addr, i2c_port_t port){
      this->address = addr;
      this->i2c_port = port;
    }

    void read_reg_single(uint8_t reg, uint8_t *data_out){
      i2c_cmd_handle_t cmd_handler = i2c_cmd_link_create();
      i2c_master_start(cmd_handler);
      i2c_master_write_byte(cmd_handler, (this->address << 1) | I2C_MASTER_WRITE, true);
      i2c_master_write_byte(cmd_handler, reg, true);
      i2c_master_start(cmd_handler);
      i2c_master_write_byte(cmd_handler, (this->address << 1) | I2C_MASTER_READ, true);
      i2c_master_read_byte(cmd_handler, data_out, I2C_MASTER_LAST_NACK);
      i2c_master_stop(cmd_handler);
      i2c_master_cmd_begin(this->i2c_port, cmd_handler, 1000);
      i2c_cmd_link_delete(cmd_handler);
    }

    void read_reg_multi(uint8_t start_reg, uint8_t *data_out, uint8_t len){
      i2c_cmd_handle_t cmd_handler = i2c_cmd_link_create();
      i2c_master_start(cmd_handler);
      i2c_master_write_byte(cmd_handler, (this->address << 1) | I2C_MASTER_WRITE, true);
      i2c_master_write_byte(cmd_handler, start_reg, true);
      i2c_master_start(cmd_handler);
      i2c_master_write_byte(cmd_handler, (this->address << 1) | I2C_MASTER_READ, true);
      i2c_master_read(cmd_handler, data_out, len, I2C_MASTER_LAST_NACK);
      i2c_master_stop(cmd_handler);
      i2c_master_cmd_begin(this->i2c_port, cmd_handler, 1000);
      i2c_cmd_link_delete(cmd_handler);
    }

    void write_reg_single(uint8_t reg, uint8_t data){
      i2c_cmd_handle_t cmd_handler = i2c_cmd_link_create();
      i2c_master_start(cmd_handler);
      i2c_master_write_byte(cmd_handler, (this->address << 1) | I2C_MASTER_WRITE, true);
      i2c_master_write_byte(cmd_handler, reg, true);
      i2c_master_write_byte(cmd_handler, data, true);
      i2c_master_stop(cmd_handler);
      i2c_master_cmd_begin(this->i2c_port, cmd_handler, 1000);
      i2c_cmd_link_delete(cmd_handler);
    }

    void write_reg_multi(uint8_t start_reg, uint8_t *data_in, uint8_t len){
      i2c_cmd_handle_t cmd_handler = i2c_cmd_link_create();
      i2c_master_start(cmd_handler);
      i2c_master_write_byte(cmd_handler, (this->address << 1) | I2C_MASTER_WRITE, true);
      i2c_master_write_byte(cmd_handler, start_reg, true);
      i2c_master_write(cmd_handler, data_in, len, true);
      i2c_master_stop(cmd_handler);
      i2c_master_cmd_begin(this->i2c_port, cmd_handler, 1000);
      i2c_cmd_link_delete(cmd_handler);
    }
};

#endif
