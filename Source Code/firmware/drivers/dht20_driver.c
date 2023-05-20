// Electronic Design Lab EE 344 IIT Bombay
// Team Tue: 23 
// Authors: Sankalp Bhamare, Rohan Rajesh Kalbag, Jujhaar Singh

// drivers to read from the DHT10 humidity sensor

#include "dht20_driver.h"

void read_dht(double *hum_out, double *temp_out)
{
  uint8_t read_cmd[3] = {0xAC, 0x33, 0x00};

  uint8_t i2c_buffer_out[9];
  char buffer_out[7];
  ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, DHT_ADDRESS, read_cmd, 3, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
  while(true) {
    ESP_ERROR_CHECK(i2c_master_read_from_device(I2C_MASTER_NUM, DHT_ADDRESS, i2c_buffer_out, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
    if (i2c_buffer_out[0] & 0x80) break;
  }
  ESP_ERROR_CHECK(i2c_master_read_from_device(I2C_MASTER_NUM, DHT_ADDRESS, i2c_buffer_out, 7, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));

  for (int i = 0; i < 7; i++)
  {
    buffer_out[i] = i2c_buffer_out[i];
  }

  char crc = Calc_CRC8(buffer_out, 6);
  if (crc == buffer_out[6]) {
    ESP_LOGI("I2C", "CRC matches, value: %x\n", buffer_out[6]);
  } else {
    ESP_LOGI("I2C", "CRC MISMATCH, value: %x\n", buffer_out[6]);
  }
  
  uint32_t raw_humidity = buffer_out[1];
  raw_humidity = (raw_humidity<<8) + buffer_out[2];
  raw_humidity = (raw_humidity<<4) +  (buffer_out[3] >> 4 );
  double humidity = raw_humidity/1048576.0 * 100;
  
  uint32_t raw_temp_c = buffer_out[3] & (0x0F);
  raw_temp_c = (raw_temp_c << 8) + buffer_out[4];
  raw_temp_c = (raw_temp_c << 8) + buffer_out[5];
  double temp_c = raw_temp_c/1048576.0*200.0 - 50.0; 

  ESP_LOGI("DHT20", "Relative Humidity[%%]: %.2f\n", humidity);
  ESP_LOGI("DHT20", "Temp[C]: %.2f\n", temp_c);
  *hum_out = humidity;
  *temp_out = temp_c;
}