#ifndef __ILI9327_EXT_H__
#define __ILI9327_EXT_H__
#define PORTRAIT 0
#define LANDSCAPE 1
#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

namespace ili9327_ext {
extern const int disp_y_size;
extern const int disp_x_size;

void fillCircle( int x, int y, int radius );
void drawRoundRect( int x1, int y1, int x2, int y2 );
void setFont( unsigned char* font );
void print( char *st, int x, int y, int deg = 0 );
void setColor( unsigned char r, unsigned char g, unsigned char b );
void setColor( unsigned int c );
void setBackColor( unsigned char r, unsigned char g, unsigned char b );
void setBackColor( unsigned int c );
void test( void );
}
#endif











