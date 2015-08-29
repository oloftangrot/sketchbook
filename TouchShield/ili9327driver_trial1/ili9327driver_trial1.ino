#include "ili9327driver.h"
#include "ili9327driver_ext.h"

extern uint8_t SevenSegNumFont[];

void setup( void ) {
  for(int p = 2; p < 10; p++ )
  {
    pinMode( p, OUTPUT );
  }
  ili9327::Lcd_Init();
  ili9327::Lcd_Clear( VGA_MAROON );
  ili9327_ext::setFont( SevenSegNumFont );
  ili9327_ext::fillCircle( 100, 100, 50 );
  ili9327_ext::drawRoundRect( 100, 200, 120, 220 );
  
}

void loop( void ) {
  ili9327::Lcd_Clear( VGA_MAROON );
  for ( int i = 100; i < 200; i++ ) {
    for ( int j = 100; j < 200; j++ ) {
      ili9327::Plot( i, j, 255 );
    }
  }
  ili9327_ext::setBackColor(128, 0, 0);

  ili9327_ext::setColor( 255, 191, 0 );
  ili9327_ext::print( "0123456789", RIGHT, 100 );
//  ili9327_ext::test();
}



