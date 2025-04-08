#include <stdio.h>
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/idf_additions.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

#include "Vl53l0x.h"
#include "Gy87_lib.h"
#include "hal/adc_types.h"

// PINS
#define SDA_PIN               GPIO_NUM_3
#define SCL_PIN               GPIO_NUM_2
#define PWM_LEFT_PIN          GPIO_NUM_5
#define PWM_RIGHT_PIN         GPIO_NUM_4
#define BATTERY_MONITOR_PIN   GPIO_NUM_10
#define TOF_XSHUT_1           GPIO_NUM_8
#define TOF_XSHUT_2           GPIO_NUM_9
#define TOF_XSHUT_3           GPIO_NUM_11
#define TOF_XSHUT_4           GPIO_NUM_6
#define TOF_XSHUT_5           GPIO_NUM_7

// RTOS Tasks delay time
#define RTOS_TOF_TASK_INTERVAL_MS              150
#define RTOS_IMU_TASK_INTERVAL_MS              150
#define RTOS_BATTERY_MONITOR_TASK_INTERVAL_MS  150
#define RTOS_PWM_TASK_INTERVAL_MS              150

// Constants
#define ADC_TO_BATTERY_2S 3.3/4096*(10000+6200)/6200

// Hardware-relevant variables
const esp_partition_t *partition;
adc_oneshot_unit_handle_t adc_handler; 

//

Gy87 imu(true, false, true);
Vl53l0x tof1, tof2, tof3, tof4, tof5;

//------------- RTOS TASKS --------------------------
void imu_task(void*);
void tof_task(void*);
void pwm_task(void*);
void battery_monitor_task(void*);

//------------- FUNCS --------------------------
void i2c_scan(){
  esp_err_t res;
  printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
  printf("00:         ");
  for (uint8_t i = 3; i < 0x78; i++){
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (i << 1) | I2C_MASTER_WRITE, 1 /* expect ack */);
    i2c_master_stop(cmd);

    res = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10 / portTICK_PERIOD_MS);
    if (i % 16 == 0) printf("\n%.2x:", i);
    if (res == 0) printf(" %.2x", i);
    else printf(" --");
    i2c_cmd_link_delete(cmd);
  }
  printf("\n\n");
  vTaskDelay(pdMS_TO_TICKS(1000));
}

extern "C" void app_main(void){
  // ----------- FLASH MEMORY INITIALIZATION ------------
  partition = esp_partition_find_first(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, "calib"); 

  // ----------- I2C INITIALIZATION ------------
  i2c_config_t i2c_bus_config = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = SDA_PIN,
    .scl_io_num = SCL_PIN,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master = {
      .clk_speed = 100000
    },
    .clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL
  };
  i2c_param_config(I2C_NUM_0, &i2c_bus_config);
  i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

  // ---------- GPIOS INITIALIZATION -----------------------
  gpio_set_direction(TOF_XSHUT_1, GPIO_MODE_OUTPUT);
  gpio_set_direction(TOF_XSHUT_2, GPIO_MODE_OUTPUT);
  gpio_set_direction(TOF_XSHUT_3, GPIO_MODE_OUTPUT);
  gpio_set_direction(TOF_XSHUT_4, GPIO_MODE_OUTPUT);
  gpio_set_direction(TOF_XSHUT_5, GPIO_MODE_OUTPUT);

  // ---------- ADC INITIALIZATION ----------------------
  adc_oneshot_unit_init_cfg_t adc_config = {
    .unit_id = ADC_UNIT_1, // adc1, channel9 -> gpio10
    .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
    .ulp_mode = ADC_ULP_MODE_DISABLE
  };
  adc_oneshot_new_unit(&adc_config, &adc_handler);

  adc_oneshot_chan_cfg_t adc_channel_config = {
    .atten = ADC_ATTEN_DB_12,
    .bitwidth = ADC_BITWIDTH_12
  };
  adc_oneshot_config_channel(
    adc_handler,
    ADC_CHANNEL_9, // adc1, channel9 -> gpio10
    &adc_channel_config
  );

  // ----------- TASKS ------------
  xTaskCreatePinnedToCore(imu_task, "imu task", 4096, NULL, 0, NULL, tskNO_AFFINITY);
  xTaskCreatePinnedToCore(tof_task, "tof task", 4096, NULL, 0, NULL, tskNO_AFFINITY);
  xTaskCreatePinnedToCore(battery_monitor_task, "battery monitoring task", 4096, NULL, 0, NULL, tskNO_AFFINITY);
  xTaskCreatePinnedToCore(pwm_task, "pwm task", 4096, NULL, 0, NULL, tskNO_AFFINITY);
}

//------------- RTOS TASKS --------------------------
void imu_task(void*){
  imu.init_i2c(I2C_NUM_0);
  imu.init_flash(partition);
  imu.config(
    DLPF_ACC_44HZ_GYRO_42HZ,
    ACC_AFS_SEL_4G,
    GYRO_FS_SEL_250_DEG_PER_SEC,
    MAG_SAMPLES_AVERAGED_1,
    MAG_DATA_OUTPUT_RATE_75_HZ,
    MAG_FIELD_RANGE_1P3_GA,
    MAG_MODE_CONTINUOUS_MEASUREMENT
  );

  while(true){
    vTaskDelay(pdMS_TO_TICKS(RTOS_IMU_TASK_INTERVAL_MS));
  }
}

void tof_task(void*){
  while(true){
    vTaskDelay(pdMS_TO_TICKS(RTOS_BATTERY_MONITOR_TASK_INTERVAL_MS));
  }
}

void battery_monitor_task(void*){
  int adc_reading;
  float battery_voltage;
  while(true){
    adc_oneshot_read(adc_handler, ADC_CHANNEL_9, &adc_reading);
    battery_voltage = adc_reading * ADC_TO_BATTERY_2S;
    printf("%.4f\n", battery_voltage);
    vTaskDelay(pdMS_TO_TICKS(RTOS_BATTERY_MONITOR_TASK_INTERVAL_MS));
  }
}

void pwm_task(void*){
  while(true){
    vTaskDelay(pdMS_TO_TICKS(RTOS_PWM_TASK_INTERVAL_MS));
  }
}
