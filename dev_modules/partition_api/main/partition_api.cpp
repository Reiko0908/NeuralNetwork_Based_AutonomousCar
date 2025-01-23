#include <stdio.h>
#include "esp_partition.h"

extern "C" void app_main(void)
{
  const esp_partition_t* partition = esp_partition_find_first(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, "calib"); 
  if(partition == NULL)
    printf("none");

  printf("%d %d\n", (int)partition->size, (int)partition->erase_size);

  uint8_t write_buffer = 125, read_buffer;
  ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->erase_size * 2));
  ESP_ERROR_CHECK(esp_partition_write(partition, 0, &write_buffer, 1));
  ESP_ERROR_CHECK(esp_partition_read(partition, 0, &read_buffer, 1));

  printf("%d", read_buffer);
}
