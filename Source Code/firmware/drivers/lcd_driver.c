// Electronic Design Lab EE 344 IIT Bombay
// Team Tue: 23 
// Authors: Sankalp Bhamare, Rohan Rajesh Kalbag, Jujhaar Singh

// drivers for the LCD 

#include "lcd_driver.h"

void init_lcd(){
  // initialize the LCD 
  raw_init_lcd(2);
  begin_lcd();
}

void render_screen(int co2_ppm, double humidity){
	// display the sensor readings on the LCD screen
	char cmd[300];
	setCursor_lcd(0,0);
	sprintf(cmd, "CO2(ppm): %d  ",co2_ppm);
	printstr_lcd(cmd);
	setCursor_lcd(0,1);
	sprintf(cmd, "Humidity: %.2f%%   ", (float)humidity);
	printstr_lcd(cmd);
}