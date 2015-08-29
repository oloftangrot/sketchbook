
/*  	

 1pcs 3.95 inch LCD Display Module TFT LCD screen for Arduino UNO R3 Board,ili9488 driver IC,480x320 resolution(NO UNO R3 development board)	

3.95 inch LCD touch screen 	
Resolution: 480 x320 	
Controller: ili9488 	
 Provide the LCD, IC specification ili9488 microcontroller arm driver 	
Appearance of size: 	
9.7 cm X 6.9 cm 	

Test code:	

ILI9488:
*/
#define LCD_RD   A0
#define LCD_WR   A1     
#define LCD_RS   A2        
#define LCD_CS   A3       
#define LCD_REST A4

#define LCD_RD_BitMask   0x01
#define LCD_WR_BitMask   0x02
#define LCD_RS_BitMask   0x04
#define LCD_CS_BitMask   0x08
#define LCD_REST_BitMask   0x10

#include <avr/pgmspace.h>

#define P_CS PORTC 
#define B_CS (1<<3)
#define cbi(reg, bitmask) reg &= ~bitmask
#define sbi(reg, bitmask) reg |= bitmask

void Lcd_Clear( unsigned int j ) ;          

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
  cbi( PORTC, LCD_RS_BitMask ); //  digitalWrite( LCD_RS, LOW );
  Lcd_Writ_Bus( VH );
}

void Lcd_Write_Data( char VH )
{
  sbi( PORTC, LCD_RS_BitMask ); // digitalWrite( LCD_RS, HIGH );
  Lcd_Writ_Bus( VH );
}


void Lcd_Init(void)   //ili9488	
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

  digitalWrite( LCD_REST, HIGH );
  delay(5); 
  digitalWrite( LCD_REST, LOW );
  delay(15);
  digitalWrite( LCD_REST, HIGH );
  delay(15);

  digitalWrite( LCD_CS, HIGH );
  digitalWrite( LCD_WR, HIGH );
  digitalWrite( LCD_CS, LOW );  //CS

  Lcd_Write_Com(0xE0);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x07);
  Lcd_Write_Data(0x10);
  Lcd_Write_Data(0x09);
  Lcd_Write_Data(0x17);
  Lcd_Write_Data(0x0B);
  Lcd_Write_Data(0x40);
  Lcd_Write_Data(0x8A);
  Lcd_Write_Data(0x4B);
  Lcd_Write_Data(0x0A);
  Lcd_Write_Data(0x0D);
  Lcd_Write_Data(0x0F);
  Lcd_Write_Data(0x15);
  Lcd_Write_Data(0x16);
  Lcd_Write_Data(0x0F);
  Lcd_Write_Com(0xE1);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x1A);
  Lcd_Write_Data(0x1B);
  Lcd_Write_Data(0x02);
  Lcd_Write_Data(0x0D);
  Lcd_Write_Data(0x05);
  Lcd_Write_Data(0x30);
  Lcd_Write_Data(0x35);
  Lcd_Write_Data(0x43);
  Lcd_Write_Data(0x02);
  Lcd_Write_Data(0x0A);
  Lcd_Write_Data(0x09);
  Lcd_Write_Data(0x32);
  Lcd_Write_Data(0x36);
  Lcd_Write_Data(0x0F);
  Lcd_Write_Com(0xB1);
  Lcd_Write_Data(0xA0);
  Lcd_Write_Com(0xB4);
  Lcd_Write_Data(0x02);
  Lcd_Write_Com(0xC0);
  Lcd_Write_Data(0x17);
  Lcd_Write_Data(0x15);
  Lcd_Write_Com(0xC1);
  Lcd_Write_Data(0x41);
  Lcd_Write_Com(0xC5);
  Lcd_Write_Data(0x00);
  Lcd_Write_Data(0x0A);
  Lcd_Write_Data(0x80);
  Lcd_Write_Com(0xB6);
  Lcd_Write_Data(0x02);
  Lcd_Write_Com(0x36);
  Lcd_Write_Data(0x48);  
  Lcd_Write_Com(0x3a);   
  Lcd_Write_Data(0x55);
  Lcd_Write_Com(0xE9);
  Lcd_Write_Data(0x00); 
  Lcd_Write_Com(0XF7);
  Lcd_Write_Data(0xA9);
  Lcd_Write_Data(0x51);
  Lcd_Write_Data(0x2C);
  Lcd_Write_Data(0x82);
  Lcd_Write_Com(0x11);
  delay(120);
  Lcd_Write_Com(0x29);

  digitalWrite( LCD_CS, HIGH );

}
 
