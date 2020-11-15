#ifndef DOGMGRAPHIC_H_INCLUDED
#define DOGMGRAPHIC_H_INCLUDED

#include <avr/io.h>
#include <inttypes.h>
#include <string.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "font.h"

/*****************************************************************************
 * BEGIN CONFIG BLOCK
 *****************************************************************************/
//Select the display type: DOGS102: 102, DOGM128/DOGL128: 128, DOGM132: 132, DOGXL160: 160, DOGXL240: 240
#define DISPLAY_TYPE  240

//Display Orientation: Normal (0) or upside-down (1)?
#define ORIENTATION_UPSIDEDOWN 0

//Should chip select (CS) be used?
#define LCD_USE_CHIPSELECT  1

//Use Backlight?  (0: no backlight, 1: backlight (on when pin is high), 2: backlight (on when pin is low))
#define LCD_USE_BACKLIGHT   0

//A0 Port (CD on DOGS & DOGXL)
#define PORT_A0  PORTF_OUT
#define DDR_A0   PORTF_DIR
#define PIN_A0   1

//Reset Port
#define PORT_RST PORTF_OUT
#define DDR_RST  PORTF_DIR
#define PIN_RST  0

//Backlight Port
#if LCD_USE_BACKLIGHT != 0
  #define PORT_LED PORTB
  #define DDR_LED  DDRB
  #define PIN_LED  4
#endif

//Chip select
#if LCD_USE_CHIPSELECT == 1
  #define PORT_CS  PORTF_OUT
  #define DDR_CS   PORTF_DIR
  #define PIN_CS   4
#endif

//SPI routines
//Define a function that initializes  the SPI interface, see below for an example
extern void init_spi_lcd(void);
#define LCD_INIT_SPI() init_spi_lcd()

//Define a function that waits until SPI interface is idle
#define spi_wait_for_idle() while(! (SPIF_STATUS & _BV(SPI_IF_bp)))

//Define how to write to SPI data register
#define spi_write(i) SPIF_DATA = i

//Define this if LCD Output should continue in next line when reaching edge of display
//Used for all outputs. To enable this feature for text only, use the appropriate flag in font.h
#define LCD_WRAP_AROUND  0

//Include graphic functions, i.e. lcd_draw_image_P, lcd_draw_image_xy_P, lcd_clear_area ? 
#define LCD_INCLUDE_GRAPHIC_FUNCTIONS  1

/*Example SPI setup (Atmega162)
 *init spi: msb first, update on falling edge , read on rising edge, 9 MHz
 *void init_spi_lcd() {
 *  SPCR = 0 << SPIE | 1 << SPE | 0 << DORD | 1 << MSTR | 1 << CPOL | 1 << CPHA | 0 << SPR1 | 0 << SPR0;
 *  SPSR = 1 << SPI2X;
 *  SPDR = LCD_NO_OP; //Do not use 0 here, only LCD_NOP is allowed!
 *  }
 */
/*****************************************************************************
 * END CONFIG BLOCK
 *****************************************************************************/




/*****************************************************************************
 * Public Functions
 *****************************************************************************/

//initializes the display in standard settings
//for DOGS, DOGM, DOGL and DOGXL
void lcd_init(void);

//write data word or command to the LCD
void lcd_data     (uint8_t data);
void lcd_command  (uint8_t cmd);


//Function to read the current position
extern uint8_t lcd_current_page;
extern uint8_t lcd_current_column;
static inline uint8_t lcd_get_position_page(void)   {return lcd_current_page;}
static inline uint8_t lcd_get_position_column(void) {return lcd_current_column;}

//User functions to write raw data from flash
#if LCD_INCLUDE_GRAPHIC_FUNCTIONS >= 1
  void lcd_draw_image_P(PGM_VOID_P progmem_image, uint8_t pages, uint8_t columns, uint8_t style);
  void lcd_draw_image_xy_P(PGM_VOID_P progmem_image, uint8_t x, uint8_t y, uint8_t pages, uint8_t columns, uint8_t style);
#endif

void lcd_clear_area(uint8_t pages, uint8_t columns, uint8_t style);
void lcd_clear_area_xy(uint8_t pages, uint8_t columns, uint8_t style, uint8_t page, uint8_t col);
  

