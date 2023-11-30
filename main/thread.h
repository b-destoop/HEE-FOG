//
// Created by bert on 30/11/23.
//

#ifndef HEE_FOG_THREAD_H
#define HEE_FOG_THREAD_H

#include "esp_log.h"

#define True = 1
#define False = 0

typedef struct data_frame {
    uint32_t timestamp; //! the ms at which the df is put in the queue
    float resonant_frequency;
    float cue; //! set 1 if the current data frame signals the start of a period
} data_frame_t;

/**
 * Put a data frame into the thread queue.
 * @param data
 * @return 0 if successful, -1 if fail.
 */
int put_df_in_q(data_frame_t data);

/**
 * Get a data frame out of the thread queue.
 * @param data
 * @return -1 if fail, 0 if success, 1 if queue emtpy
 */
int get_df_from_q(data_frame_t* data);

#endif //HEE_FOG_THREAD_H
