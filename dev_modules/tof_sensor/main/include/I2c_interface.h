#ifndef ESP_IDF_I2C_INTERFACE_H
#define ESP_IDF_I2C_INTERFACE_H

#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include <cstdint>

#define MAX_I2C_READ_LEN 64

class I2c_sensor_interface{
  public:
    i2c_master_dev_handle_t device_handler;

  public:
    void init(i2c_master_bus_handle_t *bus_handler, uint8_t addr, uint32_t clk_freq);
    void read_reg_single(uint8_t reg, uint8_t *data_out);
    void read_reg_multi(uint8_t start_reg, uint8_t *data_out, uint8_t len);
    void write_reg_single(uint8_t reg, uint8_t data);
    void write_reg_multi(uint8_t reg, uint8_t *data_in, uint8_t len);
};

#endif