//Move cursor
void lcd_moveto_xy  (uint8_t page, uint8_t column);
void lcd_move_xy    (int8_t pages, int16_t columns);

//Set display contrast
#if DISPLAY_TYPE == 240
  void lcd_set_contrast(uint8_t value);
#endif

//Text functions are included in font.c / font.h

/*****************************************************************************
 * LCD Size, based on type selection above
 *****************************************************************************/
#if DISPLAY_TYPE == 240                 // Autor: Robert Steigemann
  #define LCD_WIDTH            240      //width of the LCD
  #define LCD_HEIGHT           128      // height of the LCD
  #define LCD_RAM_PAGES        16       //size of LCD RAM
  #define LCD_PIXEL_PER_BYTE   8        //using single pixels
#endif
 
#if DISPLAY_TYPE == 160
  #define LCD_WIDTH            160 //width of the LCD
  #define LCD_HEIGHT           104 //height of the LCD
  #define LCD_RAM_PAGES        26  //size of LCD RAM
  #define LCD_PIXEL_PER_BYTE   4   //using double pixels
  #define LCD_DOUBLE_PIXEL     1   
  #define SHIFT_ADDR_NORMAL    0   //column offset for normal orientation
  #define SHIFT_ADDR_TOPVIEW   0   //column offset for bottom view orientation
#endif
 
 #if DISPLAY_TYPE == 132
  #define LCD_WIDTH          132 //width of the LCD
  #define LCD_HEIGHT         32  //height of the LCD
  #define LCD_RAM_PAGES      4   //size of LCD RAM
  #define LCD_PIXEL_PER_BYTE 8   //using single pixels
  #define SHIFT_ADDR_NORMAL  0   //column offset for normal orientation
  #define SHIFT_ADDR_TOPVIEW 0   //column offset for bottom view orientation
#endif

#if DISPLAY_TYPE == 128
  #define LCD_WIDTH          128 //width of the LCD
  #define LCD_HEIGHT         64  //height of the LCD
  #define LCD_RAM_PAGES      8   //size of LCD RAM
  #define LCD_PIXEL_PER_BYTE 8   //using single pixels
  #define SHIFT_ADDR_NORMAL  0   //column offset for normal orientation
  #define SHIFT_ADDR_TOPVIEW 4   //column offset for bottom view orientation
#endif

#if DISPLAY_TYPE == 102
  #define LCD_WIDTH          102 //width of the LCD
  #define LCD_HEIGHT         64  //height of the LCD
  #define LCD_RAM_PAGES      8   //size of LCD RAM
  #define LCD_PIXEL_PER_BYTE 8   //using single pixels
  #define SHIFT_ADDR_NORMAL  0   //column offset for normal orientation
  #define SHIFT_ADDR_TOPVIEW 30  //column offset for bottom view orientation
#endif

#if ORIENTATION_UPSIDEDOWN == 0
  #define SHIFT_ADDR   SHIFT_ADDR_NORMAL
#endif

#if ORIENTATION_UPSIDEDOWN == 1
  #define SHIFT_ADDR   SHIFT_ADDR_TOPVIEW
#endif

/*****************************************************************************
 * Command Codes
 *****************************************************************************/
#if DISPLAY_TYPE == 128 || DISPLAY_TYPE == 132 || DISPLAY_TYPE == 102
  #define LCD_DISPLAY_ENABLE    0xAE  //1: Display on/off
  #define LCD_START_LINE        0x40  //2: display start line set 
  #define LCD_PAGE_ADDRESS      0xB0  //3: Page address set (lower 4 bits select one of the pages)
  #define LCD_COL_ADDRESS       0x10  //4: column address 
  #define LCD_BOTTOMVIEW        0xA0  //8: select orientation 
  #define LCD_DISPLAY_INVERT    0xA6  //9: inverted display
  #define LCD_ALL_PIXEL         0xA4  //10: show memory content or switch all pixels on
  #define LCD_BIAS              0xA2  //11: lcd bias set
  #define LCD_RESET_CMD         0xE2  //14: Reset Controller
  #define LCD_SCAN_DIR          0xC0  //15: output mode select (turns display upside-down)
  #define LCD_POWER_CONTROL     0x28  //16: power control set 
  #define LCD_VOLTAGE           0x20  //17: voltage regulator resistor ratio set 
  #define LCD_VOLUME_MODE       0x81  //18: Volume mode set 
  #define LCD_NO_OP             0xE3  //22: NOP command
