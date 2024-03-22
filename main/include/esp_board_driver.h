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

#include <stdbool.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/**
 * @brief Add dev board pin defination and check target.
 * 
 */

#include "esp_board_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MIC_NUM_0 = 0,
    MIC_NUM_1,
} mic_num_t;


/**
 * @brief Power module of dev board. This can be expanded in the future.
 * 
 */
typedef enum {
    POWER_MODULE_LCD = 1,       /*!< LCD power control */
    POWER_MODULE_AUDIO,         /*!< Audio PA power control */
    POWER_MODULE_ALL = 0xff,    /*!< All module power control */
} power_module_t;

/**
 * @brief Deinit SD card
 * 
 * @param mount_point Path where partition was registered (e.g. "/sdcard")
 * @return 
 *    - ESP_OK: Success
 *    - Others: Fail
 */
esp_err_t esp_sdcard_deinit(char *mount_point);

/**
 * @brief Init SD crad
 * 
 * @param mount_point Path where partition should be registered (e.g. "/sdcard")
 * @param max_files Maximum number of files which can be open at the same time
 * @return
 *    - ESP_OK                  Success
 *    - ESP_ERR_INVALID_STATE   If esp_vfs_fat_register was already called
 *    - ESP_ERR_NOT_SUPPORTED   If dev board not has SDMMC/SDSPI
 *    - ESP_ERR_NO_MEM          If not enough memory or too many VFSes already registered
 *    - Others                  Fail
 */
esp_err_t esp_sdcard_init(char *mount_point, size_t max_files);

/**
 * @brief Special config for dev board
 * 
 * @return
 *    - ESP_OK: Success
 *    - Others: Fail
 */
esp_err_t esp_i2s_init();

/**
 * @brief Get the record pcm data.
 */
esp_err_t esp_get_stereo_mic_data(int32_t* buffer, int buffer_len);

int esp_get_feed_channel(void);

esp_err_t esp_wifi_sta_init();

#ifdef __cplusplus
}
#endif
