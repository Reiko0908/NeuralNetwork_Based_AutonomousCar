#include <stdio.h>
#include "esp_partition.h"

extern "C" void app_main(void)
{
  const esp_partition_t* partition = esp_partition_find_first(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, "calib"); 
  if(partition == NULL)
    printf("none");

  uint8_t write_buffer = 254;
  esp_partition_write(partition, 0, &write_buffer, 1);
  uint8_t read_buffer;
  esp_partition_read(partition, 0, &read_buffer, 1);

  printf("%d", read_buffer);
}
