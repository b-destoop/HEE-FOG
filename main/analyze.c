//
// Created by bert on 2/11/23.
//

#include "analyze.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "analyze";

void analyze_main(){
    ESP_LOGI(TAG, "Analyze main started");
}