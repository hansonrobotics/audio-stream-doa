#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_afe_sr_models.h"
#include "esp_board_driver.h"
#include "ringbuf.h"
#include "nvs_flash.h"

#include "tcp_client.h"

static char* TAG = "AFE_MAIN";

static volatile int feed_flag = 0;
static volatile int fetch_flag = 0;

static esp_afe_sr_iface_t* afe_handle = NULL;

static esp_afe_sr_data_t* afe_data_0 = NULL;
static esp_afe_sr_data_t* afe_data_1 = NULL;

static tcp_client_t* tcp_client_0;
static tcp_client_t* tcp_client_1;
static tcp_client_t* tcp_client_2;

typedef struct {
    bool vad;
    int32_t loudness;
    float vol_db;
} mic_audio_desc_t;

typedef struct {
    mic_audio_desc_t mic_0;
    mic_audio_desc_t mic_1;
} tcp_packet_t;

int32_t get_audio_buffer_rms(int16_t* buff, uint16_t num_samples) {
    int64_t sum_of_squares = 0;
    for (uint16_t i = 0; i < num_samples; i++) {
        sum_of_squares += ((int64_t)buff[i] * (int64_t)buff[i]);
    }

    int64_t mean_of_squares = sum_of_squares / num_samples;
    int32_t rms = (int32_t)sqrt(mean_of_squares);
    return rms;
}

void feedTask(void* arg){
    int feed_num_frames = afe_handle->get_feed_chunksize(afe_data_0); // samples per channel
    int feed_num_channels = esp_get_feed_channel(); // number of channels
    ESP_LOGI(TAG, "feed_num_frames: %d, feed_num_channels: %d", feed_num_frames, feed_num_channels);

    // I2S0 stereo: each sample is 32-bit, so the total rx buffer size is feed_num_frames * feed_num_channels * (32/8)
    int32_t* feed_buff_tot = malloc(feed_num_frames * feed_num_channels * sizeof(int32_t));
    assert(feed_buff_tot);
    // buffers to separate I2S0 L & R channels
    int32_t* feed_buff_0 = malloc(feed_num_frames * sizeof(int32_t));
    assert(feed_buff_0);
    int32_t* feed_buff_1 = malloc(feed_num_frames * sizeof(int32_t));
    assert(feed_buff_1);

    ESP_LOGI(TAG, "i2s feed buffers initialized");
    fetch_flag = 1;

    while (feed_flag) {
        esp_get_stereo_mic_data(feed_buff_tot, feed_num_frames * feed_num_channels * sizeof(int32_t));
        for (int i = 0; i < feed_num_frames; i++) {
          feed_buff_0[i] = feed_buff_tot[i * 2];
          feed_buff_1[i] = feed_buff_tot[i * 2 + 1];
        }

        afe_handle->feed(afe_data_0, (int16_t*)feed_buff_0);
        afe_handle->feed(afe_data_1, (int16_t*)feed_buff_1);
    }

    if (feed_buff_tot) {
        free(feed_buff_tot);
        feed_buff_tot = NULL;
    }
    if(feed_buff_0){
        free(feed_buff_0);
        feed_buff_0 = NULL;
    }
    if(feed_buff_1){
        free(feed_buff_1);
        feed_buff_1 = NULL;
    }

    fetch_flag = 0;
    vTaskDelete(NULL);
}

void fetchTask(void* arg){
    bool tcp_flag = false;
    bool tcp_ret = true;
    tcp_packet_t packet;

    bool tcp_raw_audio_flag = false;
    int64_t audio_send_start_time = 0;
    int64_t current_time = 0;
    int64_t TOTAL_AUDIO_SEND_TIME = 5000000;

    while (fetch_flag) {
        afe_fetch_result_t* res_0 = afe_handle->fetch(afe_data_0);
        afe_fetch_result_t* res_1 = afe_handle->fetch(afe_data_1);

        if (res_0 && res_0->ret_value != ESP_FAIL && res_1 && res_1->ret_value != ESP_FAIL){
            if(res_0->vad_state == 1 || res_1->vad_state == 1){
                ESP_LOGI(TAG, "vad true, tcp send");

                packet.mic_0.vad = res_0->vad_state;
                packet.mic_0.loudness = get_audio_buffer_rms(res_0->data, res_0->data_size / sizeof(int16_t));
                packet.mic_0.vol_db = res_0->data_volume;
                
                packet.mic_1.vad = res_1->vad_state;
                packet.mic_1.loudness = get_audio_buffer_rms(res_1->data, res_1->data_size / sizeof(int16_t));
                packet.mic_1.vol_db = res_1->data_volume;

                tcp_ret &= tcp_client_send(tcp_client_0, (uint8_t*)&packet, sizeof(tcp_packet_t));

                if(!tcp_raw_audio_flag){
                    tcp_raw_audio_flag = true;
                    audio_send_start_time = esp_timer_get_time();
                }
            }

            if(tcp_raw_audio_flag){
                current_time = esp_timer_get_time();
                if(current_time - audio_send_start_time >= TOTAL_AUDIO_SEND_TIME){
                    tcp_raw_audio_flag = false;
                    const char* eot_marker = "EndOfTransmission";
                    tcp_ret &= tcp_client_send(tcp_client_1, (uint8_t*)eot_marker, strlen(eot_marker));
                    tcp_ret &= tcp_client_send(tcp_client_2, (uint8_t*)eot_marker, strlen(eot_marker));
                } else {
                    tcp_ret &= tcp_client_send(tcp_client_1, (uint8_t*)res_0->data, res_0->data_size);
                    tcp_ret &= tcp_client_send(tcp_client_2, (uint8_t*)res_1->data, res_1->data_size);                
                }
            }

            if(!tcp_ret){
                ESP_LOGE(TAG, "TCP send error, stopping application");
                feed_flag = 0;
                fetch_flag = 0;
                break;
            }
        }
    }

    vTaskDelete(NULL);
}