#endif

#if DISPLAY_TYPE == 128 || DISPLAY_TYPE == 132
  #define LCD_INDICATOR         0xAC  //19: static indicator (2-byte command)
  #define LCD_BOOSTER_SET       0xF8  //20: booster ratio set
#endif
  
#if DISPLAY_TYPE == 102
  #define LCD_ADV_PROG_CTRL     0xFA  //25: advanced program control 
  #define LCD_ADV_PROG_CTRL2    0x10  //25: advanced program control 
#endif

#if DISPLAY_TYPE == 160
  #define LCD_COL_ADDRESS       0x10  //4: column address 
  #define LCD_TEMP_COMP         0x24  //5: Set Temperature Compensation
  #define LCD_PANEL_LOAD        0x28  //6: Set Panel loading
  #define LCD_PUMP_CTRL         0x2C  //7: Set pump control
  #define LCD_ADV_PROG_CTRL     0x30  //8: advanced program control first word
  #define LCD_START_LINE        0x40  //9: display scroll line set LSB
  #define LCD_START_LINE2       0x50  //9: display scroll line set MSB
  #define LCD_PAGE_ADDRESS      0x60  //10: Page address set
  #define LCD_VOLTAGE_BIAS      0x81  //11: Bias set
  #define LCD_PARTIAL_CTRL      0x84  //12: Set partial display control
  #define LCD_RAM_ADDR_CTRL     0x88  //13: Set RAM address control
  #define LCD_FIXED_LINES       0x90  //14: Set display fixed lines
  #define LCD_LINE_RATE         0xA0  //15: Set line rate
  #define LCD_ALL_PIXEL         0xA4  //16: show all points
  #define LCD_INVERSE           0xA6  //17: Inverse display
  #define LCD_DISPLAY_ENABLE    0xAE  //18: Display enable
  #define LCD_MAPPING_CTRL      0xC0  //19: LCD mapping control
  #define LCD_GRAY_SHADE        0xD0  //20: LCD gray shade
  #define LCD_RESET_CMD         0xE2  //21: System reset
  #define LCD_NO_OP             0xE3  //22: NOP
  #define LCD_BIAS_RATIO        0xE8  //24: Bias Ratio
  #define LCD_CURSOR_MODE_RESET 0xEE  //25: Reset cursor update mode
  #define LCD_CURSOR_MODE_SET   0xEF  //26: Set cursor update mode
  #define LCD_COM_END           0xF1  //27: Set COM End
  #define LCD_PARTIAL_START     0xF2  //28: Set partial display start
  #define LCD_PARTIAL_END       0xF3  //29: Set partial display end
  #define LCD_WINDOW_START_COL  0xF4  //30: Window program start column
  #define LCD_WINDOW_START_PAGE 0xF5  //31: Window program start column
  #define LCD_WINDOW_END_COL    0xF6  //32: Window program start column
  #define LCD_WINDOW_END_PAGE   0xF7  //33: Window program start column
  #define LCD_WINDOW_PROGRAM    0xF8  //34: Enable window programming
#endif 

