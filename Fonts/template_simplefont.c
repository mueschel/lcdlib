/*
    created with FontEditor written by H. Reddmann   HaReddmann at t-online dot de
    using template file by J. Michel     jan at mueschelsoft.de

    File Name           : %0:s.c
    Date                : %1:s
    Font size in bytes  : 0x%2:0.2x%3:0.2x, %9:d
    Font width          : %4:d
    Font height         : %5:d
    Font first char     : 0x%7:0.2x
    Font last char      : 0x%8:0.2x
    Font bits per pixel : %10:d
*/

#include "../font.h"
#ifdef FONTS_INCLUDE_%0:s



const uint8_t %0:s_data[] PROGMEM = {
%12:s
};

const struct font_info %0:s PROGMEM = {0x%3:0.2x+(0x%2:0.2x<<8),0x%4:0.2x,0x%5:0.2x,0x%7:0.2x,0x%8:0.2x,%0:s_data,(uint8_t*)%0:s_data+0x%8:0.2x-0x%7:0.2x+1};

#endif


