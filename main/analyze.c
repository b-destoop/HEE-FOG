//“Difficult to see; always in motion is the future.” —Yoda, The Empire Strikes Back 
//analyze.c

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "analyze.h"
#include "FFT.h"
#include "thread.h"

static const char *TAG = "analyze";


// Configuration the master be being a boss
esp_err_t Master_config() {
    int i2c_master_port = 0;
    i2c_config_t conf = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = I2C_MASTER_SDA_IO,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_io_num = I2C_MASTER_SCL_IO,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master.clk_speed = I2C_MASTER_FREQ_HZ,
            .clk_flags = 0,
    };
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    return i2c_param_config(I2C_MASTER_NUM, &conf);
}

// I2C Read Function Here the master reads the slave (we dont need a writing function)
esp_err_t i2c_master_read_slave(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data_rd, size_t size) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg_addr, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);
    // this checks every byte  it needs to send an ACK 
    for (size_t i = 0; i < size; i++) {
        i2c_master_read_byte(cmd, data_rd + i, (i == size - 1) ? I2C_MASTER_NACK : I2C_MASTER_ACK);
    }

    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

//apparently you need to wake the shitty thing up since it prefers to only steal my sleep and keep sleeping itself
void wakeUpMPU6050() {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // Write 0 to PWR_MGMT_1 to wake up the MPU6050
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (MPU6050_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, PWR_MGMT_1, true);
    i2c_master_write_byte(cmd, 0x00, true);  // Write 0 to disable sleep mode
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        printf("Error waking up MPU6050. Error code: 0x%X\n", ret);
        // Handle the error as needed
    } else {
        printf("MPU6050 woke up successfully!\n"); //I need this print oke?
    }

    // Wait for the slow and shitty device to wake up and drink a coffee
    for (int i = 0; i < 100; i++) {
        vTaskDelay(10 / portTICK_PERIOD_MS);  // 10 ms coffee ;p
    }
}

void analyze_main() {

    ESP_LOGI(TAG, "Main started");
    if (Master_config() != ESP_OK) {
        printf("I2C initialization failed\n");
        return;
    }

    wakeUpMPU6050();  // Wake up!!!
    uint8_t data[14]; // 14 bytes for 3 axes of accelerometer, 3 axes of gyroscope (we also got a temp sensor in there you know... xxx)
    float gyroscopeDataX[ARRAY_SIZE]; // array with gyro data of the X axis, initial list is filled with zero's
    float gyroscopeDataY[ARRAY_SIZE]; // array with gyro data of the Y axis, initial list is filled with zero's
    float gyroscopeDataZ[ARRAY_SIZE]; // array with gyro data of the Z axis, initial list is filled with zero's
    float AccelerometerDataX[ARRAY_SIZE]; // array with gyro data of the X axis, initial list is filled with zero's
    float AccelerometerDataY[ARRAY_SIZE]; // array with gyro data of the Y axis, initial list is filled with zero's
    float AccelerometerDataZ[ARRAY_SIZE]; // you get the point, don't you?
    int itterator = 0;

    while (1) {
        if (i2c_master_read_slave(MPU6050_ADDR, 0x3B, data, sizeof(data)) == ESP_OK) {
            printf("ts=%lu; ", esp_log_timestamp());

            // now the same with the gyroscope 
            int16_t gyro_x = (data[0] << 8) | data[1];
            int16_t gyro_y = (data[2] << 8) | data[3];
            int16_t gyro_z = (data[4] << 8) | data[5];
            // Extract accelerometer values
            int16_t accel_x = (data[8] << 8) | data[9];
            int16_t accel_y = (data[10] << 8) | data[11];
            int16_t accel_z = (data[12] << 8) | data[13];

            //shift alle data op one place and 'deletes' the last/ oldest element
            for (int i = ARRAY_SIZE - 1; i > 0; --i) {
                gyroscopeDataX[i] = gyroscopeDataX[i - 1];
                gyroscopeDataY[i] = gyroscopeDataY[i - 1];
                gyroscopeDataZ[i] = gyroscopeDataZ[i - 1];
                AccelerometerDataX[i] = AccelerometerDataX[i - 1];
                AccelerometerDataY[i] = AccelerometerDataY[i - 1];
                AccelerometerDataZ[i] = AccelerometerDataZ[i - 1];
            }
            //at the moment i save raw data 
            gyroscopeDataX[0] = gyro_x;
            gyroscopeDataY[0] = gyro_y;
            gyroscopeDataZ[0] = gyro_z;
            AccelerometerDataX[0] = accel_x;
            AccelerometerDataY[0] = accel_y;
            AccelerometerDataZ[0] = accel_z;



            // Print raw data values
            printf("X_raw=%d; Y_raw=%d; Z_raw=%d; ", accel_x, accel_y, accel_z);

            // Convert raw values to physical units (acceleration in g)
            float accel_x_g = accel_x;
            float accel_y_g = accel_y;
            float accel_z_g = accel_z;
            //google said i should do this 
            float gyro_x_dps = gyro_x / 131.0;
            float gyro_y_dps = gyro_y / 131.0;
            float gyro_z_dps = gyro_z / 131.0;

            // Print the values for fun
            printf("X_acc_der=%.2f; Y_acc_der=%.2f; Z_acc_der=%.2f; ", accel_x_g, accel_y_g, accel_z_g); //der = derived
            printf("X_gyr_der=%.2f; Y_gyr_der=%.2f; Z_gyr_der=%.2f; ", gyro_x_dps, gyro_y_dps, gyro_z_dps);

            itterator = itterator + 1;
            float listIM[ARRAY_SIZE] = {0};  // Initialize listIM with zeros

            if (itterator > ARRAY_SIZE) {
                itterator = 0;
                printf("FFT_array_in=[");
                for (unsigned int i = 0; i < ARRAY_SIZE; i++) {
                    printf("%f, ", AccelerometerDataX[i]);
                }
                printf("]; ");

                //float sampleRate = 1000.0 / MS_BETWEEN_MEASUREMENTS; // Convert to seconds
                float freq_magnitudes[ARRAY_SIZE];
                float resonantFreq = fft(AccelerometerDataX, freq_magnitudes, ARRAY_SIZE);

                printf("FFT_array_der=[");
                for (unsigned int i = 0; i < ARRAY_SIZE; i++) {
                    printf("%f, ", freq_magnitudes[i]);
                }
                printf("]; ");

                //float resonantFreq = findResonantFrequency(AccelerometerDataX, ARRAY_SIZE, sampleRate);

                printf("FFT_max_freq=%f; ", resonantFreq);
                data_frame_t output;
                output.df_timestamp = esp_log_timestamp();
                output.resonant_frequency = resonantFreq;
                output.cue = False;
                put_df_in_q(output);
            }

            printf("\n");
            //if we speed this up we will do more measurements per s
            vTaskDelay(MS_BETWEEN_MEASUREMENTS / portTICK_PERIOD_MS); // 4 measurements every second
        } else {
            printf("Failed to read IMU's data\n");
        }


    }


    ESP_LOGI(TAG, "Main ending...");
}