void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{
  Lcd_Write_Com(0x2a); // Set_column_address 4 parameters
  Lcd_Write_Data(x1>>8);
  Lcd_Write_Data(x1);
  Lcd_Write_Data(x2>>8);
  Lcd_Write_Data(x2);
 
  Lcd_Write_Com(0x2b); // Set_page_address 4 parameters
  Lcd_Write_Data(y1>>8);
  Lcd_Write_Data(y1);
  Lcd_Write_Data(y2>>8);
  Lcd_Write_Data(y2);
 
  Lcd_Write_Com(0x2c); //       Write_memory_start        
}

void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c)
{	
  unsigned int i, j;
  Lcd_Write_Com( 0x02c ); //write_memory_start
  sbi( PORTC, LCD_RS_BitMask ); // digitalWrite( LCD_RS,HIGH );
  cbi( PORTC, LCD_CS_BitMask ); // digitalWrite( LCD_CS,LOW );
  l = l + x;
  Address_set(x, y, l, y );
  j = l * 2;
  for ( i = 1; i <= j; i++ )
  {
    Lcd_Write_Data( c >> 8);
    Lcd_Write_Data( c );
  }
  sbi( PORTC, LCD_CS_BitMask ); // digitalWrite(LCD_CS,HIGH);   
}

void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) 
{	
  unsigned int i,j;
  Lcd_Write_Com( 0x02c ); //write_memory_start
  sbi( PORTC, LCD_RS_BitMask ); // digitalWrite( LCD_RS,HIGH );
  cbi( PORTC, LCD_CS_BitMask ); // digitalWrite( LCD_CS,LOW );
  l = l + y;
  Address_set( x, y, x, l );
  j = l * 2;
  for ( i = 1; i <= j; i++ )
  {
    Lcd_Write_Data( c >> 8 );
    Lcd_Write_Data( c );
  }
  sbi( PORTC, LCD_CS_BitMask ); // digitalWrite(LCD_CS,HIGH);   
}

void Rect(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)
{
  H_line(x  , y  , w, c );
  H_line(x  , y+h, w, c );
  V_line(x  , y  , h, c );
  V_line(x+w, y  , h, c );
}

void Rectf(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c)
{
  unsigned int i;
  for ( i = 0; i < h; i++ )
  {
    H_line( x, y  , w, c );
    H_line( x, y+i, w, c );
  }
}

void Lcd_Clear( unsigned int j )                   
{	
  unsigned int i, m;

  Lcd_Write_Com( 0x02c ); //write_memory_start
  sbi( PORTC, LCD_RS_BitMask ); // digitalWrite( LCD_RS, HIGH );
  cbi( PORTC, LCD_CS_BitMask ); // digitalWrite( LCD_CS, LOW );
  Address_set( 0, 0, 479, 399 );

  for ( i = 0; i < 400; i++ )
    for ( m = 0; m < 240; m++ )
    {
      Lcd_Write_Data( j >> 8 );
	    Lcd_Write_Data( j );
    }
  sbi( PORTC, LCD_CS_BitMask );	//	digitalWrite(LCD_CS,HIGH);  
}

void setup()
{
  Lcd_Init();
}

void loop()
{  

  for ( int i = 0; i < 200; i++ )
  { // rectangle at x, y, with, hight, color
    Rect( random(120), random(200), random(120), random(200), random(65535)); 
  }
  Lcd_Clear(0x00);
/*
  for ( int color=1; color<65535; color++) {
    for( int x = 290; x< 300; x++ ) {
      H_line( 200, x, 50, color );
    }
  }
  */
}

