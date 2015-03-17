#ifndef __ILI9327_H__
#define __ILI9327_H__
namespace ili9327 {
void Lcd_Writ_Bus( char VH ) ;
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
}
#endif











