//“Difficult to see; always in motion is the future.” —Yoda, The Empire Strikes Back 
#include <sys/cdefs.h>
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "analyze.h"

static const char *TAG = "analyze";

// Configuration the master be beeing a boss 
esp_err_t Master_config(){
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
//aparentlly you need to wake the shitty thing up since it prefece to only steal my sleep and keep sleeping itself
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

void analyze_main(){
    ESP_LOGI(TAG, "Main started");

    if (Master_config() != ESP_OK) {
        printf("I2C initialization failed\n");
        return;
    }

    wakeUpMPU6050();  // Wake up!!!
    int ARRAY_SIZE = 50; // how many data elements you want in the array 
    uint8_t data[14]; // 14 bytes for 3 axes of accelerometer, 3 axes of gyroscope (we also got a temp sensor in there you know... xxx)
    float gyroscopeDataX[ARRAY_SIZE]; // array with gyro data of the X axis, initial list is filled with zero's
    float gyroscopeDataY[ARRAY_SIZE]; // array with gyro data of the Y axis, initial list is filled with zero's
    float gyroscopeDataZ[ARRAY_SIZE]; // array with gyro data of the Z axis, initial list is filled with zero's
    float AccelerometerDataX[ARRAY_SIZE]; // array with gyro data of the X axis, initial list is filled with zero's
    float AccelerometerDataY[ARRAY_SIZE]; // array with gyro data of the Y axis, initial list is filled with zero's
    float AccelerometerDataZ[ARRAY_SIZE]; // you get the point don't you?
    int itterator = 0; 

    while (1) {
        if (i2c_master_read_slave(MPU6050_ADDR, 0x3B, data, sizeof(data)) == ESP_OK) {
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
            gyroscopeDataX[0]= gyro_x; 
            gyroscopeDataY[0]= gyro_y; 
            gyroscopeDataZ[0]= gyro_z; 
            AccelerometerDataX[0] = accel_x;
            AccelerometerDataY[0] = accel_y;
            AccelerometerDataZ[0] = accel_z;



            // Print raw data values
            //printf("Raw Data: X=%d, Y=%d, Z=%d\n", accel_x, accel_y, accel_z);

            // Convert raw values to physical units (acceleration in g)
            float accel_x_g = accel_x;
            float accel_y_g = accel_y;
            float accel_z_g = accel_z;
            //google said i should do this 
            float gyro_x_dps = gyro_x / 131.0;
            float gyro_y_dps = gyro_y / 131.0;
            float gyro_z_dps = gyro_z / 131.0;

            // Print the values for fun
            printf("New measurment");
            printf("Accelerometer: X=%.2f g, Y=%.2f g, Z=%.2f g\n", accel_x_g, accel_y_g, accel_z_g);
            printf("Gyroscope: X=%.2f dps, Y=%.2f dps, Z=%.2f dps\n", gyro_x_dps, gyro_y_dps, gyro_z_dps);
        } else {
            printf("Failed to read IMU's data\n");
        }
        //if we speed this up we will do more measurments per s
        vTaskDelay(250 / portTICK_PERIOD_MS); // 4 measurments every second
    }

    ESP_LOGI(TAG, "Main ending...");
}