#include "Gy87_lib.h"
#include "esp_err.h"
#include "esp_partition.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"

void float2bytes(float value, uint8_t* bytes){
  std::memcpy(bytes, &value, 4);
}

void bytes2float(uint8_t* bytes, float* value){
  std::memcpy(value, bytes, 4);
}

Gy87::Gy87(bool gyro_enable, bool acc_enable, bool mag_enable){
  this->gyro_enable = gyro_enable; 
  this->acc_enable = acc_enable; 
  this->mag_enable = mag_enable; 
  this->calibrated = false;
}

void Gy87::init_i2c(i2c_port_t port){
  this->mpu6050.init(I2C_MPU6050_ADDR, port);
  this->hmc5883l.init(I2C_HMC5883L_ADDR, port);
}

void Gy87::init_flash(const esp_partition_t *partition){
  this->partition = partition;
}

void Gy87::config(
    int dlpf_config,
    int acc_afs_sel,
    int gyro_fs_sel,

    int mag_samples_averaged,
    int mag_data_output_rate,
    int mag_field_range,
    int mag_mode
    ){

  if(this->acc_enable or this->gyro_enable){
    this->mpu6050.write_reg_single(ACC_GYRO_PWR_MGMT_1, 0x00);
    this->mpu6050.write_reg_single(DLPF_REG, dlpf_config);
  }

  if(this->acc_enable){
    this->mpu6050.write_reg_single(ACC_AFS_SEL_REG, acc_afs_sel);
    switch(acc_afs_sel){
      case ACC_AFS_SEL_2G:
        this->acc_division_factor = ACC_SENSITIVITY_16384_LSB_PER_G;
        break;

      case ACC_AFS_SEL_4G:
        this->acc_division_factor = ACC_SENSITIVITY_8192_LSB_PER_G;
        break;

      case ACC_AFS_SEL_8G:
        this->acc_division_factor = ACC_SENSITIVITY_4096_LSB_PER_G;
        break;

      case ACC_AFS_SEL_16G:
        this->acc_division_factor = ACC_SENSITIVITY_2048_LSB_PER_G;
        break;
    }
  }

  if(this->gyro_enable){
    this->mpu6050.write_reg_single(GYRO_FS_SEL_REG, gyro_fs_sel);
    switch(gyro_fs_sel){
      case GYRO_FS_SEL_250_DEG_PER_SEC:
        this->gyro_division_factor = GYRO_SENSITIVITY_131_LSB_PER_DEG_PER_SEC;
        break;

      case GYRO_FS_SEL_500_DEG_PER_SEC:
        this->gyro_division_factor = GYRO_SENSITIVITY_65P5_LSB_PER_DEG_PER_SEC;
        break;

      case GYRO_FS_SEL_1000_DEG_PER_SEC:
        this->gyro_division_factor = GYRO_SENSITIVITY_32P8_LSB_PER_DEG_PER_SEC;
        break;

      case GYRO_FS_SEL_2000_DEG_PER_SEC:
        this->gyro_division_factor = GYRO_SENSITIVITY_16P4_LSB_PER_DEG_PER_SEC;
        break;
    }
  }

  if(this->mag_enable){
    uint8_t default_1 = 0b10000000; //default unused bits config reg A
    uint8_t default_2 = 0; //default unused bits cconfig reg B
    uint8_t default_3 = 0b10000000; //default unused bits mode reg

    this->hmc5883l.write_reg_single(ACC_GYRO_INT_PIN_CFG_REG, 0b10);
    this->hmc5883l.write_reg_single(ACC_GYRO_USER_CONTROL_REG, 0x0);

    this->hmc5883l.write_reg_single(
        MAG_CONFIG_REG_A,
        default_1 | mag_samples_averaged | mag_data_output_rate
        );
    this->hmc5883l.write_reg_single(
        MAG_CONFIG_REG_B,
        default_2 | mag_field_range
        );
    this->hmc5883l.write_reg_single(
        MAG_MODE_REG,
        default_3 | mag_mode
        );

    switch(mag_field_range){
      case MAG_FIELD_RANGE_0P88_GA:
        this->mag_division_factor = MAG_SENSITIVITY_1370_LSB_PER_GAUSS;
        break;

      case MAG_FIELD_RANGE_1P3_GA:
        this->mag_division_factor = MAG_SENSITIVITY_1090_LSB_PER_GAUSS;
        break;

      case MAG_FIELD_RANGE_1P9_GA:
        this->mag_division_factor = MAG_SENSITIVITY_820_LSB_PER_GAUSS;
        break;

      case MAG_FIELD_RANGE_2P5_GA:
        this->mag_division_factor = MAG_SENSITIVITY_660_LSB_PER_GAUSS;
        break;

      case MAG_FIELD_RANGE_4_GA:
        this->mag_division_factor = MAG_SENSITIVITY_440_LSB_PER_GAUSS;
        break;

      case MAG_FIELD_RANGE_4P7_GA:
        this->mag_division_factor = MAG_SENSITIVITY_390_LSB_PER_GAUSS;
        break;

      case MAG_FIELD_RANGE_5P6_GA:
        this->mag_division_factor = MAG_SENSITIVITY_330_LSB_PER_GAUSS;
        break;

      case MAG_FIELD_RANGE_8P1_GA:
        this->mag_division_factor = MAG_SENSITIVITY_230_LSB_PER_GAUSS;
        break;
    }
  }
}

