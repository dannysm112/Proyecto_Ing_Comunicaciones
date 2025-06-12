// Implementacion real BLE con ESP-IDF
// y una version funcional simplificada para pruebas

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gattc_api.h"
#include "esp_gap_ble_api.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_system.h"

#define TAG "BLE-WIFI"

#define GATTC_APP_ID 0
#define AUTH_CHAR_UUID 0xABCD
#define SSID_CHAR_UUID 0xABCE
#define PASS_CHAR_UUID 0xABCF
#define SERVICE_UUID 0x1234

static esp_gatt_if_t gattc_if;
static uint16_t conn_id;
static esp_bd_addr_t server_bda;
static bool is_connected = false;

static void start_wifi(const char* ssid, const char* password) {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_config_t wifi_config = {};
    strcpy((char*)wifi_config.sta.ssid, ssid);
    strcpy((char*)wifi_config.sta.password, password);

    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
    esp_wifi_connect();
}

static void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if_cb, esp_ble_gattc_cb_param_t* param) {
    switch (event) {
        case ESP_GATTC_CONNECT_EVT:
            ESP_LOGI(TAG, "Connected to server.");
            is_connected = true;
            conn_id = param->connect.conn_id;
            memcpy(server_bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            break;

        case ESP_GATTC_SEARCH_CMPL_EVT:
            ESP_LOGI(TAG, "Service search completed.");
            // Aquí se empieza a interactuar con las características BLE
            break;

        case ESP_GATTC_READ_CHAR_EVT:
            ESP_LOGI(TAG, "Read UUID 0x%04x: %s", param->read.handle, param->read.value);
            break;

        case ESP_GATTC_WRITE_CHAR_EVT:
            ESP_LOGI(TAG, "Wrote to characteristic successfully.");
            break;

        case ESP_GATTC_DISCONNECT_EVT:
            ESP_LOGI(TAG, "Disconnected from server.");
            is_connected = false;
            break;

        default:
            break;
    }
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param) {
    if (event == ESP_GAP_BLE_SCAN_RESULT_EVT) {
        esp_ble_gap_cb_param_t::ble_scan_result_evt_param res = param->scan_rst;
        if (res.search_evt == ESP_GAP_SEARCH_INQ_RES && strstr((char*)res.ble_adv, "ServidorWiFi-BLE")) {
            ESP_LOGI(TAG, "ServidorWiFi-BLE encontrado. Conectando...");
            esp_ble_gap_stop_scanning();
            esp_ble_gattc_open(gattc_if, res.bda, true);
        }
    }
}

void app_main(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        ESP_ERROR_CHECK(nvs_flash_erase());
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    esp_bluedroid_init();
    esp_bluedroid_enable();

    esp_ble_gap_register_callback(gap_event_handler);
    esp_ble_gattc_register_callback(gattc_event_handler);
    esp_ble_gattc_app_register(GATTC_APP_ID);

    esp_ble_gap_set_scan_params(&((esp_ble_scan_params_t){
        .scan_type = BLE_SCAN_TYPE_ACTIVE,
        .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
        .scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
        .scan_interval = 0x50,
        .scan_window = 0x30
    }));
}


// Codigo funcional simplificado para pruebas (flujo lógico sin BLE real)
void app_main(void) {
    printf("Inicializando sistema...\n");

    // Simulación de BLE
    printf("Buscando servidor BLE...\n");
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    printf("Servidor encontrado. Enviando token...\n");
    bool auth_ok = true; // Simular autenticación exitosa

    if (auth_ok) {
        char ssid[32] = "RedEjemplo";
        char password[64] = "contrasena123";

        printf("Credenciales recibidas: SSID: %s, PASS: %s\n", ssid, password);

        // Simular almacenamiento en NVS
        printf("Guardando credenciales en NVS...\n");

        // Simular conexión WiFi
        printf("Conectando a WiFi...\n");
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        printf("Conexión exitosa. IP asignada: 192.168.0.123\n");
    } else {
        printf("Error de autenticación BLE.\n");
    }
}