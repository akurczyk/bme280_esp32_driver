#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"

typedef struct {
    gpio_num_t sda_pin;
    gpio_num_t scl_pin;
    i2c_port_t i2c_instance;
    uint16_t T1;
    int16_t T2;
    int16_t T3;
    uint16_t P1;
    int16_t P2;
    int16_t P3;
    int16_t P4;
    int16_t P5;
    int16_t P6;
    int16_t P7;
    int16_t P8;
    int16_t P9;
    uint8_t H1;
    int16_t H2;
    uint8_t H3;
    int16_t H4;
    int16_t H5;
    int8_t H6;
    int32_t adc_T;
    int32_t adc_P;
    int32_t adc_H;
    int32_t t_fine;
} bme280_config_t;

typedef struct {
    double temperature;
    double pressure;
    double humidity;
    double altitude;
} bme280_measurement_t;

uint8_t bme280_setup(bme280_config_t* inst);
uint8_t bme280_read_data(bme280_config_t* inst, uint8_t reg);
void bme280_write_data(bme280_config_t* inst, uint8_t reg, uint8_t value);
void bme280_read_compensation_data(bme280_config_t* inst);
void bme280_set_sampling(bme280_config_t* inst);
void bme280_read_temp_press_and_hum(bme280_config_t* inst);
double bme280_get_temperature(bme280_config_t* inst);
double bme280_get_pressure(bme280_config_t* inst);
double bme280_get_humidity(bme280_config_t* inst);
double bme280_get_altitude(double p, double p0);
void bme280_make_measurement(bme280_config_t* inst, bme280_measurement_t* reading);
void bme280_print_measurement(bme280_measurement_t* reading);
void bme280_clear_measurement(bme280_measurement_t* m);
void bme280_add_measurements(bme280_measurement_t* m1, bme280_measurement_t* m2);
void bme280_div_measurement(bme280_measurement_t* m, int d);