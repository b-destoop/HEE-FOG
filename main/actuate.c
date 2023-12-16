//
// Created by bert on 2/11/23.
//

#include <esp_adc/adc_oneshot.h>
#include <soc/adc_channel.h>
#include <driver/dac_cosine.h>
#include <driver/dac_oneshot.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "thread.h"
#include "angle_sensor.h"
#include <time.h>
#include <math.h>
#include <stdint.h>


// ADC DEFINES
#define ADC_ATTEN           ADC_ATTEN_DB_11
#define LDR_CHAN            ADC1_GPIO36_CHANNEL

static const char *TAG = "ACTUATE";

void actuate_main() {
    ESP_LOGI(TAG, "Main started");
    //-------------SIN Init---------------//
    clock_t startTime = clock(); // reset this value to restart sin on beat.
    const double amplitude = 5.0;
    float frequency = 1.0;  // Adjust as needed
    wearer_state_t wearerState = WALKING;

    //-------------DAC Init---------------//
    dac_oneshot_handle_t dac_chan_handle;
    dac_oneshot_config_t dac_chan_cfg = {
            .chan_id = DAC_CHAN_0,
    };
    ESP_ERROR_CHECK(dac_oneshot_new_channel(&dac_chan_cfg, &dac_chan_handle));
    uint8_t dac_val = 0; //the output value of the DAC

    //-------------ADC1 Init---------------//
    static int adc_read;
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t adc_init_config = {
            .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_init_config, &adc_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t adc_config = {
            .bitwidth = ADC_BITWIDTH_DEFAULT,
            .atten = ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, LDR_CHAN, &adc_config));

    while (1) {
        // FREQUENCY DERIVATION
        data_frame_t dataFrame;
        int res = get_df_from_q(&dataFrame);
//        ESP_LOGI(TAG, "dataframe - ts: %lu, freq: %f, cue: %i", dataFrame.df_timestamp, dataFrame.resonant_frequency,
//                 dataFrame.cue);
        if (res == 0) {
            frequency = dataFrame.resonant_frequency;
            wearerState = dataFrame.wearerState;
        }

        // ADC CODE (one-shot mode)
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, LDR_CHAN, &adc_read));
        //ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, ADC_CHAN, adc_read);

        // SIN WAVE CODE
        //---calculate time from start beat.
        clock_t currentTime = clock();
        double timeInSeconds = (double) (currentTime - startTime) / CLOCKS_PER_SEC;
        //---calculates sin at current time
        double rawSample = amplitude * sin(2.0 * M_PI * frequency * timeInSeconds);
        //---Map the rawSample from the range [-amplitude, amplitude] to [0, UINT8_MAX] (UINT8 are positive integers)
        dac_val = (uint8_t) ((rawSample + amplitude) * UINT8_MAX / (2.0 * amplitude));

        // DAC CODE
        if (wearerState == WALKING)
            ESP_ERROR_CHECK(dac_oneshot_output_voltage(dac_chan_handle, dac_val));

        vTaskDelay(pdMS_TO_TICKS(1));
    }
    ESP_LOGI(TAG, "Main ending...");
    vTaskDelete(NULL);
}