#include <cstddef>
#include <cstdio>
#include <stdio.h>
#include "esp_err.h"
#include "esp_spiffs.h"
#include "esp_log.h"

static const char* TAG = "FileSystem";

extern "C" void app_main(void)
{
  esp_vfs_spiffs_conf_t spiffs_config = {
    .base_path = "/storage",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true
  };
  esp_err_t spiffs_result = esp_vfs_spiffs_register(&spiffs_config);

  if (spiffs_result != ESP_OK){
    ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(spiffs_result));
    return;
  }

  size_t total = 0, used = 0;
  spiffs_result = esp_spiffs_info(spiffs_config.partition_label, &total, &used);
  if (spiffs_result != ESP_OK){
    ESP_LOGE(TAG, "Failed to get partition info (%s)", esp_err_to_name(spiffs_result));
  } else {
    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
  }

  FILE* f = fopen("/storage/my_project.txt", "r");
  if (f == NULL){
    ESP_LOGE(TAG, "Failed to open file for reading");
    return;
  }

  char line[64];
  fgets(line, sizeof(line), f);
  fclose(f);
  printf("%s\n", line);
}