#if DISPLAY_TYPE == 240
  #define LCD_COL_ADDRESS       0x10  //4: column address MSB
  #define LCD_TEMP_COMP         0x24  //5: Set Temperature Compensation
  #define LCD_PANEL_LOAD        0x28  //6: Set Panel loading
  #define LCD_PUMP_CTRL         0x2C  //7: Set pump control
  #define LCD_START_LINE        0x40  //9: display scroll line set LSB
  #define LCD_START_LINE2       0x50  //9: display scroll line set MSB
  #define LCD_PAGE_ADDRESS_LSB  0x60  //10: Page address set
  #define LCD_PAGE_ADDRESS_MSB  0x70  //10: Page address set
  #define LCD_POTENTIOMETER     0x81  //11: Potentiometer set
  #define LCD_PARTIAL_CTRL      0x84  //14: Set partial display control
  #define LCD_RAM_ADDR_CTRL     0x88  //15: Set RAM address control
  #define LCD_FIXED_LINES       0x90  //16: Set fixed lines
  #define LCD_LINE_RATE         0xA0  //17: Set line rate
  #define LCD_ALL_PIXEL         0xA4  //18: Set all pixel on
  #define LCD_INVERSE           0xA6  //19: Inverse display
  #define LCD_DISPLAY_ENABLE    0xA8  //20: Display enable
  #define LCD_MAPPING_CTRL      0xC0  //21: LCD mapping control
  #define LCD_NLINE_INV         0xC8  //22: Set N-Line Inversion
  #define LCD_SET_PATTERN       0xD0  //23: Set Display Pattern
  #define LCD_RESET_CMD         0xE2  //24: System reset
  #define LCD_NO_OP             0xE3  //25: NOP
  #define LCD_BIAS_RATIO        0xE8  //27: Bias Ratio
  #define LCD_COM_END           0xF1  //28: Set COM End
  #define LCD_PARTIAL_START     0xF2  //29: Set display start
  #define LCD_PARTIAL_END       0xF3  //30: Set display end
  #define LCD_WINDOW_START_COL  0xF4  //31: Window program start column addr
  #define LCD_WINDOW_START_PAGE 0xF5  //32: Window program start row adr
  #define LCD_WINDOW_END_COL    0xF6  //33: Window program end column adr
  #define LCD_WINDOW_END_PAGE   0xF7  //34: Window program end row adr
  #define LCD_WINDOW_PROGRAM    0xF8  //35: Enable window programming
#endif

/*****************************************************************************
 * Makros to execute commands 
 *****************************************************************************/
 #if DISPLAY_TYPE == 240
  #define LCD_SET_TEMP_COMP(i)          lcd_command(LCD_TEMP_COMP | ((i) & 0x3))
  #define LCD_SET_PANEL_LOAD(i)         lcd_command(LCD_PANEL_LOAD | ((i) & 0x3))
  #define LCD_SET_PUMP_CTRL(i)          lcd_command(LCD_PUMP_CTRL | ((i) & 0x3))
  #define LCD_SET_START_LINE(i)         lcd_command(LCD_START_LINE | ((i)&0xF)); \
                                        lcd_command(LCD_START_LINE2 | (((i)>>4)&0x7))
  #define LCD_SET_PARTIAL_DISPLAY_CTRL(i) \
                                        lcd_command(LCD_PARTIAL_CTRL | ((i) & 0x3))
  #define LCD_SET_RAM_ADDR_CTRL(i)      lcd_command(LCD_RAM_ADDR_CTRL | ((i) & 0x7))
  #define LCD_SET_DISPLAY_PATTERN(i)    lcd_command(LCD_SET_PATTERN | ((i) & 0x7))
  #define LCD_SET_FIXED_LINES(i)        lcd_command(LCD_FIXED_LINES | ((i) & 0xF))
  #define LCD_SET_LINE_RATE(i)          lcd_command(LCD_LINE_RATE | ((i) & 0x3))
  #define LCD_SHOW_ALL_PIXELS_ON()      lcd_command(LCD_ALL_PIXEL | 1)
  #define LCD_SHOW_ALL_PIXELS_OFF()     lcd_command(LCD_ALL_PIXEL | 0)
  #define LCD_INVERT_DISPLAY(i)         lcd_command(LCD_INVERSE | ((i)&1))
  #define LCD_SWITCH_ON()               lcd_command(LCD_DISPLAY_ENABLE | 1)
  #define LCD_SWITCH_OFF()              lcd_command(LCD_DISPLAY_ENABLE | 0)
  #define LCD_SET_MAPPING_CTRL(i)       lcd_command(LCD_MAPPING_CTRL); \
                                        lcd_command((i) & 0xF)
  #define LCD_SET_PAGE_ADDR(i)          lcd_command(LCD_PAGE_ADDRESS_LSB | ((i) & 0x0F)); \
                                        lcd_command(LCD_PAGE_ADDRESS_MSB | (((i)>>4) & 0x0F))
  #define LCD_SET_COLUMN_ADDR(i)        lcd_command(LCD_COL_ADDRESS | (((i)>>4) & 0x0F)); \
                                        lcd_command(((i) & 0x0F))
  #define LCD_GOTO_ADDRESS(page,col)    lcd_command(LCD_PAGE_ADDRESS_LSB | ((page) & 0x0F)); \
                                        lcd_command(LCD_PAGE_ADDRESS_MSB | (((page)>>4) & 0x0F)); \
                                        lcd_command(LCD_COL_ADDRESS | (((col)>>4) & 0x0F)); \
                                        lcd_command(((col) & 0x0F))

  #define LCD_NOP()                     lcd_command(LCD_NO_OP)
  #define LCD_SET_BIAS_RATIO(i)         lcd_command(LCD_BIAS_RATIO | ((i) & 0x3))
  #define LCD_SET_COM_END(i)            lcd_command(LCD_COM_END); \
                                        lcd_command(i)
  #define LCD_SET_PARTIAL_DISPLAY(start,end) \
                                        lcd_command(LCD_PARTIAL_START); \
                                        lcd_command(start); \
                                        lcd_command(LCD_PARTIAL_END); \
                                        lcd_command(end)
  #define LCD_SET_PROGRAM_WINDOW(startpage,startcol,endpage,endcol) \
                                        lcd_command(LCD_WINDOW_START_PAGE); \
                                        lcd_command(startpage); \
                                        lcd_command(LCD_WINDOW_START_COL); \
                                        lcd_command(startcol); \
                                        lcd_command(LCD_WINDOW_END_PAGE); \
                                        lcd_command(endpage); \
                                        lcd_command(LCD_WINDOW_END_COL); \
                                        lcd_command(endcol)
  #define LCD_ENABLE_WINDOW_PROGRAM     lcd_command(LCD_WINDOW_PROGRAM | 1)
  #define LCD_DISABLE_WINDOW_PROGRAM    lcd_command(LCD_WINDOW_PROGRAM | 0)
  #define LCD_SET_POTI(i)               lcd_command(LCD_POTENTIOMETER); \
                                        lcd_command(i)
  #define LCD_SYSTEM_RESET              lcd_command(LCD_RESET_CMD)
