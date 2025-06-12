#include <stdio.h>
#include "ble_client.h"
#include "wifi_connect.h"
#include "nvs_flash.h"
#include "esp_log.h"

static const char *TAG = "MAIN";

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Inicializando BLE para recuperación de credenciales...");
    char ssid[32] = {0};
    char password[64] = {0};

    bool cred_ok = ble_get_credentials(ssid, password);
    if (!cred_ok) {
        ESP_LOGE(TAG, "No se pudieron obtener credenciales por BLE.");
        return;
    }

    ESP_LOGI(TAG, "Credenciales recibidas. SSID: %s", ssid);

    ESP_LOGI(TAG, "Iniciando conexión Wi-Fi...");
    if (wifi_connect(ssid, password)) {
        ESP_LOGI(TAG, "Conexión Wi-Fi exitosa.");
    } else {
        ESP_LOGE(TAG, "Fallo en la conexión Wi-Fi.");
    }
}