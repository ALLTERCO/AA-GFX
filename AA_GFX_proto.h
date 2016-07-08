/***************************************************
 * This is an LGPL attempt to port the excelent Adafruit library 
 * from C++ to C for platforms lacking C++ or Arduino support 
 * 
 * This library is Copyright (c) 2016, Allterco, Inc.  All rights reserved
 * 
 ****************************************************/

/**********
 * T his library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 **********/

//Disclimer (sans licens lines) from C++ sources written  Adafruit Industries 
/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!
*/






#ifdef GFX_drawCircle
void GFX_drawCircle (GFX_displayInfo_t *di,int16_t x0, int16_t y0, int16_t r, uint16_t color);
#endif
#ifdef GFX_drawCircleHelper
void GFX_drawCircleHelper(GFX_displayInfo_t *di, int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
#endif

#ifdef GFX_fillCircle
void GFX_fillCircle(GFX_displayInfo_t *di,int16_t x0, int16_t y0, int16_t r, uint16_t color);
#endif

#ifdef  GFX_fillCircleHelper
// Used to do circles and roundrects
void GFX_fillCircleHelper(GFX_displayInfo_t *di,int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
#endif

#ifdef GFX_drawLine
// Bresenham's algorithm - thx wikpedia
void GFX_drawLine(GFX_displayInfo_t *di,int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
#endif

#ifdef GFX_drawRect
// Draw a rectangle
void GFX_drawRect (GFX_displayInfo_t *di,int16_t x, int16_t y, int16_t w, int16_t h,uint16_t color);
#endif

#ifdef GFX_drawRoundRect
// Draw a rounded rectangle
void GFX_drawRoundRect(GFX_displayInfo_t *di,int16_t x, int16_t y, int16_t w,int16_t h, int16_t r, uint16_t color);
#endif

#ifdef GFX_fillRoundRect
// Fill a rounded rectangle
void GFX_fillRoundRect(GFX_displayInfo_t *di,int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
#endif

#ifdef GFX_drawTriangle
// Draw a triangle
void GFX_drawTriangle(GFX_displayInfo_t *di,int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
#endif

#ifdef GFX_fillTriangle
// Fill a triangle
void GFX_fillTriangle (GFX_displayInfo_t *di,int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) ;
#endif

#ifdef GFX_drawBitmapT
// Draw a 1-bit image (bitmap) at the specified (x,y) position from the
// provided bitmap buffer using the specified
// foreground color (unset bits are transparent). Each line starts on byte
void GFX_drawBitmapT(GFX_displayInfo_t *di,int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
#endif



#ifdef GFX_drawBitmapBG
// Draw a 1-bit image (bitmap) at the specified (x,y) position from the
// provided bitmap buffer  using the specified
// foreground (for set bits) and background (for clear bits) colors. Each line starts on byte
void GFX_drawBitmapBG(GFX_displayInfo_t *di,int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) ;
#endif




#ifdef GFX_drawXBitmapT
//Draw XBitMap Files (*.xbm), exported from GIMP, unset bits are transparent
//Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
//C Array can be directly used with this function
void GFX_drawXBitmapT(GFX_displayInfo_t *di,int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
#endif

#ifdef GFX_drawXBitmapBG
//Draw XBitMap Files (*.xbm), exported from GIMP,
//Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
//C Array can be directly used with this function
void GFX_drawXBitmapBG(GFX_displayInfo_t *di,int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) ;
#endif

#ifdef GFX_drawChar
// Draw a character
void GFX_drawChar(GFX_displayInfo_t *di,  GFXfont* gfxFont, uint8_t flags, int16_t x, int16_t y,  unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
#endif
