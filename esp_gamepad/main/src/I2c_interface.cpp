// #include "../include/I2c_interface.h"
//
// void sensor_read_reg_single(uint8_t addr, uint8_t reg, uint8_t *data_out){
//   Wire.beginTransmission(addr);
//   Wire.write(reg);
//   Wire.endTransmission(false);
//   Wire.requestFrom(addr, 1, true);
//   *data_out = Wire.read();
// }
//
// void sensor_read_reg_multi(uint8_t addr, uint8_t start_reg, uint8_t *data_out, uint8_t len){
//   Wire.beginTransmission(addr);
//   Wire.write(start_reg);
//   Wire.endTransmission(false);
//   Wire.requestFrom(addr, len, true);
//   for(int i = 0; i < len; i++) data_out[i] = Wire.read();
// }
//
// void sensor_write_reg_single(uint8_t addr, uint8_t reg, uint8_t data){
//   Wire.beginTransmission(addr);
//   Wire.write(reg);
//   Wire.write(data);
//   Wire.endTransmission(true);
// }