#endif

 #if DISPLAY_TYPE == 160
  #define LCD_SET_TEMP_COMP(i)          lcd_command(LCD_TEMP_COMP | ((i) & 0x3))
  #define LCD_SET_PANEL_LOAD(i)         lcd_command(LCD_PANEL_LOAD | ((i) & 0x3))
  #define LCD_SET_PUMP_CTRL(i)          lcd_command(LCD_PUMP_CTRL | ((i) & 0x3))
  #define LCD_SET_ADV_PROG_CTRL(i)      lcd_command(LCD_ADV_PROG_CTRL); \
                                        lcd_command((i)&0x3F)  
  #define LCD_SET_START_LINE(i)         lcd_command(LCD_START_LINE | ((i)&0xF)); \
                                        lcd_command(LCD_START_LINE2 | (((i)>>4)&0x7))
  #define LCD_SET_VOLTAGE_BIAS(i)       lcd_command(LCD_VOLTAGE_BIAS); \
                                        lcd_command((i)&0xFF)
  #define LCD_SET_PARTIAL_DISPLAY_CTRL(i) lcd_command(LCD_PARTIAL_CTRL | ((i) & 0x3))
  #define LCD_SET_RAM_ADDR_CTRL(i)      lcd_command(LCD_RAM_ADDR_CTRL | ((i) & 0x7))
  #define LCD_SET_FIXED_LINES(i)        lcd_command(LCD_FIXED_LINES | ((i) & 0xF))
  #define LCD_SET_LINE_RATE(i)          lcd_command(LCD_LINE_RATE | ((i) & 0x3))
  #define LCD_SHOW_ALL_PIXELS_ON()      lcd_command(LCD_ALL_PIXEL | 1)  
  #define LCD_SHOW_ALL_PIXELS_OFF()     lcd_command(LCD_ALL_PIXEL | 0)  
  #define LCD_INVERT_DISPLAY(i)         lcd_command(LCD_INVERSE | ((i)&1))
  #define LCD_SWITCH_ON()               lcd_command(LCD_DISPLAY_ENABLE | 1)
  #define LCD_SWITCH_OFF()              lcd_command(LCD_DISPLAY_ENABLE | 0)
  #define LCD_SET_MAPPING_CTRL(i)       lcd_command(LCD_MAPPING_CTRL | ((i) & 0x7))  
  #define LCD_SET_BOTTOM_VIEW()         lcd_command(LCD_MAPPING_CTRL | 0)  
  #define LCD_SET_TOP_VIEW()            lcd_command(LCD_MAPPING_CTRL | 6)  
  #define LCD_SET_GRAY_SHADE(i)         lcd_command(LCD_GRAY_SHADE | ((i) & 0x3))
  #define LCD_SET_PAGE_ADDR(i)          lcd_command(LCD_PAGE_ADDRESS | ((i) & 0x1F))
  #define LCD_SET_COLUMN_ADDR(col)      lcd_command(LCD_COL_ADDRESS | ((((col)+SHIFT_ADDR)>>4) & 0x0F)); \
                                        lcd_command((((col)+SHIFT_ADDR) & 0x0F))
  #define LCD_GOTO_ADDRESS(page,col)    lcd_command(LCD_PAGE_ADDRESS | ((page) & 0x1F)); \
                                        lcd_command(LCD_COL_ADDRESS | ((((col)+SHIFT_ADDR)>>4) & 0x0F)); \
                                        lcd_command((((col)+SHIFT_ADDR) & 0x0F))

  #define LCD_NOP()                     lcd_command(LCD_NO_OP)
  #define LCD_SET_BIAS_RATIO(i)         lcd_command(LCD_BIAS_RATIO | ((i) & 0x3))
  #define LCD_SET_CURSOR_UPDATE_MODE    lcd_command(LCD_CURSOR_MODE_SET)
  #define LCD_RESET_CURSOR_UPDATE_MODE  lcd_command(LCD_CURSOR_MODE_RESET)
  #define LCD_SET_COM_END(i)            lcd_command(LCD_COM_END); \
                                        lcd_command(i)
  #define LCD_SET_PARTIAL_DISPLAY(start,end) \
                                        lcd_command(LCD_PARTIAL_START); \
                                        lcd_command((start) & 0x7F) \
                                        lcd_command(LCD_PARTIAL_END); \
                                        lcd_command((end) & 0x7F)
  #define LCD_SET_PROGRAM_WINDOW(startpage,startcol,endpage,endcol) \
                                        lcd_command(LCD_WINDOW_START_PAGE); \
                                        lcd_command(startpage); \
                                        lcd_command(LCD_WINDOW_START_COL); \
                                        lcd_command(startcol); \
                                        lcd_command(LCD_WINDOW_END_PAGE); \
                                        lcd_command(endpage); \
                                        lcd_command(LCD_WINDOW_END_COL); \
                                        lcd_command(endcol)
  #define LCD_ENABLE_WINDOW_PROGRAM     lcd_command(LCD_WINDOW_PROGRAM | 1)
  #define LCD_DISABLE_WINDOW_PROGRAM    lcd_command(LCD_WINDOW_PROGRAM | 0)
