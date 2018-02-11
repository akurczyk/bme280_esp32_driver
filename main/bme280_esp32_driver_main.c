#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bme280.h"

void delay_sec(int time)
{
    vTaskDelay(time * 1000 / portTICK_PERIOD_MS);
}

void app_main()
{
    bme280_config_t bme0 = {
            .sda_pin = GPIO_NUM_19,
            .scl_pin = GPIO_NUM_18,
            .i2c_instance = I2C_NUM_1
    };
    bme280_setup(&bme0);

    for (;;)
    {
        bme280_measurement_t reading;
        bme280_make_measurement(&bme0, &reading);
        bme280_print_measurement(&reading);

        delay_sec(10);
    }
}
