#include "wifi_connect.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/ip_addr.h"

static const char *TAG = "WIFI";

bool wifi_connect(const char *ssid, const char *pass) {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    
    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK
        },
    };
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password));
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "Esperando conexión Wi-Fi...");
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        ESP_LOGI(TAG, "Conectado a AP: %s", ap_info.ssid);
        return true;
    } else {
        ESP_LOGE(TAG, "No se pudo conectar al AP.");
        return false;
    }
}