// Electronic Design Lab EE 344 IIT Bombay
// Team Tue: 23 
// Authors: Sankalp Bhamare, Rohan Rajesh Kalbag, Jujhaar Singh

#include "i2c_conn.h"

// drivers for I2C communication protocol

static esp_err_t i2c_master_init(void)
{
  int i2c_master_port = I2C_MASTER_NUM;
  
  // assign the parameters for I2C communication protocol
  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_MASTER_SDA_IO,
      .scl_io_num = I2C_MASTER_SCL_IO,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = I2C_MASTER_FREQ_HZ,
  };

  i2c_param_config(i2c_master_port, &conf);

  return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void init_i2c()
{
  // initialize the I2C master
  ESP_ERROR_CHECK(i2c_master_init());
  ESP_LOGI("I2C", "I2C initialized successfully");
}

void denit_i2c()
{
  // deinitialize the I2C master
  ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
  ESP_LOGI("I2C", "I2C de-initialized successfully");
}
