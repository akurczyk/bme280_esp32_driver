#include <math.h>
#include "bme280.h"

uint8_t bme280_setup(bme280_config_t* inst)
{
    // Clear all the registers
    inst->T1 = 0;
    inst->T2 = 0;
    inst->T3 = 0;
    inst->P1 = 0;
    inst->P2 = 0;
    inst->P3 = 0;
    inst->P4 = 0;
    inst->P5 = 0;
    inst->P6 = 0;
    inst->P7 = 0;
    inst->P8 = 0;
    inst->P9 = 0;
    inst->H1 = 0;
    inst->H2 = 0;
    inst->H3 = 0;
    inst->H4 = 0;
    inst->H5 = 0;
    inst->H6 = 0;

    // Setup I2C interface
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = inst->sda_pin;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = inst->scl_pin;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    i2c_param_config(inst->i2c_instance, &conf);
    i2c_driver_install(inst->i2c_instance, conf.mode, 0, 0, 0);

    // Initialize communication with the sensor
    if (bme280_read_data(inst, 0xD0) != 0x60)
        return 0;

    bme280_write_data(inst, 0xE0, 0xB6);

    vTaskDelay(300);
    while ((bme280_read_data(inst, 0xF3) & 0x01) != 0)
        vTaskDelay(100);

    bme280_read_compensation_data(inst);
    bme280_set_sampling(inst);

    return 1;
}

uint8_t bme280_read_data(bme280_config_t* inst, uint8_t reg)
{
    i2c_cmd_handle_t cmd;
    uint8_t value;

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0xEC, 1);
    i2c_master_write_byte(cmd, reg, 1);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(inst->i2c_instance, cmd, 1000);
    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0xED, 1);
    i2c_master_read_byte(cmd, &value, 1);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(inst->i2c_instance, cmd, 1000);
    i2c_cmd_link_delete(cmd);

    return value;
}

void bme280_write_data(bme280_config_t* inst, uint8_t reg, uint8_t value)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0xEC, 1);
    i2c_master_write_byte(cmd, reg, 1);
    i2c_master_write_byte(cmd, value, 1);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(inst->i2c_instance, cmd, 1000);
    i2c_cmd_link_delete(cmd);
}

void bme280_read_compensation_data(bme280_config_t* inst)
{
    inst->T1 = (bme280_read_data(inst, 0x89) << 8) + bme280_read_data(inst, 0x88);
    inst->T2 = (bme280_read_data(inst, 0x8B) << 8) + bme280_read_data(inst, 0x8A);
    inst->T3 = (bme280_read_data(inst, 0x8D) << 8) + bme280_read_data(inst, 0x8C);

    inst->P1 = (bme280_read_data(inst, 0x8F) << 8) + bme280_read_data(inst, 0x8E);
    inst->P2 = (bme280_read_data(inst, 0x91) << 8) + bme280_read_data(inst, 0x90);
    inst->P3 = (bme280_read_data(inst, 0x93) << 8) + bme280_read_data(inst, 0x92);
    inst->P4 = (bme280_read_data(inst, 0x95) << 8) + bme280_read_data(inst, 0x94);
    inst->P5 = (bme280_read_data(inst, 0x97) << 8) + bme280_read_data(inst, 0x96);
    inst->P6 = (bme280_read_data(inst, 0x99) << 8) + bme280_read_data(inst, 0x98);
    inst->P7 = (bme280_read_data(inst, 0x9B) << 8) + bme280_read_data(inst, 0x9A);
    inst->P8 = (bme280_read_data(inst, 0x9D) << 8) + bme280_read_data(inst, 0x9C);
    inst->P9 = (bme280_read_data(inst, 0x9F) << 8) + bme280_read_data(inst, 0x9E);

    inst->H1 = bme280_read_data(inst, 0xA1);
    inst->H2 = (bme280_read_data(inst, 0xE2) << 8) + bme280_read_data(inst, 0xE1);
    inst->H3 = bme280_read_data(inst, 0xE3);
    inst->H4 = (bme280_read_data(inst, 0xE4) << 4) | (bme280_read_data(inst, 0xE5) & 0x0F);
    inst->H5 = (bme280_read_data(inst, 0xE6) << 4) | (bme280_read_data(inst, 0xE5) >> 4);
    inst->H6 = bme280_read_data(inst, 0xE7);
}

