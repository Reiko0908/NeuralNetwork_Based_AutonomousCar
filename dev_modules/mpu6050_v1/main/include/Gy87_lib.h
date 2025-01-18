#ifndef GY87_LIB_H
#define GY87_LIB_H

#include "Vec3.h"
#include "I2c_interface.h" // for further code reusable, not just on Arduino Platform
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_partition.h"
#include "esp_timer.h"
#include <cstring>

// ----- LIBRARY CONSTANTS ---------------------------------------------------------------------
#define NUM_CALIB_SAMPLES 50
#define MAG_CALIB_DURATION_US 10000000

//----- EEPROM CALIBRATION DATA ADDRESSES ------------------------------------------------------------------
#define EEPROM_GYRO_OFFSETS_START_REG 0x00
#define EEPROM_ACC_OFFSETS_START_REG 0x12
#define EEPROM_MAG_HARD_IRON_START_REG 0x24
#define EEPROM_MAG_SOFT_IRON_START_REG 0x60

// ----- I2C ADDRESSES ------------------------------------------------------------------
#define I2C_MPU6050_ADDR 0x68
#define I2C_HMC5883L_ADDR 0x1E

// ----- SENSOR CONSTANTS ------------------------------------------------------------------
#define GYRO_SENSITIVITY_131_LSB_PER_DEG_PER_SEC 131.0
#define GYRO_SENSITIVITY_65P5_LSB_PER_DEG_PER_SEC 65.5
#define GYRO_SENSITIVITY_32P8_LSB_PER_DEG_PER_SEC 32.8
#define GYRO_SENSITIVITY_16P4_LSB_PER_DEG_PER_SEC 16.4

#define ACC_SENSITIVITY_16384_LSB_PER_G 16384.0
#define ACC_SENSITIVITY_8192_LSB_PER_G 8192.0
#define ACC_SENSITIVITY_4096_LSB_PER_G 4096.0
#define ACC_SENSITIVITY_2048_LSB_PER_G 2048.0

#define MAG_SENSITIVITY_1370_LSB_PER_GAUSS 1370.0
#define MAG_SENSITIVITY_1090_LSB_PER_GAUSS 1090.0
#define MAG_SENSITIVITY_820_LSB_PER_GAUSS 820.0
#define MAG_SENSITIVITY_660_LSB_PER_GAUSS 660.0
#define MAG_SENSITIVITY_440_LSB_PER_GAUSS 440.0
#define MAG_SENSITIVITY_390_LSB_PER_GAUSS 390.0
#define MAG_SENSITIVITY_330_LSB_PER_GAUSS 330.0
#define MAG_SENSITIVITY_230_LSB_PER_GAUSS 230.0

// ----- CONFIGURATION OPTIONS ------------------------------------------------------------------
#define DLPF_ACC_260HZ_GYRO_256HZ 0
#define DLPF_ACC_184HZ_GYRO_188HZ 1
#define DLPF_ACC_94HZ_GYRO_98HZ 2
#define DLPF_ACC_44HZ_GYRO_42HZ 3
#define DLPF_ACC_21HZ_GYRO_20HZ 4
#define DLPF_ACC_10HZ_GYRO_10HZ 5
#define DLPF_ACC_15Z_GYRO_5HZ 6

#define GYRO_FS_SEL_250_DEG_PER_SEC 0
#define GYRO_FS_SEL_500_DEG_PER_SEC 1
#define GYRO_FS_SEL_1000_DEG_PER_SEC 2
#define GYRO_FS_SEL_2000_DEG_PER_SEC 3

#define ACC_AFS_SEL_2G 0
#define ACC_AFS_SEL_4G 1
#define ACC_AFS_SEL_8G 2
#define ACC_AFS_SEL_16G 3

#define MAG_SAMPLES_AVERAGED_1 0b00000000
#define MAG_SAMPLES_AVERAGED_2 0b00100000
#define MAG_SAMPLES_AVERAGED_4 0b01000000
#define MAG_SAMPLES_AVERAGED_8 0b01100000