#endif 
 
#if DISPLAY_TYPE == 128 || DISPLAY_TYPE == 132 || DISPLAY_TYPE == 102
  #define LCD_SWITCH_ON()              lcd_command(LCD_DISPLAY_ENABLE | 1)
  #define LCD_SWITCH_OFF()             lcd_command(LCD_DISPLAY_ENABLE | 0)
  #define LCD_SET_FIRST_LINE(i)        lcd_command(LCD_START_LINE | ((i) & 0x3F))
  #define LCD_SET_BOTTOM_VIEW()        lcd_command(LCD_BOTTOMVIEW | 1)
  #define LCD_SET_TOP_VIEW()           lcd_command(LCD_BOTTOMVIEW | 0)
  #define LCD_SET_MODE_POSITIVE()      lcd_command(LCD_DISPLAY_INVERT | 0)
  #define LCD_SET_MODE_INVERTED()      lcd_command(LCD_DISPLAY_INVERT | 1)
  #define LCD_SHOW_ALL_PIXELS_ON()     lcd_command(LCD_ALL_PIXEL | 1)
  #define LCD_SHOW_ALL_PIXELS_OFF()    lcd_command(LCD_ALL_PIXEL | 0)
  #define LCD_SET_BIAS_RATIO_1_7()     lcd_command(LCD_BIAS | 1)
  #define LCD_SET_BIAS_RATIO_1_9()     lcd_command(LCD_BIAS | 0)
  #define LCD_SEND_RESET()             lcd_command(LCD_RESET_CMD)
  #define LCD_ORIENTATION_NORMAL()     lcd_command(LCD_SCAN_DIR | 0x0)
  #define LCD_ORIENTATION_UPSIDEDOWN() lcd_command(LCD_SCAN_DIR | 0x8)
  #define LCD_SET_POWER_CONTROL(i)     lcd_command(LCD_POWER_CONTROL | ((i) & 0x07))
  #define LCD_SET_LOW_POWER()          lcd_command(LCD_POWER_CONTROL | 0x7)
  #define LCD_SET_WIDE_RANGE()         lcd_command(LCD_POWER_CONTROL | 0x7)
  #define LCD_SET_LOW_VOLTAGE()        lcd_command(LCD_POWER_CONTROL | 0x3)
  #define LCD_SET_BIAS_VOLTAGE(i)      lcd_command(LCD_VOLTAGE | ((i) & 0x07))                                   
  #define LCD_SET_VOLUME_MODE(i)       lcd_command(LCD_VOLUME_MODE); \
                                       lcd_command(((i) & 0x3F))       
  #define LCD_NOP()                    lcd_command(LCD_NO_OP)