void bme280_set_sampling(bme280_config_t* inst)
{
    bme280_write_data(inst, 0xF2, 0x05);
    bme280_write_data(inst, 0xF5, 0x00);
    bme280_write_data(inst, 0xF4, 0xB7);
}

void bme280_read_temp_press_and_hum(bme280_config_t* inst)
{
    inst->adc_T = (bme280_read_data(inst, 0xFA) << 16) + (bme280_read_data(inst, 0xFB) << 8) + bme280_read_data(inst, 0xFC);
    inst->adc_P = (bme280_read_data(inst, 0xF7) << 16) + (bme280_read_data(inst, 0xF8) << 8) + bme280_read_data(inst, 0xF9);
    inst->adc_H = (bme280_read_data(inst, 0xFD) << 8) + bme280_read_data(inst, 0xFE);
}

double bme280_get_temperature(bme280_config_t* inst)
{
    int32_t var1, var2;

    if (inst->adc_T == 0x800000)
        return 0.0;
    inst->adc_T >>= 4;

    var1 = ((((inst->adc_T >> 3) - ((int32_t) inst->T1 << 1))) * ((int32_t) inst->T2)) >> 11;
    var2 = (((((inst->adc_T >> 4) - ((int32_t) inst->T1)) * ((inst->adc_T >> 4) - ((int32_t) inst->T1))) >> 12) * ((int32_t) inst->T3)) >> 14;
    inst->t_fine = var1 + var2;

    double T = (inst->t_fine * 5 + 128) >> 8;
    return T / 100;
}

double bme280_get_pressure(bme280_config_t* inst)
{
    int64_t var1, var2, p;

    if (inst->adc_P == 0x800000)
        return 0.0;
    inst->adc_P >>= 4;

    var1 = ((int64_t) inst->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t) inst->P6;
    var2 = var2 + ((var1 * (int64_t) inst->P5) << 17);
    var2 = var2 + (((int64_t) inst->P4) << 35);
    var1 = ((var1 * var1 * (int64_t) inst->P3) >> 8) + ((var1 * (int64_t) inst->P2) << 12);
    var1 = (((((int64_t) 1) << 47) + var1)) * ((int64_t) inst->P1) >> 33;

    if (var1 == 0)
        return 0;
    p = 1048576 - inst->adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t) inst->P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t) inst->P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t) inst->P7) << 4);
    return ((double) p / 256.0) / 100.0;
}

double bme280_get_humidity(bme280_config_t* inst)
{
    if (inst->adc_H == 0x8000)
        return 0.0;

    int32_t v_x1_u32r;

    v_x1_u32r = (inst->t_fine - ((int32_t) 76800));

    v_x1_u32r = (((((inst->adc_H << 14) - (((int32_t) inst->H4) << 20)
            - (((int32_t) inst->H5) * v_x1_u32r)) + ((int32_t) 16384)) >> 15)
            * (((((((v_x1_u32r * ((int32_t) inst->H6)) >> 10)
                    * (((v_x1_u32r * ((int32_t) inst->H3)) >> 11)
                            + ((int32_t) 32768))) >> 10) + ((int32_t) 2097152))
                    * ((int32_t) inst->H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t) inst->H1)) >> 4));

    v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
    v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;

    float h = (v_x1_u32r >> 12);
    return (h / 1024.0) / 100.0;
}

double bme280_get_altitude(double p, double p0)
{
    return 44330 * (1 - pow((p / p0), (1 / 5.255)));
}