#define MAG_DATA_OUTPUT_RATE_0P75_HZ 0b00000000
#define MAG_DATA_OUTPUT_RATE_1P5_HZ  0b00000100
#define MAG_DATA_OUTPUT_RATE_3_HZ    0b00001000
#define MAG_DATA_OUTPUT_RATE_7P5_HZ  0b00001100
#define MAG_DATA_OUTPUT_RATE_15_HZ   0b00010000
#define MAG_DATA_OUTPUT_RATE_30_HZ   0b00010100
#define MAG_DATA_OUTPUT_RATE_75_HZ   0b00011000

#define MAG_FIELD_RANGE_0P88_GA 0b00000000
#define MAG_FIELD_RANGE_1P3_GA  0b00100000
#define MAG_FIELD_RANGE_1P9_GA  0b01000000
#define MAG_FIELD_RANGE_2P5_GA  0b01100000
#define MAG_FIELD_RANGE_4_GA    0b10000000
#define MAG_FIELD_RANGE_4P7_GA  0b10100000
#define MAG_FIELD_RANGE_5P6_GA  0b11000000
#define MAG_FIELD_RANGE_8P1_GA  0b11100000

#define MAG_MODE_CONTINUOUS_MEASUREMENT 0
#define MAG_MODE_SINGLE_MEASUREMENT 1

// ----- CONFIGURATION REGISTERS ------------------------------------------------------------------
#define DLPF_REG 0x1A
#define GYRO_FS_SEL_REG 0x1B
#define ACC_AFS_SEL_REG 0x1C

#define ACC_GYRO_PWR_MGMT_1 0x6B

// Bypass mode so that hcm5883l is communicatable via i2c
#define ACC_GYRO_INT_PIN_CFG_REG 0x37
#define ACC_GYRO_USER_CONTROL_REG 0x6A

#define MAG_CONFIG_REG_A 0x0
#define MAG_CONFIG_REG_B 0x1
#define MAG_MODE_REG 0x2

// ----- DATA OUTPUT REGISTERS ------------------------------------------------------------------
#define GYRO_FIRST_READ_REG 0x43
#define ACC_FIRST_READ_REG 0x3B
#define MAG_FIRST_READ_REG 0x3

// ----- DATA OUTPUT READ LENGTH ------------------------------------------------------------------
#define ACC_GYRO_MAG_DATA_OUTPUT_READ_LEN 6

// ----- ADDITIONAL MACROS FOR SPECIFIC USAGE ------------------------------------------------------------------
#define GYRO_DATA_OUTPUT_Z_AXIS_REG 0x47
#define GYRO_DATA_OUTPUT_Z_AXIS_READ_LEN 2

// #define float2bytes(value, bytes) std::memcpy(bytes, &value, 4)
// #define bytes2float(bytes, value) std::memcpy(value, bytes, 4)
void float2bytes(float value, uint8_t* bytes);
void bytes2float(uint8_t* bytes, float* value);

class Gy87{
  private: //variables
    float acc_division_factor, gyro_division_factor, mag_division_factor;
    bool gyro_enable, acc_enable, mag_enable;
    Vec3<float> acc_offset,
                gyro_offset,
                mag_hard_iron;
    float mag_soft_iron[3][3];
    bool calibrated, flash_initiated;
    const esp_partition_t *partition;

  public:
    I2c_sensor_interface mpu6050, hmc5883l;
    void update_acc_reading();
    void update_gyro_reading();
    void update_mag_reading();

    void calib_gyro();
    void calib_acc();
    void calib_mag();

    void save_gyro_calib_param();
    void save_acc_calib_param();
    void save_mag_calib_param();

    void load_gyro_calib_param();
    void load_acc_calib_param();
    void load_mag_calib_param();


  public: //variables
    Vec3<float> acc, gyro, mag;

  public: //funcs
    Gy87(bool gyro_enable, bool acc_enable, bool mag_enable);
    void init_i2c(i2c_port_t port);
    void init_flash(const esp_partition_t *partition);
    void config(
        int dlpf_config = 0,
        int acc_afs_sel = 0,
        int gyro_fs_sel = 0,
        int mag_samples_averaged = 0,
        int mag_data_output_rate = 0,
        int mag_field_range = 0,
        int mag_mode = 0
        );
    void update_reading();
    void calib();
    void save_calib_param();
    void load_calib_param();
};

#endif
