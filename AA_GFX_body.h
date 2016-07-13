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

#ifdef GFX_rawImgFromFile 
#include <stdio.h>
#include <stdlib.h>
#endif

#ifndef abs
#define abs(x) ((x)<0 ? -(x) : (x))
#endif
#ifndef _swap_int16_t
#define _swap_int16_t(a, b) {int16_t _swap_int16_t_swap_tmp=a;a=b;b=_swap_int16_t_swap_tmp; }
#endif

#ifdef GFX_drawCircle
void GFX_drawCircle (GFX_displayInfo_t *di, int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  GFX_drawPixel(di,x0  , y0+r, color);
  GFX_drawPixel(di,x0  , y0-r, color);
  GFX_drawPixel(di,x0+r, y0  , color);
  GFX_drawPixel(di,x0-r, y0  , color);

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    GFX_drawPixel(di,x0 + x, y0 + y, color);
    GFX_drawPixel(di,x0 - x, y0 + y, color);
    GFX_drawPixel(di,x0 + x, y0 - y, color);
    GFX_drawPixel(di,x0 - x, y0 - y, color);
    GFX_drawPixel(di,x0 + y, y0 + x, color);
    GFX_drawPixel(di,x0 - y, y0 + x, color);
    GFX_drawPixel(di,x0 + y, y0 - x, color);
    GFX_drawPixel(di,x0 - y, y0 - x, color);
  }
}
#endif


#ifdef GFX_drawCircleHelper
void GFX_drawCircleHelper(GFX_displayInfo_t *di, int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;
    if (cornername & 0x4) {
      GFX_drawPixel(di,x0 + x, y0 + y, color);
      GFX_drawPixel(di,x0 + y, y0 + x, color);
    }
    if (cornername & 0x2) {
      GFX_drawPixel(di,x0 + x, y0 - y, color);
      GFX_drawPixel(di,x0 + y, y0 - x, color);
    }
    if (cornername & 0x8) {
      GFX_drawPixel(di,x0 - y, y0 + x, color);
      GFX_drawPixel(di,x0 - x, y0 + y, color);
    }
    if (cornername & 0x1) {
      GFX_drawPixel(di,x0 - y, y0 - x, color);
      GFX_drawPixel(di,x0 - x, y0 - y, color);
    }
  }
}
#endif

#ifdef GFX_fillCircle
void GFX_fillCircle(GFX_displayInfo_t *di,int16_t x0, int16_t y0, int16_t r, uint16_t color) {
  GFX_drawFastVLine(di,x0, y0-r, 2*r+1, color);
  GFX_fillCircleHelper(di,x0, y0, r, 3, 0, color);
}
#endif

#ifdef  GFX_fillCircleHelper
// Used to do circles and roundrects
void GFX_fillCircleHelper(GFX_displayInfo_t *di,int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {
  int16_t f     = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x     = 0;
  int16_t y     = r;

  while (x<y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f     += ddF_y;
    }
    x++;
    ddF_x += 2;
    f     += ddF_x;

    if (cornername & 0x1) {
      GFX_drawFastVLine(di,x0+x, y0-y, 2*y+1+delta, color);
      GFX_drawFastVLine(di,x0+y, y0-x, 2*x+1+delta, color);
    }
    if (cornername & 0x2) {
      GFX_drawFastVLine(di,x0-x, y0-y, 2*y+1+delta, color);
      GFX_drawFastVLine(di,x0-y, y0-x, 2*x+1+delta, color);
    }
  }
}
#endif 

#ifdef GFX_drawLine
// Bresenham's algorithm - thx wikpedia
void GFX_drawLine(GFX_displayInfo_t *di,int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1) {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      GFX_drawPixel(di,y0, x0, color);
    } else {
      GFX_drawPixel(di,x0, y0, color);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}
#endif


#ifdef GFX_drawRect
// Draw a rectangle
void GFX_drawRect (GFX_displayInfo_t *di,int16_t x, int16_t y, int16_t w, int16_t h,uint16_t color) {
  GFX_drawFastHLine(di,x, y, w, color);
  GFX_drawFastHLine(di,x, y+h-1, w, color);
  GFX_drawFastVLine(di,x, y, h, color);
  GFX_drawFastVLine(di,x+w-1, y, h, color);
}
#endif



