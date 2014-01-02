#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//Include your LCD graphics library here
#include "dogm-graphic.h"
//declare font info structure defined below
struct font_info;
typedef const struct font_info * FONT_P;

/*****************************************************************************
 * BEGIN CONFIG BLOCK
 *****************************************************************************/

//Select which of the available features will be included
//If a font is not needed, then you don't need to put it to the Flash 
//Just comment one of the defines below

#define FONTS_INCLUDE_font_proportional_16px    font_proportional_16px
#define FONTS_INCLUDE_font_proportional_8px     font_proportional_8px   
#define FONTS_INCLUDE_font_fixed_8px            font_fixed_8px
#define FONTS_INCLUDE_font_fixed_16px           font_fixed_16px
#define FONTS_INCLUDE_symbols_8px               symbols_8px
#define FONTS_INCLUDE_symbols_16px              symbols_16px
#define FONTS_INCLUDE_digits_24px               digits_24px
#define FONTS_INCLUDE_digits_32px               digits_32px

//Set to 1 to include functions for direct output of integer & float values
#define INCLUDE_FLOAT_OUTPUT     0
#define INCLUDE_INTEGER_OUTPUT   1


/*****************************************************************************
 * All font structures + verbose name defines
 *****************************************************************************/
#ifdef FONTS_INCLUDE_font_fixed_8px
  extern const struct font_info font_fixed_8px PROGMEM ;
  #define FONT_FIXED_8 &font_fixed_8px
#endif

#ifdef FONTS_INCLUDE_font_fixed_16px
  extern const struct font_info font_fixed_16px PROGMEM ;
  #define FONT_FIXED_16 &font_fixed_16px
#endif

#ifdef FONTS_INCLUDE_font_proportional_16px
  extern const struct font_info font_proportional_16px PROGMEM ;
  #define FONT_PROP_16 &font_proportional_16px
#endif

#ifdef FONTS_INCLUDE_font_proportional_8px
  extern const struct font_info font_proportional_8px PROGMEM;
  #define FONT_PROP_8 &font_proportional_8px
#endif

#ifdef FONTS_INCLUDE_symbols_8px
  extern const struct font_info symbols_8px PROGMEM;
  #define FONT_SYMBOL_8 &symbols_8px
#endif

#ifdef FONTS_INCLUDE_symbols_16px
  extern const struct font_info symbols_16px PROGMEM;
  #define FONT_SYMBOL_16 &symbols_16px
#endif

#ifdef FONTS_INCLUDE_digits_24px
  extern const struct font_info digits_24px PROGMEM;
  #define FONT_DIGITS_24 &digits_24px
#endif

#ifdef FONTS_INCLUDE_digits_32px
  extern const struct font_info digits_32px PROGMEM;
  #define FONT_DIGITS_32 &digits_32px
#endif

//Assign real functions, provided by the LCD library to these macros 
//used within the font generator

#define LCD_MOVE(x,y)      lcd_move_xy((x),(y))     //relative cursor movement
#define LCD_MOVE_TO(x,y)   lcd_moveto_xy((x),(y))   //absolute cursor movement
#define LCD_WRITE(x)       lcd_data((x))            //write data to display

//Functions to read the current position as provided by the LCD library
#define LCD_CURRENT_COL()   lcd_get_position_column()
#define LCD_CURRENT_PAGE()  lcd_get_position_page()


/******************************************************************************
 * END CONFIG BLOCK
 *****************************************************************************/




/******************************************************************************
 * Internal Functions to locate and display characters
 *****************************************************************************/
/*
PGM_P   font_data              (FONT_P font);
PGM_P   font_widthtable        (FONT_P font);
uint8_t font_get_height_bytes  (FONT_P font);
uint8_t font_get_type          (FONT_P font);
uint8_t font_get_char_width    (FONT_P font, char character);
int16_t font_get_char_number   (FONT_P font, char character);
PGM_P   font_get_char_position (FONT_P font, char character);
*/


/******************************************************************************
 * Functions to display characters and strings
 *****************************************************************************/
//where font is a pointer to a FONT_P struct,
//style is one or more of the defines for different font sizes & styles shown below
//and str and c are the string or the character to be displayed

uint16_t lcd_put_string       (FONT_P font, uint8_t style, char* str);
uint16_t lcd_put_string_length(FONT_P font, uint8_t style, char* str,      uint8_t length);
uint16_t lcd_put_string_P     (FONT_P font, uint8_t style, PGM_P str);
uint8_t  lcd_put_char         (FONT_P font, uint8_t style, char c);
uint16_t lcd_put_string_xy_P  (FONT_P font, uint8_t style, PGM_P str,      uint8_t page, uint8_t col);
uint8_t  lcd_put_char_xy      (FONT_P font, uint8_t style, char character, uint8_t page, uint8_t col);

void     lcd_set_font         (FONT_P font, uint8_t style);
uint8_t  lcd_putc             (char c);
uint8_t  lcd_putc_xy          (char c, uint8_t page, uint8_t col);
uint16_t lcd_putstr           (char* str);
uint16_t lcd_putstr_P         (PGM_P str);
uint16_t lcd_putstr_xy_P      (PGM_P  str, uint8_t page, uint8_t col);

#if INCLUDE_INTEGER_OUTPUT == 1
uint16_t lcd_put_long (int32_t integer);
uint16_t lcd_put_int  (int16_t integer);
uint16_t lcd_put_uint (uint16_t integer);
uint16_t lcd_put_short(int8_t  integer);
#endif
#if INCLUDE_FLOAT_OUTPUT == 1
uint16_t lcd_put_float (float integer);
#endif



/******************************************************************************
 * Define font styles
 * These names are used as the style attribute to above put* functions
 *****************************************************************************/

#define NORMAL        0
#define NORMAL_SIZE   0
#define DOUBLE_HEIGHT 1    //double the height of each character 
#define DOUBLE_WIDTH  2    //double the width of each character 
#define DOUBLE_SIZE   3    //double height and double width
#define INVERT        4    //draw white letters with black background
#define WRAP          8    //wrap around text that does not fit in the current line
#define UNDERLINE    16    //underline the text
#define SPACING      32    //increase spacing between character


//used for internal functions:
#define DOUBLE_HEIGHT_BIT 1
#define DOUBLE_WIDTH_BIT  2
#define INVERT_BIT        4
#define WRAP_BIT          8
#define UNDERLINE_BIT    16

/******************************************************************************
 * Documentation: Example Font File
 * A c-template to use with Hagen Reddmanns font generator is 
 * included (template_simplefont.c)
 *****************************************************************************/
/*
#include "font.h"
#ifdef %FONT INCLUDE IDENTIFIER%
const char fontname_length[] PROGMEM = {...}
const char fontname_data[] PROGMEM = {...}   
const struct font_info fontname PROGMEM = {...};
#endif
*/


/******************************************************************************
 * Structure holds information about fonts 
 *****************************************************************************/
struct font_info {
    uint16_t size;       //size of data array
    uint8_t  width;      //(maximum) width of character
    uint8_t  height;     //height of character
    uint8_t  firstchar;  //the number of the first included character (often 0x20)
    uint8_t  lastchar;   //the last included character (often 0xFF)
    PGM_VOID_P    widthtable; //Pointer to the table holding character widths (NULL for monospaced fonts)
    PGM_VOID_P    data;       //Pointer to data arrray
    };



#endif
