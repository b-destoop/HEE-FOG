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
#include "hal/ledc_types.h"
#include "thread.h"


// ADC DEFINES
#define ADC_ATTEN           ADC_ATTEN_DB_11
#define ADC_CHAN            ADC1_GPIO36_CHANNEL

static const char *TAG = "ACTUATE";

void actuate_main() {
    ESP_LOGI(TAG, "Main started");
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
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHAN, &adc_config));

    while (1) {
        // FREQUENCY DERIVATION
        data_frame_t dataFrame;
        get_df_from_q(&dataFrame);
        ESP_LOGI(TAG, "dataframe - ts: %lu, freq: %f, cue: %i", dataFrame.df_timestamp, dataFrame.resonant_frequency,
                 dataFrame.cue);

        // ADC CODE (one-shot mode)
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHAN, &adc_read));
        //ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, ADC_CHAN, adc_read);

        // DAC CODE
        dac_val = adc_read >> 4;
        ESP_ERROR_CHECK(dac_oneshot_output_voltage(dac_chan_handle, dac_val));

        vTaskDelay(pdMS_TO_TICKS(1));
    }
    ESP_LOGI(TAG, "Main ending...");
    vTaskDelete(NULL);
}