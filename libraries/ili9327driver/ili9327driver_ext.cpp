#include "Arduino.h"
#include "ili9327driver.h"
#include "ili9327driver_ext.h"
struct current_font
{
	uint8_t* font;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
};


using namespace ili9327;
//
//   This is a driver for the TFT 3.5 Inch 400x240 touch shield from http://www.mcufriends.com
//   Since the shield is fixed the wiring defines is given here and
//   there is no interface to define them using an API call. The graphic controller is controlled 
//   in 8-bit mode.
//
namespace ili9327_ext {
unsigned int col = 255;

boolean	transparent = true;
unsigned char orient = LANDSCAPE;
//unsigned char orient = PORTRAIT;
unsigned char	fch, fcl, bch, bcl;
const int disp_y_size = 240;
const int disp_x_size = 400;
struct current_font	cfont;

void setColor( unsigned char r, unsigned char g, unsigned char b )
{
	fch = ( ( r & 248 ) | g >> 5 );
	fcl = ( ( g & 28 ) << 3 | b >> 3 );
}

void setBackColor( unsigned char r, unsigned char g, unsigned char b)
{
	bch = ( ( r & 248 ) | g >> 5 );
	bcl = ( ( g & 28 ) << 3 | b >> 3 );
	transparent = false;
}

void setXY(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	if ( orient == LANDSCAPE )
	{
    unsigned int tmp;
    tmp = x1;
    x1 = y1;
    y1 = tmp;
    tmp = x2;
    x2 = y2;
    y2 = tmp;
		y1  = disp_y_size-y1;
		y2 = disp_y_size-y2;
    tmp = y1;
    y1 = y2;
    y2 = tmp;
	}
	Address_set( x1, y1, x2, y2 ); // Use existing code...TODO or modify existing parsing orient??
}

void clrXY( void )
{
	if ( orient == PORTRAIT )
		setXY(0, 0, disp_x_size, disp_y_size );
	else
		setXY(0, 0, disp_y_size, disp_x_size );
}
#if 0
void setPixel(unsigned int color) 
{
  Lcd_Writ_Bus( color>>8 ) ; // Make two 8-bit transfers.
	Lcd_Writ_Bus( color & 0xff ) ;	// rrrrrggggggbbbbb
}
#endif
void setPixel(unsigned char hiCol, unsigned char loCol ) 
{
  Lcd_Writ_Bus( hiCol ) ; // Make two 8-bit transfers.
	Lcd_Writ_Bus( loCol ) ;	// rrrrrggggggbbbbb
}

void fillCircle(int x, int y, int radius)
{
  for( int y1 = -radius; y1 <= 0; y1++ ) 
    for( int x1 = -radius; x1 <= 0; x1++ )
      if( x1 * x1 + y1 * y1 <= radius * radius ) {
//        drawHLine(x+x1, y+y1, 2*(-x1));
//        drawHLine(x+x1, y-y1, 2*(-x1));
        H_line(x+x1, y+y1, 2*(-x1), col );
        H_line(x+x1, y-y1, 2*(-x1), col );
				break;
      }
}

void drawRoundRect(int x1, int y1, int x2, int y2)
{
	if ( x1 > x2 ) { // swap
		int tmp = x1;
    x1 = x2;
    x2 = tmp;
	}
	if ( y1 > y2 ) { // swap
	  int tmp = y1;
    y1 = y2;
    y2 = tmp;
	}
	if ((x2-x1)>4 && (y2-y1)>4)
	{
		Plot( x1 + 1, y1 + 1, col );
		Plot( x2 - 1, y1 + 1, col );
		Plot( x1 + 1, y2 - 1, col );
		Plot( x2 - 1, y2 - 1, col );
		H_line( x1 + 2, y1,     x2 - x1 - 4, col );
		H_line( x1 + 2, y2,     x2 - x1 - 4, col );
		V_line( x1,     y1 + 2, y2 - y1 - 4, col );
		V_line( x2,     y1 + 2, y2 - y1 - 4, col );
	}
}

void printChar(byte c, int x, int y)
{
	unsigned char i, ch;
	int j;
	int temp; 

	cbi(P_CS, B_CS);
  
	if ( !transparent ) {
		if (orient == PORTRAIT) {
			setXY( x, y, x + cfont.x_size - 1, y + cfont.y_size - 1 );
  		sbi( PORTC, LCD_RS_BitMask ); // digitalWrite( LCD_RS, HIGH );
			temp = ( ( c - cfont.offset ) * ( ( cfont.x_size / 8 ) * cfont.y_size ) ) + 4;
			for ( j = 0; j < ( ( cfont.x_size / 8 ) * cfont.y_size); j++ ) {
				ch = pgm_read_byte ( &cfont.font[temp] );
				for ( i = 0; i < 8; i++ ) {   
					if ( ch & ( 1 << 7 ) ) { // Mask out the highest bit
						setPixel( fch, fcl );
					} 
					else {
						setPixel( bch, bcl );
					}
					ch <<= 1; // Shift up the next bit.
				}
				temp++;
			}
		}
		else
		{
			temp = ( ( c - cfont.offset ) * ( ( cfont.x_size / 8 ) * cfont.y_size ) ) + 4;

			for ( j = 0; j < ( ( cfont.x_size / 8 ) * cfont.y_size ); j += ( cfont.x_size / 8 ) ) {
				setXY( x, y + ( j / (cfont.x_size / 8 ) ), x + cfont.x_size - 1, y + ( j / ( cfont.x_size / 8 ) ) );
				sbi( PORTC, LCD_RS_BitMask ); // digitalWrite( LCD_RS, HIGH );
				for ( int zz = ( cfont.x_size / 8 ) - 1; zz >= 0; zz-- ) {
					ch = pgm_read_byte ( &cfont.font[temp+zz] );
					for ( i = 0; i < 8; i++ ) {   
						if ( ( ch & 1 ) != 0 ) {
							setPixel( fch, fcl );
						} 
						else {
							setPixel( bch, bcl );
						}
						ch >>= 1; 
					}
				}
				temp += cfont.x_size / 8;
			}
		}
	}
	else
	{
		temp = ( ( c - cfont.offset ) * ( ( cfont.x_size / 8 ) * cfont.y_size ) ) + 4;
		for ( j = 0; j < cfont.y_size; j++ ) {
			for ( int zz = 0; zz < ( cfont.x_size / 8 ); zz++ ) {
				ch = pgm_read_byte( &cfont.font[temp + zz] ); 
				for( i = 0; i < 8; i++ ) {   
					setXY( x + i + ( zz * 8 ), y + j, x + i + ( zz * 8) + 1, y + j + 1 );
  				sbi( PORTC, LCD_RS_BitMask ); // digitalWrite( LCD_RS, HIGH );
					if ( ch & ( 1<< 7 ) ) {
						setPixel( fch, fcl );
					} 
					ch <<= 1;
				}
			}
			temp += cfont.x_size / 8;
		}
	}
	sbi(P_CS, B_CS);
	clrXY();
}

void setFont(uint8_t* font)
{
	cfont.font = font;
	cfont.x_size = pgm_read_byte( &cfont.font[0] ) ;
	cfont.y_size = pgm_read_byte( &cfont.font[1] ) ;
	cfont.offset = pgm_read_byte( &cfont.font[2] ) ;
	cfont.numchars = pgm_read_byte( &cfont.font[3] );
}

void print(char *st, int x, int y, int deg)
{
	int stl, i;

	stl = strlen(st);

	if ( orient == PORTRAIT ) {
		if ( x == RIGHT )
			x = ( disp_x_size + 1 ) - ( stl * cfont.x_size );
		if ( x == CENTER )
			x = ( ( disp_x_size + 1 ) - ( stl * cfont.x_size ) ) / 2;
	}
	else {
		if ( x == RIGHT )
			x = ( disp_y_size + 1 ) - ( stl * cfont.x_size );
		if ( x == CENTER )
			x = ( ( disp_y_size + 1 ) - ( stl * cfont.x_size ) ) / 2;
	}

	for ( i = 0; i < stl; i++ )
		if ( deg == 0 )
			printChar( *st++, x + ( i * ( cfont.x_size ) ), y );
//		else
//			rotateChar( *st++, x, y, i, deg );
}

}
