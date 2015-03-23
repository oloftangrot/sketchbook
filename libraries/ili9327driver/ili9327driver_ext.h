#ifndef __ILI9327_EXT_H__
#define __ILI9327_EXT_H__
#define PORTRAIT 0
#define LANDSCAPE 1
#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

namespace ili9327_ext {

void fillCircle( int x, int y, int radius );
void drawRoundRect( int x1, int y1, int x2, int y2 );
void setFont( unsigned char* font );
void print( char *st, int x, int y, int deg = 0 );
void setColor( unsigned char r, unsigned char g, unsigned char b );
void setBackColor( unsigned char r, unsigned char g, unsigned char b );
void test( void );
}
#endif











