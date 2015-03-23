#ifndef __ILI9327_H__
#define __ILI9327_H__

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
#if 0
typedef union twobytes_t {  
	unsigned int  Word;
  struct  {
		unsigned char  HiByte;
		unsigned char  LoByte;
	} bytes;
};
#endif
namespace ili9327 {
	void Lcd_Writ_Bus( unsigned char VH ) ;
	void Lcd_Write_Com( char VH ) ;
	void Lcd_Write_Data( char VH ) ;
	void Lcd_Write_Com_Data( int com, int dat );
	void Address_set( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2 );
	void Lcd_Init( void );
	void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
	void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
	void Rect(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c);
	void Rectf(unsigned int x,unsigned int y,unsigned int w,unsigned int h,unsigned int c);
	void Lcd_Clear( unsigned int j );
  void Plot( unsigned int x, unsigned int y, unsigned int c );
}
#endif











