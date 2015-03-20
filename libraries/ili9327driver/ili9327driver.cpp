#include "Arduino.h"
#include "ili9327driver.h"
//
//   This is a driver for the TFT 3.5 Inch 400x240 touch shield from http://www.mcufriends.com
//   Since the shield is fixed the wiring defines is given here and
//   there is no interface to define them using an API call. The graphic controller is controlled 
//   in serial 8-bit mode.
//
namespace ili9327 {

void Lcd_Writ_Bus( unsigned char data )
{
  PORTD &= 0x03;
  PORTD |= ( data & 0xfc );
  PORTB &= 0xfc;
  PORTB |= ( data & 0x03 );
  cbi( PORTC, (1 << 1) ); //  digitalWrite( LCD_WR, LOW );
  sbi( PORTC, (1 << 1) ); //  digitalWrite( LCD_WR, HIGH );
}

void Lcd_Write_Com( char VH )  
{   
  cbi( PORTC, ( 1<<2 ) ); //  digitalWrite( LCD_RS, LOW );
  Lcd_Writ_Bus( VH );
}

void Lcd_Write_Data( char VH )
{
  sbi( PORTC, ( 1 << 2 ) ); // digitalWrite( LCD_RS, HIGH );
  Lcd_Writ_Bus( VH );
}

void Lcd_Write_Com_Data( int com, int dat )
{
  Lcd_Write_Com( com );
  Lcd_Write_Data( dat );
}

void Address_set( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2 )
{
  Lcd_Write_Com( 0x2a ); // Set_column_address 4 parameters

  Lcd_Write_Data( x1 >> 8 );
  Lcd_Write_Data( x1 );
  Lcd_Write_Data( x2 >> 8 );
  Lcd_Write_Data( x2 );

  Lcd_Write_Com( 0x2b ); // Set_page_address 4 parameters
  Lcd_Write_Data( y1 >> 8 );
  Lcd_Write_Data( y1 );
  Lcd_Write_Data( y2 >> 8 );
  Lcd_Write_Data( y2 );

  Lcd_Write_Com( 0x2c ); // Write_memory_start								 
}

void Lcd_Init( void )
{
  pinMode( LCD_RD, OUTPUT );
  pinMode( LCD_WR, OUTPUT );
  pinMode( LCD_RS, OUTPUT );
  pinMode( LCD_CS, OUTPUT );
  pinMode( LCD_REST, OUTPUT );
  digitalWrite( LCD_RD, HIGH );
  digitalWrite( LCD_WR, HIGH );
  digitalWrite( LCD_RS, HIGH );
  digitalWrite( LCD_CS, HIGH );
  digitalWrite( LCD_REST, HIGH );

  digitalWrite(LCD_REST,HIGH);
  delay(5); 
  digitalWrite(LCD_REST,LOW);
  delay(15);
  digitalWrite(LCD_REST,HIGH);
  delay(15);

  digitalWrite(LCD_CS,HIGH);
  digitalWrite(LCD_WR,HIGH);
  digitalWrite(LCD_CS,LOW);  //CS

  Lcd_Write_Com(0xE9);
  Lcd_Write_Data(0x20);

  Lcd_Write_Com(0x11); //Exit Sleep
  delay(100);

  Lcd_Write_Com(0xD1);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x71);
  Lcd_Write_Data(0x19);

  Lcd_Write_Com(0xD0);
  Lcd_Write_Data(0x07);
  Lcd_Write_Data(0x01);
  Lcd_Write_Data(0x08);

  Lcd_Write_Com(0x36);
  Lcd_Write_Data(0x48);

  Lcd_Write_Com(0x3A);
  Lcd_Write_Data(0x05);

  Lcd_Write_Com(0xC1);
  Lcd_Write_Data(0x10);
  Lcd_Write_Data(0x10);
  Lcd_Write_Data(0x02);
  Lcd_Write_Data(0x02);

