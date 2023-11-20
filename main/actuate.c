//
// Created by bert on 2/11/23.
//

#include <esp_adc/adc_oneshot.h>
#include <soc/adc_channel.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/ledc.h"
#include "hal/ledc_types.h"

// PWM DEFINES
#define PWM_GPIO 13

// pwm limit info:
// https://docs.espressif.com/projects/esp-idf/en/v4.2/esp32/api-reference/peripherals/ledc.html#supported-range-of-frequency-and-duty-resolutions
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_HIGH_SPEED_MODE
#define LEDC_OUTPUT_IO          (5) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_5_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (4095) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095 / 8192
#define LEDC_FREQUENCY          (2500000) // Frequency in Hertz. ==> higher frequency means smaller capacitor for RC filter

// ADC DEFINES
#define ADC_ATTEN           ADC_ATTEN_DB_11
#define ADC_CHAN            ADC1_GPIO36_CHANNEL

static const char *TAG = "ACTUATE";

static void pwm_init(void);


void actuate_main() {
    ESP_LOGI(TAG, "Main started");

    pwm_init();

    //-------------ADC1 Init---------------//
    static int adc_read;
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
            .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
            .bitwidth = ADC_BITWIDTH_DEFAULT,
            .atten = ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHAN, &config));

    while (1) {
        // ADC ONE-SHOT CODE
        ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHAN, &adc_read));
        //ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, ADC_CHAN, adc_read);

        // PWM CODE
        // the adc is 12 bit, the duty cycle res is 4 bit ==> bit shift the adc reading 8 bits to the right
        int duty = adc_read >> (12 - LEDC_DUTY_RES);
        // Set duty where ADC_max=4095 and DUTY_max=8192 (=2^13)
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));


        vTaskDelay(pdMS_TO_TICKS(1));
    }
    ESP_LOGI(TAG, "Main ending...");
    vTaskDelete(NULL);


    while (1) {
    }
}


static void pwm_init(void) {
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
            .speed_mode       = LEDC_MODE,
            .timer_num        = LEDC_TIMER,
            .duty_resolution  = LEDC_DUTY_RES,
            .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
            .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
            .speed_mode     = LEDC_MODE,
            .channel        = LEDC_CHANNEL,
            .timer_sel      = LEDC_TIMER,
            .intr_type      = LEDC_INTR_DISABLE,
            .gpio_num       = LEDC_OUTPUT_IO,
            .duty           = 0, // Set duty to 0%
            .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}