#endif

#if DISPLAY_TYPE == 128 || DISPLAY_TYPE == 132
  #define LCD_SET_PAGE_ADDR(i)         lcd_command(LCD_PAGE_ADDRESS | ((i) & 0x0F))
  #define LCD_SET_COLUMN_ADDR(col)     lcd_command(LCD_COL_ADDRESS | ((((col)+SHIFT_ADDR)>>4) & 0x0F)); \
                                       lcd_command((((col)+SHIFT_ADDR) & 0x0F))
  #define LCD_GOTO_ADDRESS(page,col)   lcd_command(LCD_PAGE_ADDRESS | ((page) & 0x1F)); \
                                       lcd_command(LCD_COL_ADDRESS | ((((col)+SHIFT_ADDR)>>4) & 0x0F)); \
                                       lcd_command((((col)+SHIFT_ADDR) & 0x0F))
  #define LCD_SET_INDICATOR_OFF()      lcd_command(LCD_INDICATOR | 0); \
                                       lcd_command(0x00)
  #define LCD_SET_INDICATOR_STATIC()   lcd_command(LCD_INDICATOR | 1); \
                                       lcd_command(0x11)
  #define LCD_SET_INDICATOR_1HZ()      lcd_command(LCD_INDICATOR | 1); \
                                       lcd_command(0x01)
  #define LCD_SET_INDICATOR_2HZ()      lcd_command(LCD_INDICATOR | 1); \
                                       lcd_command(0x10)
  #define LCD_SET_INDICATOR(i,j)       lcd_command(LCD_INDICATOR | ((i) & 1)); \
                                       lcd_command(((j) & 2))
  #define LCD_SET_BOOSTER_MODE(i)      lcd_command(LCD_BOOSTER_SET); \
                                       lcd_command(((i) & 0x03))  
  #define LCD_SET_BOOSTER_MODE_234     lcd_command(LCD_BOOSTER_SET); \
                                       lcd_command(0x0)  
  #define LCD_SET_BOOSTER_MODE_5       lcd_command(LCD_BOOSTER_SET); \
                                       lcd_command(0x1)  
  #define LCD_SET_BOOSTER_MODE_6       lcd_command(LCD_BOOSTER_SET); \
                                       lcd_command(0x3)  
  #define LCD_SLEEP_MODE               lcd_command(LCD_INDICATOR_OFF); \
                                       lcd_command(LCD_DISPLAY_ENABLE | 0); \
                                       lcd_command(LCD_ALL_PIXEL | 1)
