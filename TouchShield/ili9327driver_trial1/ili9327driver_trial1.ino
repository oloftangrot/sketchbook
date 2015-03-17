#include "ili9327driver.h"


void fillCircle(int x, int y, int radius)
{
  for( int y1 = -radius; y1 <= 0; y1++ ) 
    for( int x1 = -radius; x1 <= 0; x1++ )
      if( x1 * x1 + y1 * y1 <= radius * radius ) {
//        drawHLine(x+x1, y+y1, 2*(-x1));
//        drawHLine(x+x1, y-y1, 2*(-x1));
        ili9327::H_line(x+x1, y+y1, 2*(-x1), 255 );
        ili9327::H_line(x+x1, y-y1, 2*(-x1), 255 );
	break;
      }
}

void setup( void ) {
  for(int p = 2; p < 10; p++ )
  {
    pinMode( p, OUTPUT );
  }
  ili9327::Lcd_Init();
  ili9327::Lcd_Clear( 0x00 );
  fillCircle( 100, 100, 50 );
}

void loop( void ) {
  
}


