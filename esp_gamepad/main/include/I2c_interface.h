#ifndef ESP_IDF_I2C_INTERFACE_H
#define ESP_IDF_I2C_INTERFACE_H

void sensor_read_reg_single(uint8_t addr, uint8_t reg, uint8_t *data_out);
void sensor_read_reg_multi(uint8_t addr, uint8_t start_reg, uint8_t *data_out, uint8_t len);
void sensor_write_reg_single(uint8_t addr, uint8_t reg, uint8_t data);

#endif