#ifdef GFX_drawRoundRect
// Draw a rounded rectangle
void GFX_drawRoundRect(GFX_displayInfo_t *di,int16_t x, int16_t y, int16_t w,int16_t h, int16_t r, uint16_t color) {
  // smarter version
  GFX_drawFastHLine(di,x+r  , y    , w-2*r, color); // Top
  GFX_drawFastHLine(di,x+r  , y+h-1, w-2*r, color); // Bottom
  GFX_drawFastVLine(di,x    , y+r  , h-2*r, color); // Left
  GFX_drawFastVLine(di,x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
  GFX_drawCircleHelper(di,x+r    , y+r    , r, 1, color);
  GFX_drawCircleHelper(di,x+w-r-1, y+r    , r, 2, color);
  GFX_drawCircleHelper(di,x+w-r-1, y+h-r-1, r, 4, color);
  GFX_drawCircleHelper(di,x+r    , y+h-r-1, r, 8, color);
}
#endif

#ifdef GFX_fillRoundRect
// Fill a rounded rectangle
void GFX_fillRoundRect(GFX_displayInfo_t *di,int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
  // smarter version
  GFX_fillRect(di,x+r, y, w-2*r, h, color);

  // draw four corners
  GFX_fillCircleHelper(di,x+w-r-1, y+r, r, 1, h-2*r-1, color);
  GFX_fillCircleHelper(di,x+r    , y+r, r, 2, h-2*r-1, color);
}
#endif

#ifdef GFX_drawTriangle
// Draw a triangle
void GFX_drawTriangle(GFX_displayInfo_t *di,int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
  GFX_drawLine(di,x0, y0, x1, y1, color);
  GFX_drawLine(di,x1, y1, x2, y2, color);
  GFX_drawLine(di,x2, y2, x0, y0, color);
}
#endif

#ifdef GFX_fillTriangle
// Fill a triangle
void GFX_fillTriangle (GFX_displayInfo_t *di,int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {

  int16_t a, b, y, last;

  // Sort coordinates by Y order (y2 >= y1 >= y0)
  if (y0 > y1) {
    _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
  }
  if (y1 > y2) {
    _swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
  }
  if (y0 > y1) {
    _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
  }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    GFX_drawFastHLine(di,a, y0, b-a+1, color);
    return;
  }

  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
  int32_t
    sa   = 0,
    sb   = 0;

  // For upper part of triangle, find scanline crossings for segments
  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
  // is included here (and second loop will be skipped, avoiding a /0
  // error there), otherwise scanline y1 is skipped here and handled
  // in the second loop...which also avoids a /0 error here if y0=y1
  // (flat-topped triangle).
  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;
    /* longhand:
    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) _swap_int16_t(a,b);
    GFX_drawFastHLine(di,a, y, b-a+1, color);
  }

  // For lower part of triangle, find scanline crossings for segments
  // 0-2 and 1-2.  This loop is skipped if y1=y2.
  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;
    /* longhand:
    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
    */
    if(a > b) _swap_int16_t(a,b);
    GFX_drawFastHLine(di,a, y, b-a+1, color);
  }
}
#endif

#ifdef GFX_drawBitmapT
// Draw a 1-bit image (bitmap) at the specified (x,y) position from the
// provided bitmap buffer using the specified
// foreground color (unset bits are transparent). Each line starts on byte
void GFX_drawBitmapT(GFX_displayInfo_t *di,int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t cbyte=0;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++) {
      if(i & 7) cbyte <<= 1;
      else      cbyte   = bitmap[j * byteWidth + i / 8];
      if(cbyte & 0x80) GFX_drawPixel(di,x+i, y+j, color);
    }
  }
}
#endif

#ifdef GFX_drawBitmapBG
// Draw a 1-bit image (bitmap) at the specified (x,y) position from the
// provided bitmap buffer  using the specified
// foreground (for set bits) and background (for clear bits) colors. Each line starts on byte
void GFX_drawBitmapBG(GFX_displayInfo_t *di,int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {
	//TODO: use uint16_t cbuf[16] and use it in GFXsetWindowCD it will be 32 times faster 
  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t cbyte=0;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(i & 7) cbyte <<= 1;
      else      cbyte   = bitmap[j * byteWidth + i / 8];
      if(cbyte & 0x80) GFX_drawPixel(di,x+i, y+j, color);
      else            GFX_drawPixel(di,x+i, y+j, bg);
    }
  }
}
#endif