//----UPDATE READINGS PROCEDURES-------------------------------------------
void Gy87::update_gyro_reading(){
  uint8_t buf[ACC_GYRO_MAG_DATA_OUTPUT_READ_LEN];
  this->mpu6050.read_reg_multi(GYRO_FIRST_READ_REG, buf, ACC_GYRO_MAG_DATA_OUTPUT_READ_LEN);
  this->gyro.x = (int16_t)((int16_t)buf[0] << 8 | buf[1]);
  this->gyro.y = (int16_t)((int16_t)buf[2] << 8 | buf[3]);
  this->gyro.z = (int16_t)((int16_t)buf[4] << 8 | buf[5]);
  this->gyro /= this->gyro_division_factor;
  if(this->calibrated)
    this->gyro -= this->gyro_offset;
}

void Gy87::update_acc_reading(){
  uint8_t buf[ACC_GYRO_MAG_DATA_OUTPUT_READ_LEN];
  this->mpu6050.read_reg_multi(ACC_FIRST_READ_REG, buf, ACC_GYRO_MAG_DATA_OUTPUT_READ_LEN);
  this->acc.x = (int16_t)((int16_t)buf[0] << 8 | buf[1]);
  this->acc.y = (int16_t)((int16_t)buf[2] << 8 | buf[3]);
  this->acc.z = (int16_t)((int16_t)buf[4] << 8 | buf[5]);
  this->acc /= this->acc_division_factor;
  if(this->calibrated){
    this->acc -= this->acc_offset;
    this->acc.z -= 1.0;
  }
}

void Gy87::update_mag_reading(){
  uint8_t buf[ACC_GYRO_MAG_DATA_OUTPUT_READ_LEN];
  this->hmc5883l.read_reg_multi(MAG_FIRST_READ_REG, buf, ACC_GYRO_MAG_DATA_OUTPUT_READ_LEN);
  this->mag.x = (int16_t)((int16_t)buf[0] << 8 | buf[1]);
  this->mag.y = (int16_t)((int16_t)buf[2] << 8 | buf[3]);
  this->mag.z = (int16_t)((int16_t)buf[4] << 8 | buf[5]);
  this->mag /= this->mag_division_factor;
  if(this->calibrated)
    this->mag -= this->mag_hard_iron;
}

void Gy87::update_reading(){
  if(this->acc_enable) this->update_acc_reading();
  if(this->gyro_enable) this->update_gyro_reading();
  if(this->mag_enable) this->update_mag_reading();
}


//----CALIBRATION PROCEDURES-------------------------------------------
void Gy87::calib_gyro(){
  if(not this->gyro_enable){
    printf("Gyroscope is not enabled\n");
    return;
  }

  printf("Start calibrating gyroscope\n");
  printf("Hold the board still for 1s\n");
  vTaskDelay(1250 / portTICK_PERIOD_MS);

  double gyro_x_sum = 0.0,
         gyro_y_sum = 0.0,
         gyro_z_sum = 0.0;

  for(int i = 0; i < NUM_CALIB_SAMPLES; i++){
    this->update_gyro_reading();
    gyro_x_sum += this->gyro.x;
    gyro_y_sum += this->gyro.y;
    gyro_z_sum += this->gyro.z;
  }

  this->calibrated = true;
  this->gyro_offset = {
    (float)(gyro_x_sum / NUM_CALIB_SAMPLES),
    (float)(gyro_y_sum / NUM_CALIB_SAMPLES),
    (float)(gyro_z_sum / NUM_CALIB_SAMPLES)
  };

  printf("Gyroscope calibrated\n");
}

void Gy87::calib_acc(){
  if(not this->acc_enable){
    printf("Accelerometer is not enabled\n");
    return;
  }

  printf("Start calibrating accelerometer\n");
  printf("Hold the board still for 1s\n");
  vTaskDelay(1250 / portTICK_PERIOD_MS);

  double acc_x_sum = 0.0,
         acc_y_sum = 0.0,
         acc_z_sum = 0.0;

  for(int i = 0; i < NUM_CALIB_SAMPLES; i++){
    this->update_acc_reading();
    acc_x_sum += this->acc.x;
    acc_y_sum += this->acc.y;
    acc_z_sum += this->acc.z;
  }

  this->calibrated = true;
  this->acc_offset = {
    (float)(acc_x_sum / NUM_CALIB_SAMPLES),
    (float)(acc_y_sum / NUM_CALIB_SAMPLES),
    (float)(acc_z_sum / NUM_CALIB_SAMPLES)
  };

  printf("Accelerometer calibrated\n");
}

