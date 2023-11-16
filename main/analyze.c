#include <sys/cdefs.h>
//
// Created by bert on 2/11/23.
//

#include "analyze.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "analyze";

void analyze_main(){
    ESP_LOGI(TAG, "Main started");

    ESP_LOGI(TAG, "Main ending...");
    vTaskDelete(NULL);
}