  Lcd_Write_Com(0xC0); //Set Default Gamma
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x35);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x01);
  Lcd_Write_Data(0x02);

  Lcd_Write_Com(0xC5); //Set frame rate
  Lcd_Write_Data(0x04);

  Lcd_Write_Com(0xD2); //power setting
  Lcd_Write_Data(0x01);
  Lcd_Write_Data(0x44);

  Lcd_Write_Com(0xC8); //Set Gamma
  Lcd_Write_Data(0x04);
  Lcd_Write_Data(0x67);
  Lcd_Write_Data(0x35);
  Lcd_Write_Data(0x04);
  Lcd_Write_Data(0x08);
  Lcd_Write_Data(0x06);
  Lcd_Write_Data(0x24);
  Lcd_Write_Data(0x01);
  Lcd_Write_Data(0x37);
  Lcd_Write_Data(0x40);
  Lcd_Write_Data(0x03);
  Lcd_Write_Data(0x10);
  Lcd_Write_Data(0x08);
  Lcd_Write_Data(0x80);
  Lcd_Write_Data(0x00);

  Lcd_Write_Com(0x2A); 
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0xeF);

  Lcd_Write_Com(0x2B); 
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x01);
  Lcd_Write_Data(0x3F);
  Lcd_Write_Data(0x8F); // on internet

  Lcd_Write_Com(0x29); //display on      

  Lcd_Write_Com(0x2C); //display on 

  digitalWrite( LCD_CS, HIGH );
}

void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c)
{	
  unsigned int i, j;
  Lcd_Write_Com( 0x02c ); //write_memory_start
  sbi( PORTC, ( 1 << 2 ) ); // digitalWrite( LCD_RS,HIGH );
  cbi( PORTC, ( 1 << 3 ) ); // digitalWrite( LCD_CS,LOW );
  l = l + x;
  Address_set(x, y, l, y );
  j = l * 2;
  for ( i = 1; i <= j; i++ )
  {
    Lcd_Write_Data(c);
  }
  sbi( PORTC, ( 1 << 3 ) ); // digitalWrite(LCD_CS,HIGH);   
}

void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) 
{	
  unsigned int i,j;
  Lcd_Write_Com( 0x02c ); //write_memory_start
  sbi( PORTC, ( 1 << 2 ) ); // digitalWrite( LCD_RS,HIGH );
  cbi( PORTC, ( 1 << 3 ) ); // digitalWrite( LCD_CS,LOW );
  l=l+y;
  Address_set( x, y, x, l );
  j = l * 2;
  for ( i = 1; i <= j; i++ )
  {
    Lcd_Write_Data(c);
  }
  sbi( PORTC, ( 1 << 3 ) ); // digitalWrite(LCD_CS,HIGH);   
}

void Rect(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)
{
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}

void Rectf(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)
{
  unsigned int i;
  for ( i = 0; i < h; i++ )
  {
    H_line( x, y  , w, c);
    H_line( x, y+i, w, c);
  }
}

void Lcd_Clear( unsigned int j )                   
{	
  unsigned int i, m;

  Lcd_Write_Com( 0x02c ); //write_memory_start
  digitalWrite( LCD_RS, HIGH );
  digitalWrite( LCD_CS, LOW );
  Address_set( 0, 0, 479, 399 );

  for ( i = 0; i < 480; i++ )
    for ( m = 0; m < 400; m++ )
    {
      Lcd_Write_Data( j );
    }
  digitalWrite(LCD_CS,HIGH);   
}
  void Plot( unsigned int x, unsigned int y, unsigned int c )
	{
		Lcd_Write_Com( 0x02c ); //write_memory_start
		digitalWrite( LCD_RS, HIGH );
		digitalWrite( LCD_CS, LOW );
		Address_set( x, y, x, y + 1 );
		Lcd_Write_Data(c);
		Lcd_Write_Data(c);
		digitalWrite(LCD_CS,HIGH);  
	}
}
