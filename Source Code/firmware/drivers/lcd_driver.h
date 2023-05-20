// Electronic Design Lab EE 344 IIT Bombay
// Team Tue: 23 
// Authors: Sankalp Bhamare, Rohan Rajesh Kalbag, Jujhaar Singh

#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#include "raw_lcd_driver.h"

void init_lcd();
void render_screen(int co2_ppm, double humidity);
#endif