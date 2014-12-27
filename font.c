/******************************************************************************
 * Font Generator
 * designed for GLCD with memory organized in vertical bytes
 * 
 * Own character sets or symbols can easily be added:
 * The structure of font data is compatible with Hagen Reddmanns FontEditor
 * http://www.mikrocontroller.net/topic/99701#865331
 * when using the attached template_simplefont.c and template_simplefont.h
 * files, saving in uncompressed format and using font heights of multiples
 * of 8.
 * 
 * Fixed width fonts are treated as proportional fonts, but do not have a 
 * table for the width of each character (D'OH!)
 * 
 * When using with other GLCDs, make sure the byte orientation of the LCDs
 * memory matches the design of the ea-dogm series or link the LCD access
 * functions (see header file) to functions converting the data.
 * 
 * My thanks go to Hagen Reddmann for his Font Editor software
 * (http://www.mikrocontroller.net/topic/99701#865331)
 * and Benedikt K. for his various LCD fonts 
 * (http://www.mikrocontroller.net/topic/54860)
 * as well as Oliver Schwaneberg for adding several new functions
 * 
 * Author:  Jan Michel (jan at mueschelsoft dot de)
 * License: GNU General Public License, version 3
 * Version: v0.94 October 2011
 * ****************************************************************************
 * New features in v0.94
 *   - Characters may now be bigger than 128 Byte and occupy more than 1024 Pixels each
 *   - Added 32px high digits (4 digits plus colon plus large spacing = 128px)
 * New features in v0.93
 *   - output functions for long / int / float depend on global font setting
 *     (removing the two additional parameters made usage much easier I think)
 *   - fixed bug in font_proportional_16px regarding space
 *   - fixed symbol_16px
 * New features in v0.92
 *   - font and style setting can be stored globally (lcd_set_font)
 *   - simple put functions: lcd_putc, lcd_putstr, lcd_putstr_P
 *   - added 24px high, 16px wide digit font
 *   - added underline to font styles
 * New features in v0.91
 *   - text can be displayed in inverted mode
 *   - text can automatically wrap to the next line (if selected)
 *   - direct output of long / int / float
 *   - put_string and put_char functions now return the total width of the
 *     text in pixel
 *****************************************************************************/

#include "font.h"

/******************************************************************************
 * Global storage for easy-to-use putc functions
 *****************************************************************************/
FONT_P global_font_select;
uint8_t global_font_style;

/******************************************************************************
 * Stores the default font type and style in a global variable
 * font          - Font identifier
 * style         - Style Modifier
 */
inline void lcd_set_font(FONT_P font, uint8_t style){
  global_font_select = font;
  global_font_style = style;
}

/******************************************************************************
 * Helper Functions to find, retrieve and display characters
 *****************************************************************************/

/******************************************************************************
 * Loads the pointer to the selected fonts data
 */
inline PGM_P font_data(FONT_P font) {
  PGM_P tmp;
  if (sizeof(tmp) == 2)
    tmp = (PGM_P)pgm_read_word(&(font->data));
  else
    memcpy_P((char*)&tmp,&(font->data),sizeof(tmp));
  return tmp;
  }


/******************************************************************************
 * Loads the pointer to the width table for the selected font
 */
inline PGM_P font_widthtable(FONT_P font) {
  PGM_P tmp;
  if (sizeof(tmp) == 2)
    tmp = (PGM_P)pgm_read_word(&(font->widthtable));
  else
    memcpy_P((char*)&tmp,&(font->widthtable),sizeof(tmp));
  return tmp;
  }


/******************************************************************************
 * Loads the height (in bytes) of the given font
 */
inline uint8_t font_get_height_bytes(FONT_P font) {
  FONT_P tmp = font;
  uint8_t t = pgm_read_byte(&tmp->height);
  return (((uint8_t)(t-1)>>3)+1);
  }




/******************************************************************************
 * Get the number of the character in the given font
 */
inline int16_t font_get_char_number(FONT_P font, char character) {
  FONT_P tmp = font;
  if (character > pgm_read_byte(&tmp->lastchar)) 
    return -1;
  uint8_t first = pgm_read_byte(&tmp->firstchar);
  if (character < first) 
    return -1;
  return character - first;
  }


/******************************************************************************
 * Read the width of the selected character from the font width table
 */
