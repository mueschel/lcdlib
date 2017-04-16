lcdlib
======

Library for EA-DOG LCDs, including font-generator (also usable with color LCD)
Further information and discussion (in german) can be found at:
http://www.mikrocontroller.net/topic/144500

The library works with various EA-DOG LCDs (see below) and ILI9341-based colour LCD
using AVR microcontrollers. A version to be used with Arduino and e.g. a ESP8266 module
is available in https://github.com/mueschel/lcdlib-arduino .


Files
=====

dogm-graphic .c/.h
------------------

Provides all basic functions to access the display for
- EA-DOGS102  GLCD (102px x 64px)
- EA-DOGM128  GLCD (128px x 64px)
- EA-DOGM132  GLCD (132px x 32px)
- EA-DOGL128  GLCD (128px x 64px)
- EA-DOGXL160 GLCD (160px x 104px)
- EA-DOGXL240 GLCD (240px x 128px)
- can be easily adapted for other displays with similar characteristics
     
Since no graphics ram is used, the memory footprint is rather small but
also does not allow to change single pixels. Data written to the LCD can
not be read back! Because of that, no drawing functions (lines, circles) 
are included.
Thanks to Oliver Schwaneberg for adding several functions to this library!
 
 
font .c/.h
----------
Font Generator
designed for GLCD with memory organized in vertical bytes

Own character sets or symbols can easily be added:
The structure of font data is compatible with Hagen Reddmanns FontEditor
http://www.mikrocontroller.net/topic/99701#865331
when using the attached template_simplefont.c and template_simplefont.h
files, saving in uncompressed format and using font heights of multiples
of 8.
 
Fixed width fonts are treated as proportional fonts, but do not have a 
table for the width of each character (D'OH!)

When using with other GLCDs, make sure the byte orientation of the LCDs
memory matches the design of the ea-dogm series or link the LCD access
functions (see header file) to functions converting the data.
 

lcd-color-graphic .c/.h
-----------------------
An alternative lcd library controlling a LCD with an ILI9341 driver in 
16 Bit color mode using 4-wire SPI or any other display with similar
features. Just use this file instead of dogm-graphic. The function
lcd_write_font_byte() provides the conversion from the font-generator
byte format to the format used by the display.

 
Fonts/template_simplefont.c
---------------------------
A template file to be used with Hagen Reddmanns FontEditor to create 
compatible fonts.

