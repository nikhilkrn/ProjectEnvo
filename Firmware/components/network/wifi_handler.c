

#include "wifi_hnadler.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"


static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{

    if (event_base == WIFI_EVENT || event_base == IP_EVENT)
    {
        ESP_LOGI("EVENT", "Base=%s ID=%ld",
         event_base,
         event_id);
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:
            esp_wifi_connect();
            ESP_LOGI("[WIFI]","Connecting ....");
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
            {
            wifi_event_sta_disconnected_t *event =
                (wifi_event_sta_disconnected_t *)event_data;

            ESP_LOGW("TAG",
                        "Disconnected. Reason = %d",
                        event->reason);

            esp_wifi_connect();   // Retry
            break;
            }

        case IP_EVENT_STA_GOT_IP:
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            ESP_LOGI("[IP]", "GOT IP: "IPSTR , IP2STR(&event->ip_info.ip));

        default:
            break;
        }
    }
}

void Wifi_init()
{

    
    // Initialize the TCP/IP
    ESP_ERROR_CHECK(esp_netif_init());

    // Create a Event Loop to handle WiFi Event
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Wifi configuration
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_netif_create_default_wifi_sta();
    // Register the Event here Wifi event is handling wifi related event transition and ip_got_ip is IP_EVENT just checking if got ip or not in the STA mode
    esp_event_handler_instance_t wifi_any_instance;
    esp_event_handler_instance_t ip_got_ip_instace;

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &wifi_any_instance));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &ip_got_ip_instace));

    wifi_config_t wifi_cfg = {
        .sta = {
            .ssid = "ShivaaHomes",
            .password = "Shivaa123",
            .threshold = {
                .authmode = WIFI_AUTH_WPA2_PSK
            },
        },
    .ap = {.ssid = "ProjectEnvo", 
        .password = "EnvoLabs123",
        .authmode = WIFI_AUTH_WPA2_PSK, .max_connection = 5, .channel = 1}
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI("[WIFI]", "Wifi Intialized");
}