inline uint8_t font_get_char_width(FONT_P font, char character) {
  PGM_P table = font_widthtable(font);
  if (table)
    return pgm_read_byte(table+font_get_char_number(font,character));
  else
    return pgm_read_byte(&font->width);
  }


/******************************************************************************
 * Calculate the pointer to the requested character inside the Flash ROM
 */
PGM_P font_get_char_position(FONT_P font, char character) {
  uint16_t ret         = 0;
  int16_t  charnum_ret = font_get_char_number(font, character);
  uint8_t  charnum     = charnum_ret;
  PGM_P    base        = font_widthtable(font);

  if (charnum_ret < 0)     //char not found
    return 0;
  if(base == 0)            //fixed width
    return font_data(font) + (uint16_t)charnum * (uint8_t)(font_get_height_bytes(font) * font_get_char_width(font,character));
  if (charnum)             //proportional width
    while(charnum--)
      ret += (uint8_t) pgm_read_byte(base++);
  return (font_data(font))+ret*font_get_height_bytes(font);
  }


/******************************************************************************
 * Doubles the bytes of either the upper of lower nibble of the given byte
 * part = 0:  abcdefgh -> eeffgghh
 * part = 1:  abcdefgh -> aabbccdd
 * Used for double height font
 */
inline unsigned char double_bits(uint8_t part, char c) {
  uint8_t t = 0;
  if (part) c = c>>4;
  if (c & 0x08) t  = 0xC0;
  if (c & 0x04) t |= 0x30;
  if (c & 0x02) t |= 0x0C;
  if (c & 0x01) t |= 0x03;
  return t;
  }

/******************************************************************************
 * Output functions for characters and strings
 *****************************************************************************/

/******************************************************************************
 * Outputs a character on the display, using the given font and style
 */
uint8_t lcd_put_char(FONT_P font, uint8_t style, char character) {
  uint16_t  i;
  uint8_t row  = 0;                             //current row of char
  #ifdef LCD_DOUBLE_PIXEL
    uint8_t hc   = 1;                           //height forced
  #else
    uint8_t hc   = (style & DOUBLE_HEIGHT)?1:0; //height changed
  #endif
  uint8_t wc   = (style & DOUBLE_WIDTH)?1:0;    //width changed
  uint8_t ul   = (style & UNDERLINE)?0x80:0x00; //underline
  uint8_t inv  = (style & INVERT)?0xFF:0;       //inverted
  uint8_t spc  = (style & SPACING)?3:1;         //spacing
  uint8_t tmp;

  //load information about character
   uint8_t char_width    = font_get_char_width(font,character); 
   uint8_t font_height   = font_get_height_bytes(font);
   uint8_t free_space    = spc;
   PGM_P   tableposition = font_get_char_position(font,character);

  //final size of character
  uint8_t char_final_width  = (uint8_t)(char_width+free_space) << wc;
  uint8_t char_final_height = (uint8_t)font_height << hc; 

  //check for avail. space on display
  if ((style & WRAP) && (LCD_CURRENT_COL() + char_final_width > LCD_WIDTH)) {
    LCD_MOVE_TO(LCD_CURRENT_PAGE()+char_final_height,0);
    if (character == ' ') return 0;
    }
  
  //write character
  do {
    for(i=(row>>hc); i<char_width*font_height; i+=font_height) {
      tmp = pgm_read_byte(tableposition+i);
      if(row == char_final_height-1) 
        tmp |= ul;
      if(hc)
        tmp = double_bits((row&1),tmp);
      if(inv)
        tmp = ~tmp;
      LCD_WRITE(tmp);
      if(wc) 
        LCD_WRITE(tmp);
      }
    if (free_space) {
      uint8_t c = inv;
      if(row == char_final_height-1) {
        c ^= ul; 
        if(hc)
          c ^= ul>>1;      
          }
	    for(uint8_t x = free_space<<wc; x>0;x--) {
        LCD_WRITE(c);
		    }		    
      }
    LCD_MOVE(1,-char_final_width);
    } while (++row < char_final_height);

  //move cursor to upper right corner of character
  LCD_MOVE(-char_final_height,char_final_width);
  return char_final_width;
  }


/******************************************************************************
 * Outputs a string on the display, loading it from the program memory,
 * using the given font and style
 */
uint16_t lcd_put_string_P(FONT_P font, uint8_t style, PGM_P str) {
  unsigned char t;
  uint16_t length = 0;
  while((t = pgm_read_byte(str++))) 
    length += lcd_put_char(font,style,t);
  return length;
  }

  
