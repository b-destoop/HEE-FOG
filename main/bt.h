//
// Created by bert on 29/11/23.
//

#ifndef HEE_FOG_BT_H
#define HEE_FOG_BT_H

#define SPP_TAG "BT_SPP_ACCEPTOR"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "ESP_SPP_ACCEPTOR"
#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
#define SPP_SHOW_MODE SPP_SHOW_SPEED    /*Choose show mode: show data or speed*/

int bt_establish_connection();

int bt_send_data(char* input);

#endif //HEE_FOG_BT_H
