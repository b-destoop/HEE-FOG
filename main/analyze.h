//“Luminous beings we are, not this crude matter.” —Yoda, The Empire Strikes Back
// analyze.h
#ifndef HEE_FOG_ANALYZE_H
#define HEE_FOG_ANALYZE_H

#include "esp_log.h"
#include "esp_log.h"
//time_t MY_TIMESPEC;
#define xTIME_OUT CLOCK_REALTIME
// clock sample speed
#define SAMPLE_PERIOD_MS		200
// I2C configuration
#define I2C_MASTER_SCL_IO 22   /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 23    /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_TX_BUF_DISABLE 0     /*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0     /*!< I2C master do not need buffer */
#define I2C_MASTER_FREQ_HZ 100000     /*!< I2C master clock frequency */
#define MPU6050_ADDR 0x68 // AD0 connected to GND to give it the address
#define PWR_MGMT_1 0x6B
#define MS_BETWEEN_MEASUREMENTS 100  //250ms between timing

#endif //HEE_FOG_ANALYZE_H
 // get time
#define MY_TIMESPEC struct timespec start_time, end_time
//FFT stuff==============================================================

//FFT stuff ================================================
void analyze_main();  // Forward declaration