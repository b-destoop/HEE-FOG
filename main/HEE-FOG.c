/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <pthread.h>

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

static uint8_t s_led_state = 0;

int blink10times();

void app_main(void) {
    pthread_attr_t attr;
    pthread_t thread1, thread2;
    int res = 0;

    /* Set the GPIO as a push/pull output */
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    // create the 2 threads
    pthread_create(&thread1, NULL, blink10times, NULL);
    vTaskDelay(30 / portTICK_PERIOD_MS);
    pthread_create(&thread2, NULL, blink10times, NULL);

    // wait for the threads to finish
    ESP_LOGI(TAG, "Waiting for threads to finish.");
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    ESP_LOGI(TAG, "thread1 joined.");

    gpio_set_level(BLINK_GPIO, 1);

}

int blink10times() {
    for (int i = 0; i < 10; i++) {
        ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
        /* Set the GPIO level according to the state (LOW or HIGH)*/
        gpio_set_level(BLINK_GPIO, s_led_state);
        // Toggle the LED state //
        s_led_state = !s_led_state;
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
    return 0;
}