#include <sys/cdefs.h>
//
// Created by bert on 2/11/23.
//

#include "analyze.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "ANALYZE";

void analyze_main(){
    ESP_LOGI(TAG, "Main started");

    ESP_LOGI(TAG, "Main ending...");
    vTaskDelete(NULL);
}