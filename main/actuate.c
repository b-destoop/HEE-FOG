//
// Created by bert on 2/11/23.
//

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define BLINK_GPIO 13
#define AMOUNT_OF_BLINKS 5

static uint8_t s_led_state = 0;
static const char *TAG = "actuate";

void blink_x_times(int amount) {
    for (int i = 0; i < amount; i++) {
        ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        /* Set the GPIO level according to the state (LOW or HIGH)*/
        gpio_set_level(BLINK_GPIO, s_led_state);
        // Toggle the LED state //
        s_led_state = !s_led_state;
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}

void actuate_main() {
    blink_x_times(AMOUNT_OF_BLINKS);
}
