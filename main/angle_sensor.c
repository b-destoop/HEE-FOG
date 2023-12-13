//
// Created by bert on 13/12/23.
//

#include "angle_sensor.h"

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "analyze.h"
#include "FFT.h"
#include "thread.h"
#include "angle_sensor.h"
#include <esp_adc/adc_oneshot.h>
#include <soc/adc_channel.h>
#include "driver/gpio.h"
#include <math.h>

#define LDR_CHAN ADC2_GPIO4_CHANNEL
#define LED_GPIO 22

static adc_oneshot_unit_handle_t adc_handle;

static const char *TAG = "angle_sens";

void angle_sens_init() {
    //-------------ADC1 Init---------------//
    adc_oneshot_unit_init_cfg_t adc_init_config = {
            .unit_id = ADC_UNIT_2,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_init_config, &adc_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t adc_config = {
            .bitwidth = ADC_BITWIDTH_DEFAULT,
            .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, LDR_CHAN, &adc_config));
    //gpio_set_level(LED_GPIO, 1);
}


double angle_sens_read_angle() {
    int ldrValue;
    double angleValue;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, LDR_CHAN, &ldrValue));
    angleValue =  -36.73 * log(ldrValue) + 291.83;
    ESP_LOGI(TAG, "ldr_value: %i - angleValue: %f", ldrValue, angleValue);
    return angleValue;
}