void Gy87::calib_mag(){
  if(not this->mag_enable){
    printf("Magnetometer is not enabled\n");
    return;
  }

  printf("Start calibrating magnetometer\n");
  printf("Rotate the board around 360 degrees in all 3D directions for 10s\n");
  vTaskDelay(1250 / portTICK_PERIOD_MS);

  uint32_t start_us = esp_timer_get_time();
  Vec3<float> reading_min(1000.0, 1000.0, 1000.0),
              reading_max(-1000.0, -1000.0, -1000.0);

  while(esp_timer_get_time() - start_us < MAG_CALIB_DURATION_US){
    this->update_mag_reading();
    
    if(this->mag.x >= reading_max.x) reading_max.x = this->mag.x;
    if(this->mag.y >= reading_max.y) reading_max.y = this->mag.y;
    if(this->mag.z >= reading_max.z) reading_max.z = this->mag.z;
    if(this->mag.x <= reading_min.x) reading_min.x = this->mag.x;
    if(this->mag.y <= reading_min.y) reading_min.y = this->mag.y;
    if(this->mag.z <= reading_min.z) reading_min.z = this->mag.z;
  }

  this->calibrated = true;
  this->mag_hard_iron = (reading_max + reading_min) / 2;

  printf("Magnetometer calibrated\n");
}

void Gy87::calib(){
  this->calibrated = true;
  this->calib_gyro();
  this->calib_acc();
  this->calib_mag();
}
//-----------------------------------------------------------------------

//----SAVE CALIBRATION PARAMETERS PROCEDURES-----------------------------
void Gy87::save_gyro_calib_param(){
  if(not this->gyro_enable){
    printf("Gyroscope is not enabled\n");
    return;
  }
  uint8_t buf[sizeof(float)*3];
  vec2bytes(this->gyro_offset, buf);
  esp_partition_erase_range(this->partition, PARTITION_GYRO_OFFSETS_START_REG, this->partition->erase_size);
  esp_partition_write(this->partition, PARTITION_GYRO_OFFSETS_START_REG, buf, sizeof(float)*3);
  printf("Saved gyroscope calibration parameters\n");
}

void Gy87::save_acc_calib_param(){
  if(not this->acc_enable){
    printf("Accelerometer is not enabled\n");
    return;
  }
  uint8_t buf[sizeof(float) * 3];
  vec2bytes(this->acc_offset, buf);
  esp_partition_erase_range(this->partition, PARTITION_ACC_OFFSETS_START_REG, this->partition->erase_size);
  esp_partition_write(this->partition, PARTITION_ACC_OFFSETS_START_REG, buf, sizeof(float) * 3);
  printf("Saved accelerometer calibration parameters\n");
}

void Gy87::save_mag_calib_param(){
  if(not this->mag_enable){
    printf("Magnetometer is not enabled\n");
    return;
  }

  uint8_t buf[sizeof(float) * 3];
  vec2bytes(this->mag_hard_iron, buf);
  esp_partition_erase_range(this->partition, PARTITION_MAG_HARD_IRON_START_REG, this->partition->erase_size);
  esp_partition_write(this->partition, PARTITION_MAG_HARD_IRON_START_REG, buf, sizeof(float) * 3);
  printf("Saved magnetometer calibration parameters\n");
}

void Gy87::save_calib_param(){
  if(!this->calibrated) return;
  this->save_gyro_calib_param();
  this->save_acc_calib_param();
  this->save_mag_calib_param();
}
//-----------------------------------------------------------------------

//----LOAD CALIBRATION PARAMETERS PROCEDURES-----------------------------
void Gy87::load_gyro_calib_param(){
  this->calibrated = true;
  uint8_t buf[sizeof(float)*3];
  esp_partition_read(this->partition, PARTITION_GYRO_OFFSETS_START_REG, buf, sizeof(float) * 3);
  bytes2vec(buf, &this->gyro_offset);
  printf("Gyroscope calibration parameters loaded from memory\n");
}

void Gy87::load_acc_calib_param(){
  this->calibrated = true;
  uint8_t buf[sizeof(float)*3];
  esp_partition_read(this->partition, PARTITION_ACC_OFFSETS_START_REG, buf, sizeof(float) * 3);
  bytes2vec(buf, &this->acc_offset);
  printf("Accelerometer calibration parameters loaded from memory\n");
}

void Gy87::load_mag_calib_param(){
  this->calibrated = true;
  uint8_t buf[sizeof(float)*3];
  esp_partition_read(this->partition, PARTITION_MAG_HARD_IRON_START_REG, buf, sizeof(float) * 3);
  bytes2vec(buf, &this->mag_hard_iron);
  printf("Magnetometer calibration parameters loaded from memory\n");
}

void Gy87::load_calib_param(){
  this->calibrated = true;
  if(this->gyro_enable) this->load_gyro_calib_param();
  if(this->acc_enable) this->load_acc_calib_param();
  if(this->mag_enable) this->load_mag_calib_param();
}
//-----------------------------------------------------------------------