/******************************************************************************
 * Outputs a string on the display, using the given font and style
 */
uint16_t lcd_put_string(FONT_P font, uint8_t style, char* str) {
  unsigned char t;
  uint16_t length = 0;
  while((t = *str++))
    length += lcd_put_char(font,style,t);
  return length;
  }

  
/******************************************************************************
 * Outputs a string on the display, using the given font and style, reading
 * length characters from the memory
 */ 
uint16_t lcd_put_string_length(FONT_P font, uint8_t style, char* str, uint8_t length) {
  unsigned char t;
  uint16_t total_len = 0;
  for(t=0;t<length;t++)
    total_len += lcd_put_char(font,style,*str++);
  return total_len;
  }

  
/******************************************************************************
 * Outputs a string on the display, using the given font and style, reading
 * the string from program memory. The position of the string on the display
 * is selected by page / col.
 */ 
uint16_t lcd_put_string_xy_P(FONT_P font, uint8_t style, PGM_P str,uint8_t page, uint8_t col) {
  LCD_MOVE_TO(page,col);
  return lcd_put_string_P(font,style,str);
  }
  
  
/******************************************************************************
 * Outputs a string on the display, using the given font and style, reading
 * the string from main memory. The position of the string on the display
 * is selected by page / col.
 */ 
uint8_t lcd_put_char_xy(FONT_P font, uint8_t style, char character, uint8_t page, uint8_t col) {
  LCD_MOVE_TO(page,col);
  return lcd_put_char(font,style,character);
  }


/******************************************************************************
 * Outputs a character on the display, using the global font and style
 */ 
uint8_t  lcd_putc(char c) {
  return lcd_put_char(global_font_select, global_font_style, c);
  }

/******************************************************************************
 * Outputs a character on the display, using the global font and style
 */ 
uint8_t  lcd_putc_xy(char c, uint8_t page, uint8_t col) {
  return lcd_put_char_xy(global_font_select, global_font_style, c, page, col);
  }  
  
  
/******************************************************************************
 * Outputs a string on the display, using the global font and style
 */   
uint16_t lcd_putstr(char* str) {
  return lcd_put_string(global_font_select, global_font_style, str);
  }

  
  
/******************************************************************************
 * Outputs a string stored in program memory on the display, using the global 
 * font and style
 */   
uint16_t lcd_putstr_P(PGM_P str) {
  return lcd_put_string_P(global_font_select, global_font_style, str);
  }

  
/******************************************************************************
 * Outputs a string on the display, using the global font and style at the 
 * given position
 */   
uint16_t lcd_putstr_xy_P(PGM_P  str, uint8_t page, uint8_t col) {
  return lcd_put_string_xy_P(global_font_select, global_font_style, str, page, col);
  }  
  

#if INCLUDE_INTEGER_OUTPUT == 1
/******************************************************************************
 * Outputs a 32bit signed integer on the display // Added by Olli S.
 */ 
uint16_t lcd_put_long  (int32_t integer) {
  char buffer[10];
	ltoa(integer, buffer, 10);
	return lcd_put_string(global_font_select, global_font_style, buffer);
  }

  
/******************************************************************************
 * Outputs a 16bit signed integer on the display // Added by Olli S.
 */   
uint16_t lcd_put_int (int16_t integer) {
  char buffer[10];
	itoa(integer, buffer, 10);
	return lcd_put_string(global_font_select, global_font_style, buffer);
  }  

  
/******************************************************************************
 * Outputs a 16bit unsigned integer on the display // Added by Olli S.
 */   
uint16_t lcd_put_uint  (uint16_t integer) {
  char buffer[10];
	utoa(integer, buffer, 10);
	return lcd_put_string(global_font_select, global_font_style, buffer);
  }  
  
/******************************************************************************
 * Outputs a 8bit signed integer on the display
 */   
uint16_t lcd_put_short (int8_t integer) {
  char buffer[10];
	itoa(integer, buffer, 10);
	return lcd_put_string(global_font_select, global_font_style, buffer);
  }    
  
#endif

#if INCLUDE_FLOAT_OUTPUT == 1
/******************************************************************************
 * Outputs a float on the display // Added by Olli S.
 */ 
uint16_t lcd_put_float  (float fvalue) {
  char buffer[10];
	dtostrf(fvalue, 2, 1, buffer);
	return lcd_put_string(global_font_select, global_font_style, buffer);
  }
#endif
