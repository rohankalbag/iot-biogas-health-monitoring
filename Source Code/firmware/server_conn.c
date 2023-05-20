// Electronic Design Lab EE 344 IIT Bombay
// Team Tue: 23 
// Authors: Sankalp Bhamare, Rohan Rajesh Kalbag, Jujhaar Singh
// drivers to establish connections with web server

#include "server_conn.h"

static EventGroupHandle_t s_wifi_event_group;

static const char *TAG = "wifi station";

static int s_retry_num = 0;

static const char *REQUEST = "GET " WEB_PATH " HTTP/1.0\r\n"
                             "Host: " WEB_SERVER ":" WEB_PORT "\r\n"
                             "User-Agent: esp-idf/1.0 esp32\r\n"
                             "\r\n";

esp_err_t raw_http_send(char *req, char *recv_buf, int recv_buff_size)
{
  const struct addrinfo hints = {
      .ai_family = AF_INET,
      .ai_socktype = SOCK_STREAM,
  };
  struct addrinfo *res;
  struct in_addr *addr;
  int s, r;

  for (int i = 0; i < MAX_HTTP_RETRIES; i++)
  {
    int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

    if (err != 0 || res == NULL)
    {
      // dns lookup failed
      ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      continue;
    }

    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;

    s = socket(res->ai_family, res->ai_socktype, 0);
    if (s < 0)
    {
      // socket error
      ESP_LOGE(TAG, "Failed to allocate socket.");
      freeaddrinfo(res);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      continue;
    }

    if (connect(s, res->ai_addr, res->ai_addrlen) != 0)
    {
      // socket connection failure
      ESP_LOGE(TAG, "socket connect failed errno=%d", errno);
      close(s);
      freeaddrinfo(res);
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      continue;
    }

    freeaddrinfo(res);
    if (write(s, req, strlen(req)) < 0)
    {
      // send to socket failed
      ESP_LOGE(TAG, "socket send failed");
      close(s);
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      continue;
    }

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                   sizeof(receiving_timeout)) < 0)
    {
      // failure to set timeout for receiving socket
      ESP_LOGE(TAG, "failed to set socket receiving timeout");
      close(s);
      vTaskDelay(4000 / portTICK_PERIOD_MS);
      continue;
    }

    close(s);
    return ESP_OK;

    // read the http response obtained from the socket
    do
    {
      bzero(recv_buf, recv_buff_size);
      r = read(s, recv_buf, recv_buff_size - 1);
      for (int i = 0; i < r; i++)
      {
        putchar(recv_buf[i]);
      }
    } while (r > 0);
  }
  return ESP_MAXIMUM_RETRY;
}

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data)
{
  if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
  {
    esp_wifi_connect();
  }
  else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
  {
    if (s_retry_num < ESP_MAXIMUM_RETRY)
    {
      // retry to connect if less than max retry attempts
      esp_wifi_connect();
      s_retry_num++;
      ESP_LOGI(TAG, "retry to connect to the AP");
    }
    else
    {
      xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
    }
    // if exceeded max retry attempts then disconnect
    ESP_LOGI(TAG, "connect to the AP fail");
  }
  else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
  {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
    s_retry_num = 0;
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

esp_err_t wifi_init_sta(void)
{
  s_wifi_event_group = xEventGroupCreate();

  ESP_ERROR_CHECK(esp_netif_init());

  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  esp_event_handler_instance_t instance_any_id;
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                      ESP_EVENT_ANY_ID,
                                                      &event_handler,
                                                      NULL,
                                                      &instance_any_id));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                      IP_EVENT_STA_GOT_IP,
                                                      &event_handler,
                                                      NULL,
                                                      &instance_got_ip));

  // use the hotspot connection credentials
  wifi_config_t wifi_config = {
      .sta = {
          .ssid = ESP_WIFI_SSID,
          .password = ESP_WIFI_PASS,
          .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
          .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
      },
  };

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "wifi_init_sta finished.");

  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                         WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                         pdFALSE,
                                         pdFALSE,
                                         portMAX_DELAY);

  if (bits & WIFI_CONNECTED_BIT)
  {
    ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
             ESP_WIFI_SSID, ESP_WIFI_PASS);

    return ESP_OK;
  }
  else if (bits & WIFI_FAIL_BIT)
  {
    ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
             ESP_WIFI_SSID, ESP_WIFI_PASS);
    return ESP_ERR_WIFI_CONN;
  }
  else
  {
    ESP_LOGE(TAG, "UNEXPECTED EVENT");
    return ESP_FAIL;
  }
}

esp_err_t http_post_task(int co2_ppm, float humidity)
{
  // function to make the https post request to the webserver
  // this function sends the sensor data to the webserver
  char recv_buf[64];
  char req[500] = {'\0'};
  char req_start[] = "POST /data HTTP/1.1\nHost: 192.168.255.135:5000 \nContent-Type: application/json\n";
  char req_body[100];
  sprintf(req_body, "co2:%d\nhumidity:%.2f\n", co2_ppm, humidity);

  strcat(req, req_start);

  strcat(req, req_body);
  printf("%s\n", req);
  ESP_ERROR_CHECK(raw_http_send(req, recv_buf, 64));
  return ESP_OK;
}