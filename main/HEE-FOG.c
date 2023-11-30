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
#include "thread.h"


/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO


void app_main(void) {
    /* tasks work like threads, kinda https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_idf.html */

    /* Set the GPIO as a push/pull output */
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    // create the 2 tasks
    TaskHandle_t analyze_task_handle, actuate_task_handle = NULL;

    // initialize the threading
    thread_initialize();

    // Create the task, storing the handle.  Note that the passed parameter ucParameterToPass
    // must exist for the lifetime of the task, so in this case is declared static.  If it was just an
    // an automatic stack variable it might no longer exist, or at least have been corrupted, by the time
    // the new task attempts to access it.
    xTaskCreate(analyze_main, "ANALYZE", CONFIG_ESP_MAIN_TASK_STACK_SIZE,
                NULL, tskIDLE_PRIORITY, &analyze_task_handle);
    configASSERT(analyze_task_handle);
    xTaskCreate(actuate_main, "ACTUATE", CONFIG_ESP_MAIN_TASK_STACK_SIZE,
                NULL, tskIDLE_PRIORITY, &actuate_task_handle);
    configASSERT(actuate_task_handle);

}
