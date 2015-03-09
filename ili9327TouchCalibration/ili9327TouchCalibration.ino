#include "calibrate.h"
#include "TouchScreen.h"
//   ic:ili9327  
#define LCD_RD   A0
#define LCD_WR   A1     
#define LCD_RS   A2        
#define LCD_CS   A3       
#define LCD_REST A4

// These are the pins for the touch shield!
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

#define HC245 2 // The enable pin for the resitive/ili9327 transceiver
#define MINPRESSURE 10
#define MAXPRESSURE 10000

// calibration points for 240x400 screen
const POINT cal_pa[3] = { // See Atmel application note AVR341
  { 120, 360 },   // P1 : 50% max height, max width - 10 %
  { 216, 200 },   // P2 : Max height - 10 %, 50% max widht
  { 24, 40 }      // P3 : 10%, 10 %
};
POINT res_pa[3]; // Result from screen read back 
MATRIX M;
const char s = 2;

void doCalibration( void )
{
  for ( int i = 0; i < 3; i++ ) { // Draw the points, assuming grafics is enabled.
    Rect( cal_pa[i].x - s, cal_pa[i].y -s, 2 * s, 2 * s, random(65535)); 
  }
  digitalWrite( HC245, LOW ); // Set bus tranceiver to disable the graphic controller.
  TouchScreen ts = TouchScreen( XP, YP, XM, YM, 300 );

  for ( int i = 0; i < 3; i++ ) { // Loop over the calibration points
    res_pa[i].x = 0;
    res_pa[i].y = 0;
    Serial.print( "Press P" ); 
    Serial.print ( i + 1 ); 
    Serial.println( "..." );
    for ( int j = 0; j < 10; ) {
      TSPoint p = ts.getPoint();
      if ( p.z > MINPRESSURE && p.z < MAXPRESSURE ) {
        j++; // Increase the accepted point counter.
        Serial.print( "X = " ); Serial.print( p.x );
        Serial.print( "\tY = " ); Serial.print( p.y );
        Serial.print( "\tPressure = " ); Serial.println( p.z );
        res_pa[i].x += p.x;
        res_pa[i].y += p.y;
      }
    }
    res_pa[i].x = res_pa[i].x / 10;
    res_pa[i].y = res_pa[i].y / 10;
  }
  setCalibrationMatrix( cal_pa, res_pa, &M );
}

void setup()
{
  for(int p = 3; p < 10; p++ ) 
  {
    pinMode( p, OUTPUT );
  }
  pinMode( HC245, OUTPUT );
  digitalWrite( HC245, HIGH ); // Set bus tranceiver to enable the graphic controller.

  pinMode( A0, OUTPUT );
  pinMode( A1, OUTPUT );
  pinMode( A2, OUTPUT );
  pinMode( A3, OUTPUT );
  pinMode( A4, OUTPUT );
  digitalWrite( A0, HIGH );
  digitalWrite( A1, HIGH );
  digitalWrite( A2, HIGH );
  digitalWrite( A3, HIGH );
  digitalWrite( A4, HIGH );
  Lcd_Init();
  LCD_Clear(0x00);
  Serial.begin(9600);
  Serial.println( "Touch screen calibration V0.1 started." );
}

void loop() {
}

void Lcd_Writ_Bus( char VH )
{
  unsigned char i,temp,data; 
  data = VH;
  for ( i = 8; i <= 9; i++ )
  {
    temp = ( data & 0x01 );
    if(temp)
      digitalWrite( i, HIGH );
    else
      digitalWrite( i, LOW );
    data = data >> 1;
  }	
  for ( i = 2; i <= 7; i++ )
  {
    temp = ( data & 0x01 );
    if( temp )
      digitalWrite( i, HIGH );
    else
      digitalWrite( i, LOW );
    data = data >> 1;
  }	 

  digitalWrite( LCD_WR, LOW );
  digitalWrite( LCD_WR, HIGH );
}

void Lcd_Write_Com( char VH )  
{   
  digitalWrite( LCD_RS, LOW );
  Lcd_Writ_Bus( VH );
}

void Lcd_Write_Data( char VH )
{
  digitalWrite( LCD_RS, HIGH );
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
  Lcd_Write_Data( y1 >>8 );
  Lcd_Write_Data( y1 );
  Lcd_Write_Data( y2 >> 8 );
  Lcd_Write_Data( y2 );

  Lcd_Write_Com( 0x2c ); // Write_memory_start								 
}

void Lcd_Init( void )
{
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
  digitalWrite( LCD_RS,HIGH );
  digitalWrite( LCD_CS,LOW );
  l = l + x;
  Address_set(x, y, l, y );
  j = l * 2;
  for ( i = 1; i <= j; i++ )
  {
    Lcd_Write_Data(c);
  }
  digitalWrite(LCD_CS,HIGH);   
}

void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c)                   
{	
  unsigned int i,j;
  Lcd_Write_Com( 0x02c ); //write_memory_start
  digitalWrite( LCD_RS, HIGH );
  digitalWrite( LCD_CS, LOW );
  l=l+y;
  Address_set( x, y, x, l );
  j = l * 2;
  for ( i = 1; i <= j; i++ )
  {
    Lcd_Write_Data(c);
  }
  digitalWrite(LCD_CS,HIGH);   
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

void LCD_Clear( unsigned int j )                   
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