#ifdef GFX_drawXBitmapT
//Draw XBitMap Files (*.xbm), exported from GIMP, unset bits are transparent
//Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
//C Array can be directly used with this function
void GFX_drawXBitmapT(GFX_displayInfo_t *di,int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t cbyte=0;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++) {
      if(i & 7) cbyte >>= 1;
      else      cbyte   = bitmap[j * byteWidth + i / 8];
      if(cbyte & 0x01) GFX_drawPixel(di,x+i, y+j, color);
    }
  }
}
#endif

#ifdef GFX_drawXBitmapBG
//Draw XBitMap Files (*.xbm), exported from GIMP,
//Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
//C Array can be directly used with this function
void GFX_drawXBitmapBG(GFX_displayInfo_t *di,int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {
	//TODO: use uint16_t cbuf[16] and use it in GFXsetWindowCD it will be 32 times faster 
  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t cbyte=0;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++ ) {
      if(i & 7) cbyte >>= 1;
      else      cbyte   = bitmap[j * byteWidth + i / 8];
      if(cbyte & 0x01) GFX_drawPixel(di,x+i, y+j, color);
      else            GFX_drawPixel(di,x+i, y+j, bg);
    }
  }
}
#endif

#ifdef GFX_drawChar
// Draw a character
void GFX_drawChar(GFX_displayInfo_t *di, const GFXfont* gfxFont, uint8_t size, unsigned char c,uint8_t flags, int16_t x, int16_t y, uint16_t color, uint16_t bg) {

	if(c == '\r') return;//ignore;
	
	if(c == '\n') {
		if ((flags & GFX_DCF_ABSOLUTE) == 0) {
			di->cursor_x  = 0;
			di->cursor_y += (int16_t)gfxFont->yAdvance;
		}
		return;
	}
	

	
	// Character is assumed previously filtered by write() to eliminate
	// newlines, returns, non-printable characters, etc.  Calling drawChar()
	// directly with 'bad' characters of font may cause mayhem!

	c -= gfxFont->first;
	GFXglyph *glyph  = (GFXglyph *)(gfxFont->glyph+c);
	uint8_t  *bitmap = (uint8_t *)gfxFont->bitmap;

	uint16_t bo = glyph->bitmapOffset;
	uint8_t  w  = glyph->width;
	uint8_t  h  = glyph->height;
//	uint8_t  xa = glyph->xAdvance;
	int8_t   xo = glyph->xOffset;
	int8_t   yo = glyph->yOffset;
	uint8_t  xx, yy, bits=0, bit = 0;
	int16_t  xo16=0, yo16=0;

	if ((flags & GFX_DCF_ABSOLUTE) == 0) { //ofset non absolute writes to currsor
		if (di->wrap) {
			if (di->cursor_x+glyph->xAdvance>di->width) {
				di->cursor_x=0;
				di->cursor_y += (int16_t)gfxFont->yAdvance;
				if (di->cursor_y>di->height) di->cursor_y=0;
			}
		}
		
		x+=di->cursor_x;
		y+=di->cursor_y;
		di->cursor_x+=glyph->xAdvance;
	}
	if (w==0 && h==0) { //no bitmap!
		if (flags & GFX_DCF_SETBG) {
			GFX_fillRect(di,x, y, glyph->xAdvance*(int16_t)size, gfxFont->yAdvance*(int16_t)size, bg);
		}
		return;
	}
	
	if(size > 1) {
		xo16 = xo;
		yo16 = yo;
	}

	// Todo: Add character clipping here

	// NOTE: THERE IS OPTIONAL 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
	// THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
	// has typically been used with the 'classic' font to overwrite old
	// screen contents with new data.  This ONLY works because the
	// characters are a uniform size; it's not a sensible thing to do with
	// proportionally-spaced fonts with glyphs of varying sizes (and that
	// may overlap).  To replace previously-drawn text when using a custom
	// font, use the getTextBounds() function to determine the smallest
	// rectangle encompassing a string, erase the area with fillRect(),
	// then draw new text.  This WILL infortunately 'blink' the text, but
	// is unavoidable.  Drawing 'background' pixels will NOT fix this,
	// only creates a new set of problems.  Have an idea to work around
	// this (a canvas object type for MCUs that can afford the RAM and
	// displays supporting setAddrWindow() and pushColors()), but haven't
	// implemented this yet.

	for(yy=0; yy<h; yy++) {
		for(xx=0; xx<w; xx++) {
			if(!(bit++ & 7)) {
				bits = bitmap[bo++];
			}
			if(bits & 0x80) {
				if(size == 1) GFX_drawPixel(di,x+xo+xx, y+yo+yy, color);
				else GFX_fillRect(di,x+(xo16+xx)*size, y+(yo16+yy)*size, size, size, color);
			} else if (flags & GFX_DCF_SETBG) {
				if(size == 1) GFX_drawPixel(di,x+xo+xx, y+yo+yy, bg);
				else GFX_fillRect(di,x+(xo16+xx)*size, y+(yo16+yy)*size, size, size, bg);
			}
			bits <<= 1;
		}
	}

}
#endif


