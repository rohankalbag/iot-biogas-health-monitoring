// Electronic Design Lab EE 344 IIT Bombay
// Team Tue: 23 
// Authors: Sankalp Bhamare, Rohan Rajesh Kalbag, Jujhaar Singh

#include "raw_lcd_driver.h"
uint8_t lcd_displayfunction;
uint8_t lcd_displaycontrol;
uint8_t lcd_displaymode;
uint8_t lcd_rows;
uint8_t lcd_charsize;
uint8_t lcd_backlightval;


void raw_init_lcd(uint8_t _lcd_rows)
{
	// initialize the parameters for the lcd
  	lcd_rows = _lcd_rows;
	lcd_charsize = LCD_5x8DOTS;
	lcd_backlightval = LCD_BACKLIGHT;
}

void begin_lcd() {
	// start displaying
	lcd_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	if (lcd_rows > 1) {
		lcd_displayfunction |= LCD_2LINE;
	}

	// for some 1 line displays you can select a 10 pixel high font
	if ((lcd_charsize != 0) && (lcd_rows == 1)) {
		lcd_displayfunction |= LCD_5x10DOTS;
	}

	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	usleep(50*1000);

	// Now we pull both RS and R/W low to begin_lcd commands
	expanderWrite_lcd(lcd_backlightval);	// reset expanderand turn backlight_lcd off (Bit 8 =1)
	usleep(1000*1000);

	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46

	// we start in 8bit mode, try to set 4 bit mode
	write4bits(0x03 << 4);
  	usleep(4500);

	// second try
	write4bits(0x03 << 4);
	usleep(4500); // wait min 4.1ms

	// third go!
	write4bits(0x03 << 4);
	usleep(150);

	// finally, set to 4-bit interface
	write4bits(0x02 << 4);

	// set num lines, font size, etc.
	command_lcd(LCD_FUNCTIONSET | lcd_displayfunction);

	// turn the display_lcd on with no cursor_lcd or blinking default
	lcd_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display_lcd();

	// clear_lcd it off
	clear_lcd();

	// Initialize to default text direction (for roman languages)
	lcd_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	command_lcd(LCD_ENTRYMODESET | lcd_displaymode);

	home_lcd();
}

void clear_lcd(){
	// clear the display
	command_lcd(LCD_CLEARDISPLAY);// clear_lcd display_lcd, set cursor_lcd position to zero
	usleep(2000);  // this command_lcd takes a long time!
}

void home_lcd(){
	// reset cursor position
	command_lcd(LCD_RETURNHOME);  // set cursor_lcd position to zero
	usleep(2000);  // this command_lcd takes a long time!
}

void setCursor_lcd(uint8_t col, uint8_t row) {
	// set the cursor
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if (row > lcd_rows) {
		row = lcd_rows-1;    // we count rows starting w/0
	}
	command_lcd(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display_lcd on/off (quickly)

void noDisplay_lcd() {
	lcd_displaycontrol &= ~LCD_DISPLAYON;
	command_lcd(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

void display_lcd() {
	lcd_displaycontrol |= LCD_DISPLAYON;
	command_lcd(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

// Turns the underline cursor_lcd on/off

void noCursor_lcd() {
	lcd_displaycontrol &= ~LCD_CURSORON;
	command_lcd(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

void cursor_lcd() {
	lcd_displaycontrol |= LCD_CURSORON;
	command_lcd(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

// Turn on and off the blinking cursor_lcd
void noBlink_lcd() {
	lcd_displaycontrol &= ~LCD_BLINKON;
	command_lcd(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

void blink_lcd() {
	lcd_displaycontrol |= LCD_BLINKON;
	command_lcd(LCD_DISPLAYCONTROL | lcd_displaycontrol);
}

// These commands scroll the display_lcd without changing the RAM
void scrollDisplayLeft_lcd(void) {
	command_lcd(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void scrollDisplayRight_lcd(void) {
	command_lcd(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void leftToRight_lcd(void) {
	lcd_displaymode |= LCD_ENTRYLEFT;
	command_lcd(LCD_ENTRYMODESET | lcd_displaymode);
}

// This is for text that flows Right to Left
void rightToLeft_lcd(void) {
	lcd_displaymode &= ~LCD_ENTRYLEFT;
	command_lcd(LCD_ENTRYMODESET | lcd_displaymode);
}

// This will 'right justify' text from the cursor_lcd
void autoscroll_lcd(void) {
	lcd_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command_lcd(LCD_ENTRYMODESET | lcd_displaymode);
}

// This will 'left justify' text from the cursor_lcd
void noAutoscroll_lcd(void) {
	lcd_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command_lcd(LCD_ENTRYMODESET | lcd_displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void createChar_lcd(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command_lcd(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		write_lcd(charmap[i]);
	}
}

// Turn the (optional) backlight_lcd off/on
void noBacklight_lcd(void) {
	lcd_backlightval=LCD_NOBACKLIGHT;
	expanderWrite_lcd(0);
}

void backlight_lcd(void) {
	lcd_backlightval=LCD_BACKLIGHT;
	expanderWrite_lcd(0);
}

bool getBacklight_lcd() {
  return lcd_backlightval == LCD_BACKLIGHT;
}


// mid level commands, for sending data/cmds

void command_lcd(uint8_t value) {
	send_lcd(value, 0);
}

size_t write_lcd(uint8_t value) {
	send_lcd(value, Rs);
	return 1;
}


// low level data pushing commands

// write_lcdeither command_lcd or data
void send_lcd(uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
	write4bits((highnib)|mode);
	write4bits((lownib)|mode);
}

void write4bits(uint8_t value) {
	expanderWrite_lcd(value);
	pulseEnable_lcd(value);
}

void expanderWrite_lcd(uint8_t _data){
  uint8_t data_t =  _data | lcd_backlightval;
  ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, LCD_ADDRESS, &data_t, 1, 1000));
}

void pulseEnable_lcd(uint8_t _data){
	expanderWrite_lcd(_data | En);	// En high
	usleep(1);		// enable pulse must be >450ns

	expanderWrite_lcd(_data & ~En);	// En low
	usleep(50);		// commands need > 37us to settle
}

void load_custom_character_lcd(uint8_t char_num, uint8_t *rows){
	createChar_lcd(char_num, rows);
}

void setBacklight_lcd(uint8_t new_val){
	if (new_val) {
		backlight_lcd();		// turn backlight_lcd on
	} else {
		noBacklight_lcd();		// turn backlight_lcd off
	}
}

void printstr_lcd(const char c[]){
	//This function is not identical to the function used for "real" I2C displays
	//it's here so the user sketch doesn't have to be changed
  const char *tc = c;
  while(*tc != '\0')
    write_lcd(*(tc++));
}

void blink_on_lcd() { blink_lcd(); }
void blink_off_lcd() { noBlink_lcd(); }
void cursor_on_lcd() { cursor_lcd(); }
void cursor_off_lcd() { noCursor_lcd(); }