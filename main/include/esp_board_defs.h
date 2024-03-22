/**
 * 
 * @copyright Copyright 2021 Espressif Systems (Shanghai) Co. Ltd.
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *               http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */
#pragma once

#include "driver/gpio.h"
#include "esp_idf_version.h"

/**
 * @brief ESP32-S3 Custom Board I2C GPIO defineation
 * 
 */
#define FUNC_I2C_EN     (0)
#define GPIO_I2C_SCL    (GPIO_NUM_NC)
#define GPIO_I2C_SDA    (GPIO_NUM_NC)

/**
 * @brief ESP32-S3 Custom Board SDMMC GPIO defination
 * 
 * @note Only avaliable when PMOD connected
 */
#define FUNC_SDMMC_EN   (1)
#define SDMMC_BUS_WIDTH (1)
#define GPIO_SDMMC_CLK  (GPIO_NUM_39)
#define GPIO_SDMMC_CMD  (GPIO_NUM_38)
#define GPIO_SDMMC_D0   (GPIO_NUM_40)
#define GPIO_SDMMC_D1   (GPIO_NUM_4)
#define GPIO_SDMMC_D2   (GPIO_NUM_12)
#define GPIO_SDMMC_D3   (GPIO_NUM_13)
#define GPIO_SDMMC_DET  (GPIO_NUM_NC)

/**
 * @brief ESP32-S3 Custom Board SDSPI GPIO definationv
 * 
 */
#define FUNC_SDSPI_EN       (0)
#define SDSPI_HOST          (SPI2_HOST)
#define GPIO_SDSPI_CS       (GPIO_NUM_NC)
#define GPIO_SDSPI_SCLK     (GPIO_NUM_NC)
#define GPIO_SDSPI_MISO     (GPIO_NUM_NC)
#define GPIO_SDSPI_MOSI     (GPIO_NUM_NC)

/**
 * @brief ESP32-S3 Custom Board I2S0 GPIO defination
 * 
 */
#define FUNC_I2S0_EN         (1)
#define GPIO_I2S0_LRCK       (GPIO_NUM_47)
#define GPIO_I2S0_SCLK       (GPIO_NUM_48)
#define GPIO_I2S0_SDIN       (GPIO_NUM_21)
#define GPIO_I2S0_MCLK       (GPIO_NUM_NC)
#define GPIO_I2S0_DOUT       (GPIO_NUM_NC)

/**
 * @brief ESP32-S3 Custom Board I2S1 GPIO defination
 * 
 */
#define FUNC_I2S_EN         (0)
#define GPIO_I2S_LRCK       (GPIO_NUM_41)
#define GPIO_I2S_SCLK       (GPIO_NUM_42)
#define GPIO_I2S_SDIN       (GPIO_NUM_2)
#define GPIO_I2S_MCLK       (GPIO_NUM_NC)
#define GPIO_I2S_DOUT       (GPIO_NUM_NC)

/**
 * @brief ESP32-S3 Custom Board power control IO
 * 
 * @note Some power control pins might not be listed yet
 * 
 */
#define FUNC_PWR_CTRL       (0)
#define GPIO_PWR_CTRL       (GPIO_NUM_NC)
#define GPIO_PWR_ON_LEVEL   (1)

#define I2S0_CONFIG_DEFAULT(sample_rate, channel_fmt, bits_per_chan) { \
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate), \
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(bits_per_chan, channel_fmt), \
        .gpio_cfg = { \
            .mclk = GPIO_I2S0_MCLK, \
            .bclk = GPIO_I2S0_SCLK, \
            .ws   = GPIO_I2S0_LRCK, \
            .dout = GPIO_I2S0_DOUT, \
            .din  = GPIO_I2S0_SDIN, \
            .invert_flags = { \
                .mclk_inv = false, \
                .bclk_inv = false, \
                .ws_inv   = false, \
            }, \
        }, \
    }

#define I2S_CONFIG_DEFAULT(sample_rate, channel_fmt, bits_per_chan) { \
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(sample_rate), \
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(bits_per_chan, channel_fmt), \
        .gpio_cfg = { \
            .mclk = GPIO_I2S_MCLK, \
            .bclk = GPIO_I2S_SCLK, \
            .ws   = GPIO_I2S_LRCK, \
            .dout = GPIO_I2S_DOUT, \
            .din  = GPIO_I2S_SDIN, \
            .invert_flags = { \
                .mclk_inv = false, \
                .bclk_inv = false, \
                .ws_inv   = false, \
            }, \
        }, \
    }

