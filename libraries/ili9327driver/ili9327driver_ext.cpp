#include "Arduino.h"
#include "ili9327driver.h"
using namespace ili9327;
//
//   This is a driver for the TFT 3.5 Inch 400x240 touch shield from http://www.mcufriends.com
//   Since the shield is fixed the wiring defines is given here and
//   there is no interface to define them using an API call. The graphic controller is controlled 
//   in serial 'SPI' mode.
//
namespace ili9327_ext {
unsigned int col = 255;

boolean	transparent = true;

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
#if 0
void printChar(byte c, int x, int y)
{
	byte i,ch;
	word j;
	word temp; 

	cbi(P_CS, B_CS);
  
	if (!transparent)
	{
		if (orient==PORTRAIT)
		{
			setXY(x,y,x+cfont.x_size-1,y+cfont.y_size-1);
	  
			temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
			for(j=0;j<((cfont.x_size/8)*cfont.y_size);j++)
			{
				ch=pgm_read_byte(&cfont.font[temp]);
				for(i=0;i<8;i++)
				{   
					if((ch&(1<<(7-i)))!=0)   
					{
						setPixel((fch<<8)|fcl);
					} 
					else
					{
						setPixel((bch<<8)|bcl);
					}   
				}
				temp++;
			}
		}
		else
		{
			temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;

			for(j=0;j<((cfont.x_size/8)*cfont.y_size);j+=(cfont.x_size/8))
			{
				setXY(x,y+(j/(cfont.x_size/8)),x+cfont.x_size-1,y+(j/(cfont.x_size/8)));
				for (int zz=(cfont.x_size/8)-1; zz>=0; zz--)
				{
					ch=pgm_read_byte(&cfont.font[temp+zz]);
					for(i=0;i<8;i++)
					{   
						if((ch&(1<<i))!=0)   
						{
							setPixel((fch<<8)|fcl);
						} 
						else
						{
							setPixel((bch<<8)|bcl);
						}   
					}
				}
				temp+=(cfont.x_size/8);
			}
		}
	}
	else
	{
		temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
		for(j=0;j<cfont.y_size;j++) 
		{
			for (int zz=0; zz<(cfont.x_size/8); zz++)
			{
				ch=pgm_read_byte(&cfont.font[temp+zz]); 
				for(i=0;i<8;i++)
				{   
					setXY(x+i+(zz*8),y+j,x+i+(zz*8)+1,y+j+1);
				
					if((ch&(1<<(7-i)))!=0)   
					{
						setPixel((fch<<8)|fcl);
					} 
				}
			}
			temp+=(cfont.x_size/8);
		}
	}

	sbi(P_CS, B_CS);
	clrXY();
}
#endif

}
