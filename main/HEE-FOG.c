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

#include "analyze.h"
#include "actuate.h"

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO


void app_main(void) {
    pthread_t thread_analyze, thread_actuators;
    int res;

    /* Set the GPIO as a push/pull output */
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    // create the 2 threads
    res = pthread_create(&thread_analyze, NULL, (void *(*)(void *)) analyze_main, NULL);
    assert(res == 0);
    vTaskDelay(30 / portTICK_PERIOD_MS);
    res = pthread_create(&thread_actuators, NULL, (void *(*)(void *)) actuate_main, NULL);
    assert(res == 0);

    // wait for the threads to finish
    ESP_LOGI(TAG, "Waiting for threads to finish.");
    pthread_join(thread_analyze, NULL);
    ESP_LOGI(TAG, "Thread analyze finished.");
    pthread_join(thread_actuators, NULL);
    ESP_LOGI(TAG, "Thread actuate finished.");

    gpio_set_level(BLINK_GPIO, 1);

}
