// Electronic Design Lab EE 344 IIT Bombay
// Team Tue: 23 
// Authors: Sankalp Bhamare, Rohan Rajesh Kalbag, Jujhaar Singh

// drivers to initialize and switch off and on the buzzer

#include "buzzer_driver.h"

void init_buzzer(void)
{
  gpio_reset_pin(BUZZER_PIN);
  /* Set the GPIO as a push/pull output */
  gpio_set_direction(BUZZER_PIN, GPIO_MODE_OUTPUT);
}

void buzzer_toggle(bool power)
{
  /* Set the GPIO level according to the state (LOW or HIGH)*/
  gpio_set_level(BUZZER_PIN, power);
}