#ifdef GFX_setCursor 
void GFX_setCursor(GFX_displayInfo_t *di,int16_t x, int16_t y){
	di->cursor_x=x;
	di->cursor_y=y;
}
#endif


#ifdef GFX_rawImgFromFH
uint8_t GFX_rawImgFromFH(GFX_displayInfo_t *di,FILE *f, int16_t x, int16_t y,int16_t w,int16_t h, uint16_t linestobuffer/*=0*/){
	if (linestobuffer==0) linestobuffer=1;
	uint8_t *line_buf=(uint8_t *)(malloc(w*2*linestobuffer));
	if (!line_buf) return GFX_RAWIMGFROMFILE_FAILED_TO_MALLOC;
	
	int16_t linesleft=h;
	int16_t linnes_to_io;
	while (linesleft) {
		linnes_to_io=(linesleft<linestobuffer?linesleft:linestobuffer);
		if (fread(line_buf,2*w,linnes_to_io,f)!=linnes_to_io){
			free(line_buf);
			return GFX_RAWIMGFROMFILE_FAILED_TO_READ;
		};
		GFX_rawImg(di,x,y+(h-linesleft),w,linnes_to_io,line_buf);
		linesleft-=linnes_to_io;
	}
	free(line_buf);
	return GFX_RAWIMGFROMFILE_OK;
} 

#endif

#ifdef GFX_rawImgFromFile
/*
#define GFX_RAWIMGFROMFILE_FAILED_TO_MALLOC 1
#define GFX_RAWIMGFROMFILE_FAILED_TO_OPEN 2
#define GFX_RAWIMGFROMFILE_FAILED_TO_READ 3
#define GFX_RAWIMGFROMFILE_FAILED_TO_SEEK 4
#define GFX_RAWIMGFROMFILE_OK 0
*/
uint8_t GFX_rawImgFromFile(GFX_displayInfo_t *di,const char *fn, int16_t x, int16_t y,int16_t w,int16_t h, uint16_t linestobuffer/*=0*/){
	FILE *f=fopen(fn,"rb");
	if (f) {
		uint8_t res=GFX_rawImgFromFH(di,f,x,y,w,h,linestobuffer);
		fclose(f);
		return res;
	} else {
		return GFX_RAWIMGFROMFILE_FAILED_TO_OPEN;
	}	
}
#endif

#ifdef GFX_rawImgFromSpack
uint8_t GFX_rawImgFromSpack(GFX_displayInfo_t *di,FILE *spf, const spritepac_t *sp, int16_t x, int16_t y, uint16_t linestobuffer/*=0*/){
	if (fseek(spf,sp->ofs,SEEK_SET)==-1) return GFX_RAWIMGFROMFILE_FAILED_TO_SEEK;
	return GFX_rawImgFromFH(di,spf,x,y, sp->w,sp->h,linestobuffer);
}
#endif
