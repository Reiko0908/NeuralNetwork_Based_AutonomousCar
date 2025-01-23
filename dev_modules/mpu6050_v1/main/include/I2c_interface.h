#ifndef ESP_IDF_I2C_INTERFACE_H
#define ESP_IDF_I2C_INTERFACE_H

#include "driver/i2c.h"

class I2c_sensor_interface{
  public:
    uint8_t address;
    i2c_port_t i2c_port;

  public:
    void init(uint8_t addr, i2c_port_t port);
    void read_reg_single(uint8_t reg, uint8_t *data_out);
    void read_reg_multi(uint8_t start_reg, uint8_t *data_out, uint8_t len);
    void write_reg_single(uint8_t reg, uint8_t data);
    void write_reg_multi(uint8_t start_reg, uint8_t *data_in, uint8_t len);
};

#endif
