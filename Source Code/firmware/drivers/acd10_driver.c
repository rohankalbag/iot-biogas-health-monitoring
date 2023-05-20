// Electronic Design Lab EE 344 IIT Bombay
// Team Tue: 23 
// Authors: Sankalp Bhamare, Rohan Rajesh Kalbag, Jujhaar Singh

// drivers for the acd10 co2 sensor

#include "acd10_driver.h"

int read_co2_ppm()
{ 
  // function to read the co2 sensor readings
  uint8_t read_cmd[2] = {0x03, 0x00};

  uint8_t i2c_buffer_out[9];
  char buffer_out[9];
  ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, ACD_ADDRESS, read_cmd, 2, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
  ESP_ERROR_CHECK(i2c_master_read_from_device(I2C_MASTER_NUM, ACD_ADDRESS, i2c_buffer_out, 9, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));

  for (int i = 0; i < 9; i++)
  {
    buffer_out[i] = i2c_buffer_out[i];
  }
  ESP_LOGI("ACD10", "\n[PPM3]\t[PPM2]\t[CRC1]\t[PPM1]\t[PPM0]\t[CRC2]\t[D1]\t[D2]\t[CRC3]\n%02X\t%02X\t%02X\t%02X\t%02X\t%02X\t%02X\t%02X\t%02X\t", buffer_out[0], buffer_out[1], buffer_out[2], buffer_out[3], buffer_out[4], buffer_out[5], buffer_out[6], buffer_out[7], buffer_out[8]);

  ESP_LOGI("ACD10", "\n[CRC1]\t[CRC2]\t[CRC3]\n%s\t%s\t%s", Calc_CRC8(buffer_out, 2) == buffer_out[2] ? "MATCH" : "FAIL", Calc_CRC8(buffer_out + 3, 2) == buffer_out[5] ? "MATCH" : "FAIL", Calc_CRC8(buffer_out + 6, 2) == buffer_out[8] ? "MATCH" : "FAIL");
  int co2_ppm = (((((int)buffer_out[0]) * 256 + buffer_out[1]) * 256 + buffer_out[3]) * 256) + buffer_out[4];
  ESP_LOGI("ACD10", "CO2[PPM]: %d\n", co2_ppm);
  return co2_ppm;
}