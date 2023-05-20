// Electronic Design Lab EE 344 IIT Bombay
// Team Tue: 23 
// Authors: Sankalp Bhamare, Rohan Rajesh Kalbag, Jujhaar Singh

#include "driver/gpio.h"
#define BUZZER_PIN GPIO_NUM_12

void init_buzzer(void);
void buzzer_toggle(bool power);