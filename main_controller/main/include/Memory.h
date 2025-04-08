#ifndef MEMORY
#define MEMORY

// This file stores the addresses (destination) of calibrations
// This file stores important addresses of calibrations data,
// configurations in the ESP32S3 flash memory by making use
// of the esp_partition.h api

#include "esp_partition.h"

#define PARTITION_GYRO_OFFSETS_START_REG 0x0000
#define PARTITION_ACC_OFFSETS_START_REG 0x1000 //to fit the partition->erase_size
#define PARTITION_MAG_HARD_IRON_START_REG 0x2000
#define PARTITION_MAG_SOFT_IRON_START_REG 0x3000
#define PARTITION_TOF_OFFSETS_START_REG 0x4000

#endif