void app_main(){
    ESP_LOGI(TAG, "main start");
    //Initialize NVS for WiFi
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize I2S and WiFi
    ESP_ERROR_CHECK(esp_i2s_init());
    ESP_ERROR_CHECK(esp_wifi_sta_init());

    // Initialize TCP clients
    tcp_client_0 = tcp_client_init(CONFIG_TCP_SERVER_IP, 5000);
    if(tcp_client_0 == NULL){
        ESP_LOGE(TAG, "TCP client 0 initialization failed");
        return;
    }
    tcp_client_1 = tcp_client_init(CONFIG_TCP_SERVER_IP, 5001);
    if(tcp_client_1 == NULL){
        ESP_LOGE(TAG, "TCP client 1 initialization failed");
        return;
    }
    tcp_client_2 = tcp_client_init(CONFIG_TCP_SERVER_IP, 5002);
    if(tcp_client_1 == NULL){
        ESP_LOGE(TAG, "TCP client 2 initialization failed");
        return;
    }

    // Config and initialize AFE Handles
    afe_config_t afe_config = AFE_CONFIG_DEFAULT();
    afe_config.aec_init = false;
    afe_config.se_init = true;
    afe_config.vad_init = true;
    afe_config.wakenet_init = false;
    afe_config.voice_communication_init = false;
    afe_config.voice_communication_agc_init = false;
    afe_config.vad_mode = VAD_MODE_3;
    afe_config.afe_mode = SR_MODE_LOW_COST;
    afe_config.memory_alloc_mode = AFE_MEMORY_ALLOC_MORE_PSRAM;
    afe_config.afe_linear_gain = 1.0;
    afe_config.agc_mode = AFE_MN_PEAK_NO_AGC;
    afe_config.afe_ns_mode = NS_MODE_SSP;
    afe_config.afe_ns_model_name = NULL;
    afe_config.pcm_config.total_ch_num = 2;
    afe_config.pcm_config.mic_num = 1;
    afe_config.pcm_config.ref_num = 1;
    afe_config.pcm_config.sample_rate = 16000;

    afe_handle = (esp_afe_sr_iface_t*)&ESP_AFE_SR_HANDLE;
    afe_data_0 = afe_handle->create_from_config(&afe_config);
    if (afe_data_0 == NULL) {
        ESP_LOGI(TAG, "create afe_0 fail!");
        return;
    } else {
        ESP_LOGI(TAG, "AFE_0 created");
    }

    afe_data_1 = afe_handle->create_from_config(&afe_config);
    if (afe_data_1 == NULL) {
        ESP_LOGI(TAG, "create afe_1 fail!");
        return;
    } else {
        ESP_LOGI(TAG, "AFE_1 created");
    }

    // Begin audio feed and fetch tasks
    feed_flag = 1;
    xTaskCreatePinnedToCore(&feedTask, "feed", 8 * 1024, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(&fetchTask, "fetch", 8 * 1024, NULL, 5, NULL, 1);

    /*
    // You can call afe_handle->destroy to destroy AFE.
    afe_handle_0->destroy(afe_data_0);
    afe_data_0 = NULL;
    ESP_LOGI(TAG, "destroy_0 successful");

    afe_handle_1->destroy(afe_data_1);
    afe_data_1 = NULL;
    ESP_LOGI(TAG, "destroy_1 successful");
    */
}