#endif

#if DISPLAY_TYPE == 102
  #define LCD_SET_PAGE_ADDR(i)         lcd_command(LCD_PAGE_ADDRESS | ((i) & 0x0F))
  #define LCD_SET_COLUMN_ADDR(col)     lcd_command((((col)+SHIFT_ADDR) & 0x0F)); \
                                       lcd_command(LCD_COL_ADDRESS | ((((col)+SHIFT_ADDR)>>4) & 0x0F))
  #define LCD_GOTO_ADDRESS(page,col)   lcd_command(LCD_PAGE_ADDRESS | ((page) & 0x0F)); \
                                       lcd_command((((col)+SHIFT_ADDR) & 0x0F)); \
                                       lcd_command(LCD_COL_ADDRESS | ((((col)+SHIFT_ADDR)>>4) & 0x0F))
  #define LCD_TEMPCOMP_HIGH  0x80
  #define LCD_COLWRAP        0x02
  #define LCD_PAGEWRAP       0x01
  #define LCD_SET_ADV_PROG_CTRL(i)     lcd_command(LCD_ADV_PROG_CTRL); \
                                       lcd_command(LCD_ADV_PROG_CTRL2 | i)
#endif




 
/*****************************************************************************
 * Output pin controlling makros
 *****************************************************************************/

//Control A0 input of LCD
#define LCD_DRAM()            PORT_A0 |= _BV(PIN_A0)
#define LCD_CMD()             PORT_A0 &= ~_BV(PIN_A0)
#define LCD_SET_OUTPUT_A0()   DDR_A0  |= _BV(PIN_A0)

//Control reset input of LCD
#define LCD_RESET_OFF()       PORT_RST |= _BV(PIN_RST)
#define LCD_RESET_ON()        PORT_RST &= ~_BV(PIN_RST)
#define LCD_SET_OUTPUT_RST()  DDR_RST |= _BV(PIN_RST)
#define LCD_RESET()           LCD_RESET_ON();  _delay_ms(1); LCD_RESET_OFF();  _delay_ms(10)

//Control pin for Backlight
#if LCD_USE_BACKLIGHT == 1      //normal
  #define LCD_SET_OUTPUT_LED() DDR_LED |= _BV(PIN_LED)
  #define BACKLIGHT_ON()       PORT_LED |= (1<<PIN_LED)
  #define BACKLIGHT_OFF()      PORT_LED &= ~(1<<PIN_LED)
#elif LCD_USE_BACKLIGHT == 2    //inverted
  #define LCD_SET_OUTPUT_LED() DDR_LED |= _BV(PIN_LED)
  #define BACKLIGHT_ON()       PORT_LED &= ~(1<<PIN_LED)
  #define BACKLIGHT_OFF()      PORT_LED |= (1<<PIN_LED)
#else
  #define LCD_SET_OUTPUT_LED() 
  #define BACKLIGHT_ON()   
  #define BACKLIGHT_OFF()    
#endif

//Control pin for chip select
#if LCD_USE_CHIPSELECT == 1
  #define LCD_SET_OUTPUT_CS()  DDR_CS  |= _BV(PIN_CS)
  #define LCD_SELECT()         PORT_CS &= ~_BV(PIN_CS)
  #define LCD_UNSELECT()       spi_wait_for_idle(); PORT_CS |= _BV(PIN_CS)
#else
  #define LCD_SET_OUTPUT_CS()  
  #define LCD_SELECT()         spi_wait_for_idle();
  #define LCD_UNSELECT()   
#endif

//combined direction selection for all pins
#define LCD_SET_PIN_DIRECTIONS()   LCD_SET_OUTPUT_A0(); \
                                   LCD_SET_OUTPUT_RST(); \
                                   LCD_SET_OUTPUT_LED(); \
                                   LCD_SET_OUTPUT_CS()


//Bit positions for style settings
#define NORMAL      0
#define INVERT      4
#define INVERT_BIT  4


#endif

