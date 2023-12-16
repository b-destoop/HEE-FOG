//
// Created by bert on 13/12/23.
//

#ifndef HEE_FOG_ANGLE_SENSOR_H
#define HEE_FOG_ANGLE_SENSOR_H

#endif //HEE_FOG_ANGLE_SENSOR_H

#ifndef WEARER_STATE_DEF
#define WEARER_STATE_DEF
typedef enum {
    WALKING = 0,
    SITTING,
    STANDING
} wearer_state_t;
#endif

void angle_sens_init();

double angle_sens_read_angle();

wearer_state_t get_wearer_state();
