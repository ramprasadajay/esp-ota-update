#include <stdio.h>
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"

static const char *OTA_TAG = "[OTA]";
#define OTA_URL "https://drive.google.com/u/2/uc?id=15mYOdzJISTZx_sSeLuKgyUADZLG00VD8&export=download"
TaskHandle_t ota_task_handle;

/*
 * Attach the certificate as below. This is the convention to use as follows
 * this starts with _binary_<file_name>_start. The file should be give by replacing
 * the special characters with '_'.
 *
 * For Example: if file name is google.crt then "_binary_google_crt_start"
 */
extern const uint8_t server_cert_pem_start[] asm("_binary_google_crt_start");

esp_err_t client_event_handler(esp_http_client_event_t *evt)
{
    return ESP_OK;
}

/*
 * @note This function validates whether the incoming version is not already installed on this device
 * @param esp_app_desc_t *incoming_ota_desc
 * @returns
 * ESP_OK if incoming version is different from current version
 * ESP_FAIL if incoming version is same as current version
 */
esp_err_t validate_image_header(esp_app_desc_t *incoming_ota_desc)
{
    const esp_partition_t *running_partition = esp_ota_get_running_partition();
    esp_app_desc_t running_partition_description;
    esp_ota_get_partition_description(running_partition, &running_partition_description);

    ESP_LOGI(OTA_TAG, "Current Version: %s\n", running_partition_description.version);
    ESP_LOGI(OTA_TAG, "Incoming Version: %s\n", incoming_ota_desc->version);

    if (strcmp(running_partition_description.version, incoming_ota_desc->version) == 0)
    {
        ESP_LOGW(OTA_TAG, "The device is upto date. Aborting OTA Update.");
        return ESP_FAIL;
    }
    return ESP_OK;
}

void run_ota(void *pvParams)
{
    ESP_LOGI(OTA_TAG, "OTA Initiated");

    esp_http_client_config_t clientConfig = {
        .url = OTA_URL,
        .event_handler = client_event_handler,
        .cert_pem = (char *)server_cert_pem_start,
    };

    esp_https_ota_config_t ota_config = {
        .http_config = &clientConfig,
    };

    esp_https_ota_handle_t ota_handle = NULL;

    if (esp_https_ota_begin(&ota_config, &ota_handle) != ESP_OK)
    {
        ESP_LOGE(OTA_TAG, "esp_https_ota_begin failed");
        // Delete run_ota Task
        vTaskDelete(ota_task_handle);
    }
    ESP_LOGW(OTA_TAG, "OTA begin.");

    esp_app_desc_t incoming_ota_desc;
    if (esp_https_ota_get_img_desc(ota_handle, &incoming_ota_desc) != ESP_OK)
    {
        ESP_LOGE(OTA_TAG, "esp_https_ota_get_img_desc failed");
        // stop the OTA
        esp_https_ota_finish(ota_handle);
        // Delete run_ota Task
        vTaskDelete(ota_task_handle);
    }

    if (validate_image_header(&incoming_ota_desc) != ESP_OK)
    {
        ESP_LOGE(OTA_TAG, "validate_image_header failed");
        // stop the OTA
        esp_https_ota_finish(ota_handle);
        // Delete run_ota Task
        vTaskDelete(ota_task_handle);
    }

    while (1)
    {
        esp_err_t ota_result = esp_https_ota_perform(ota_handle);
        if (ota_result != ESP_ERR_HTTPS_OTA_IN_PROGRESS)
            break;
    }

    if (esp_https_ota_finish(ota_handle) != ESP_OK)
    {
        ESP_LOGE(OTA_TAG, "esp_https_ota_finish failed");
        // Delete run_ota Task
        vTaskDelete(ota_task_handle);
    }
    else
    {
        ESP_LOGI(OTA_TAG, "OTA Update is Success. Restarting the device in 5 seconds");
        printf("restarting in 5 seconds\n");
        vTaskDelay(pdMS_TO_TICKS(5000));
        esp_restart();
    }
    ESP_LOGE(OTA_TAG, "Failed to perform OTA Update");
    // Delete run_ota Task
    vTaskDelete(ota_task_handle);
}
void app_main(void)
{
    xTaskCreatePinnedToCore(&run_ota, "run_ota", 1028 * 8, NULL, 2, &ota_task_handle, tskNO_AFFINITY);
}