/******************************************************************************
 * Display Library
 * for an LCD with ILI9341 driver in 16 Bit color mode using 4-wire SPI
 * New features in v0.01
 *   - well... everything.
 *****************************************************************************/

#include "lcd-color-graphic.h"


/******************************************************************************
  * Global variables for color handling
  * Foreground is the normal drawing color
  * Background is used as background, e.g. when writing fonts.
  */

color_t foreground = {.red=0x1F, .green=0x3F, .blue=0x1F};
color_t background = {.red=0,    .green=0,    .blue=0};


/******************************************************************************
  * Initializes the display
  */
void lcd_init() {
  LCD_SET_PIN_DIRECTIONS();  //set outputs
  _delay_ms(1);
  LCD_INIT_SPI();            //Initialize SPI Interface  
  _delay_ms(50);
  LCD_RESET_ON();            //Apply Reset to the Display Controller  
  _delay_ms(100); 
  LCD_RESET_OFF();  
  _delay_ms(100);
  LCD_SELECT();               //Switches chip select on
  lcd_command(LCD_SLEEP_OUT); //Wake up LCD
  _delay_ms(70);
  return;
  }

  
/******************************************************************************
  * Sends a command to the display
  */
inline void lcd_command(uint8_t c) {
  spi_wait_for_idle();
  LCD_CMD();
  spi_write(c);
  }

/******************************************************************************
  * Sends a command with one argument
  */
inline void lcd_command_1(uint8_t c, uint8_t data) {
  lcd_command(c);
  lcd_data(data);
}  
  
/******************************************************************************
  * Sends a data word to the display
  */
inline void lcd_data(uint8_t c) {
  spi_wait_for_idle();
  LCD_DATA();
  spi_write(c);
}  



/******************************************************************************
  * Stores the main drawing color for later use
  */
inline void lcd_set_foreground(uint8_t r, uint8_t g, uint8_t b) {
  foreground.red = r;
  foreground.green = g;
  foreground.blue = b;
  }


/******************************************************************************
  * Stores the background color for later use
  */
inline void lcd_set_background(uint8_t r, uint8_t g, uint8_t b) {
  background.red = r;
  background.green = g;
  background.blue = b;
  }


/******************************************************************************
  * Sets the column range used for the next write operation
  */
inline void lcd_set_column(uint16_t start, uint16_t end) {
  lcd_command(LCD_SET_COLUMN);
  lcd_data(start >> 8);
  lcd_data(start);
  lcd_data(end >> 8);
  lcd_data(end);
  }

/******************************************************************************
  * Sets the page range used for the next write operation
  */
inline void lcd_set_page(uint16_t start, uint16_t end) {
  lcd_command(LCD_SET_PAGE);
  lcd_data(start >> 8);
  lcd_data(start);
  lcd_data(end >> 8);
  lcd_data(end);
  }

/******************************************************************************
  * Writes a pixel to the display using 16 Bit color mode
  */
inline void lcd_send_pixel(color_t c) {
  lcd_data((c.red<<3) | (c.green>>3));
  lcd_data((c.green<<5) | c.blue);
  }  
  
/******************************************************************************
  * Sets a pixel at a given position
  */
inline void lcd_set_pixel_xy(uint16_t column, uint16_t page) {
  lcd_set_page(page,page);
  lcd_set_column(column,column);
  lcd_command(LCD_WRITE_MEM);
  lcd_send_pixel(foreground);
  }    


/******************************************************************************
 * This function sets an area of the screen to a given color
 * col0          - left edge of the area
 * col1          - right edge of the area
 * page0         - top edge of the area
 * page1         - bottom edge of the area
 * r,g,b         - the color to be used
 */
void lcd_set_area_xy(uint16_t col0, uint16_t col1, uint16_t page0, uint16_t page1) {
  lcd_set_column(col0,col1);
  lcd_set_page(page0,page1);
  lcd_command(LCD_WRITE_MEM);
  for(uint16_t y = page0; y <= page1; y++)
    for(uint16_t x = col0; x <= col1; x++) {
      lcd_send_pixel(background);
      }
  }
  
  
  
//=============================================================================
//Compatibility functions to accept output of font generator
// Pages are counted in units of 8 pixels!
//=============================================================================

uint16_t lcd_current_page = 0;
uint16_t lcd_current_column = 0;


/******************************************************************************
 * Changes the internal cursor by s pages
 * s             - number of pages to move
 */ 
uint16_t lcd_inc_page(int16_t s) {
  uint16_t p = lcd_current_page;
  p += s;
  if (p > LCD_HEIGHT/8)
    p = 0;
  lcd_current_page = p;
  
  return p;
  }

/******************************************************************************
 * Changes the internal cursor by s columns, including wrapping (if selected)
 * s             - number of columns to move
 */ 
uint16_t lcd_inc_column(int16_t s) {
  uint16_t c = lcd_current_column;
  c += s;
  if (c > LCD_WIDTH) 
    c = 0;
  lcd_current_column = c;
  return c;
  }
  
  
/******************************************************************************
 * Moves the cursor to the given position
 * pages         - page to move to
 * columns       - column to move to
 */ 
void lcd_moveto_xy(uint16_t page, uint16_t column) {
  lcd_current_column = column; 
  lcd_current_page = page;
  }

/******************************************************************************
 * Moves the cursor relative to the current position
 * pages         - number of pages to move
 * columns       - number of columns to move
 */  
void lcd_move_xy(int16_t pages, int16_t columns) {
  lcd_moveto_xy(lcd_inc_page(pages),lcd_inc_column(columns));
  }


/******************************************************************************
 * Takes a vertical byte from the font generator and prints it on the display
 * b             - Bit pattern to display
 */ 
void lcd_write_font_byte(uint8_t b) {
  lcd_set_page(8*lcd_current_page,8*lcd_current_page+8);
  lcd_set_column(lcd_current_column,lcd_current_column);
  lcd_command(LCD_WRITE_MEM);
  for(uint8_t i=0;i<8;i++) {
    if(b&1)
      lcd_send_pixel(foreground);
    else
      lcd_send_pixel(background);
    b = b>>1;
    }
  lcd_inc_column(1);
  }
  