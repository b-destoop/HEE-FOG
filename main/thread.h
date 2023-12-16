//
// Created by bert on 30/11/23.
//

#ifndef HEE_FOG_THREAD_H
#define HEE_FOG_THREAD_H

#include "esp_log.h"
#include "angle_sensor.h"

#define True    1
#define False   0


typedef struct data_frame {
    uint32_t df_timestamp; //! the ms at which the df is put in the queue
    float resonant_frequency;
    int cue; //! set 1 if the current data frame signals the start of a period
    wearer_state_t wearerState; //! indicate how the patient is behaving
} data_frame_t;

/**
 * Initialize the backend of the custom threading library.
 * @return 0 on success, -1 on fail
 */
int thread_initialize();


/**
 * Put a data frame into the thread queue.
 * @param data
 * @return 0 if successful, -1 on fail, 1 if fail because the queue is full.
 */
int put_df_in_q(data_frame_t data);

/**
 * Get a data frame out of the thread queue.
 * @param data
 * @return -1 if fail, 0 if success
 */
int get_df_from_q(data_frame_t *data);

#endif //HEE_FOG_THREAD_H
