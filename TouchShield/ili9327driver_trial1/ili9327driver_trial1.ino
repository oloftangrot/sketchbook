#include "ili9327driver.h"
#include "ili9327driver_ext.h"

void setup( void ) {
  for(int p = 2; p < 10; p++ )
  {
    pinMode( p, OUTPUT );
  }
  ili9327::Lcd_Init();
  ili9327::Lcd_Clear( 0x00 );
  ili9327_ext::fillCircle( 100, 100, 50 );
  ili9327_ext::drawRoundRect( 100, 200, 110, 210 );
}

void loop( void ) {
  ili9327::Lcd_Clear( 0x00f );
  for ( int i= 100; i < 200; i++ ) {
    for ( int j= 100; j < 200; j++ ) {
      ili9327::Plot( i, j, 255 );}}
}


