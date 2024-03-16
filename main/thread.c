//
// Created by bert on 30/11/23.
//

#include "freertos/FreeRTOS.h"
#include <freertos/queue.h>
#include "thread.h"

QueueHandle_t xQueue;

int thread_initialize() {
    xQueue = xQueueCreate(50, sizeof(data_frame_t));
    if (xQueue) return 0;
    else return -1;
}

int put_df_in_q(data_frame_t data) {
    int res = xQueueSendToBack(xQueue, (void *) &data, 0);
    switch (res) {
        case pdTRUE:
            return 0;
        case errQUEUE_FULL:
            return 1;
        default:
            return -1;
    }
}

int get_df_from_q(data_frame_t *data) {
    int res = xQueueReceive(xQueue, data, 1); // avoid blocking actuate thread
    if (res == pdTRUE) return 0;
    else return -1;
}

