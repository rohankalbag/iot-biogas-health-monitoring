// Electronic Design Lab EE 344 IIT Bombay
// Team Tue: 23 
// Authors: Sankalp Bhamare, Rohan Rajesh Kalbag, Jujhaar Singh

#ifndef RAW_LCD_DRIVER_H
#define RAW_LCD_DRIVER_H

#include "i2c_global.h"
#include <unistd.h>

#define LCD_ADDRESS 0x27 // I2C address of the LCD

// lcd driver specific parameters
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display_lcd entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display_lcd on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display_lcd/cursor_lcd shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight_lcd control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x4  // Enable bit
#define Rw 0x2  // Read/Write bit
#define Rs 0x1  // Register select bit

void raw_init_lcd(uint8_t lcd_rows);


void begin_lcd();

  /**
  * Remove all the characters currently shown. Next print/write operation will start
  * from the first position on LCD display_lcd.
  */
void clear_lcd();

/**
  * Next print/write operation will will start from the first position on the LCD display_lcd.
  */
void home_lcd();

  /**
  * Do not show any characters on the LCD display_lcd. Backlight state will remain unchanged.
  * Also all characters written on the display_lcd will return, when the display_lcd in enabled again.
  */
void noDisplay_lcd();

/**
  * Show the characters on the LCD display_lcd, this is the normal behaviour. This method should
  * only be used after noDisplay_lcd() has been used.
  */
void display_lcd();

/**
  * Do not blink_lcd the cursor_lcd indicator.
  */
void noBlink_lcd();

/**
  * Start blinking the cursor_lcd indicator.
  */
void blink_lcd();

/**
  * Do not show a cursor_lcd indicator.
  */
void noCursor_lcd();

/**
  * Show a cursor_lcd indicator, cursor_lcd can blink_lcd on not blink_lcd. Use the
  * methods blink_lcd() and noBlink_lcd() for changing cursor_lcd blink_lcd.
  */
void cursor_lcd();

void scrollDisplayLeft_lcd();
void scrollDisplayRight_lcd();
void printLeft();
void printRight_lcd();
void leftToRight_lcd();
void rightToLeft_lcd();
void shiftIncrement_lcd();
void shiftDecrement_lcd();
void noBacklight_lcd();
void backlight_lcd();
bool getBacklight_lcd();
void autoscroll_lcd();
void noAutoscroll_lcd();
void createChar_lcd(uint8_t, uint8_t[]);
void setCursor_lcd(uint8_t, uint8_t);
size_t write_lcd(uint8_t);
void command_lcd(uint8_t);

void blink_on_lcd();
void blink_off_lcd();
void cursor_on_lcd();
void cursor_off_lcd(); 

// Compatibility API function aliases
void setBacklight_lcd(uint8_t new_val);				// alias for backlight_lcd() and nobacklight()
void load_custom_character_lcd(uint8_t char_num, uint8_t *rows);	// alias for createChar_lcd()
void printstr_lcd(const char[]);


void send_lcd(uint8_t, uint8_t);
void write4bits(uint8_t);
void expanderWrite_lcd(uint8_t);
void pulseEnable_lcd(uint8_t);

extern uint8_t lcd_displayfunction;
extern uint8_t lcd_displaycontrol;
extern uint8_t lcd_displaymode;
extern uint8_t lcd_rows;
extern uint8_t lcd_charsize;
extern uint8_t lcd_backlightval;

#endif
