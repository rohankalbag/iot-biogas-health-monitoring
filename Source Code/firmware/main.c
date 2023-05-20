// Electronic Design Lab EE 344 IIT Bombay
// Team Tue: 23 
// Authors: Sankalp Bhamare, Rohan Rajesh Kalbag, Jujhaar Singh

#include "server_conn.h"
#include "drivers/i2c_conn.h"

// sensor threshold value, sensor rings after co2 value exceeds threshold

#define SENSOR_THRESHOLD 3000

static const char *TAG = "BioGAS";

void app_main(void)
{
    esp_err_t ret;
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_LOGI(TAG, "SYSTEM BOOTED");

    ESP_LOGI(TAG, "I2C init start");
    init_i2c();
    ESP_LOGI(TAG, "Connecting LCD....");
    init_lcd();
    ESP_LOGI(TAG, "LCD CONNECTED");
    begin_lcd();

    // Basic User Interface on the LCD

    printstr_lcd("EDL P-18");
    setCursor_lcd(0, 1);
    printstr_lcd("TUE-23");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    setCursor_lcd(0, 1);
    printstr_lcd("Sankalp");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    setCursor_lcd(0, 1);
    printstr_lcd("Jujhaar");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    setCursor_lcd(0, 1);
    printstr_lcd("Rohan   ");
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    setCursor_lcd(0, 1);
    printstr_lcd("WiFi Init..");

    ESP_LOGI(TAG, "initializing WiFi module.");
    ret = wifi_init_sta();
    
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "WiFi initialization failed!");
        for (;;)
        {}
    }

    // After WiFi connection is established
    setCursor_lcd(0, 1);
    printstr_lcd("WiFi Connected");

    vTaskDelay(4000 / portTICK_PERIOD_MS);

    setCursor_lcd(0, 1);
    printstr_lcd("System Start");

    vTaskDelay(4000 / portTICK_PERIOD_MS);
    ESP_LOGI(TAG, "WiFi module initialized.");

    // uncomment below line if you want to use LCD backlight
    // backlight_lcd(); 

    init_buzzer(); // initialize buzzer
    for (;;)
    {
        // read co2 sensor data
        int co2_ppm = read_co2_ppm();
        // conditional to check if the sensor reading is under threshold
        if (co2_ppm > SENSOR_THRESHOLD)
        {
            buzzer_toggle(true);
        }
        else
        {
            buzzer_toggle(false);
        }

        double temp, rel_humidity;
        // read the dht sensor values
        read_dht(&rel_humidity, &temp);
        // send sensor readings to web server
        render_screen(co2_ppm, rel_humidity);
        http_post_task(co2_ppm, rel_humidity);
        ESP_LOGI(TAG, "HTTP DATA PUSHED");
    }
    denit_i2c();
    vTaskDelay(10);
}