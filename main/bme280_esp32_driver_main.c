#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bme280.h"

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
        bme280_read_temp_press_and_hum(&bme0);
        double temperature = bme280_get_temperature(&bme0);
        double pressure = bme280_get_pressure(&bme0);
        double humidity = bme280_get_humidity(&bme0);
        double altitude = bme280_get_altitude(pressure, 1013.0);
        
        printf("Temperature: %.1f *C\r\n", temperature);
        printf("Pressure:    %.0f hPa\r\n", pressure);
        printf("Humidity:    %.2f %%\r\n", humidity);
        printf("Altitude:    %.0f m\r\n", altitude);
        printf("\r\n");

        vTaskDelay(10 * 1000);
    }
}
