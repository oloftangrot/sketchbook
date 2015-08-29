#include "MCUFRIEND_kbv.h"
#include "mcufriend_shield.h"

#define wait_ms(ms)  delay(ms)
#define AUTO_READINC    (1<<0)
#define TWO_PARM_WINDOW (1<<1)
#define MV_AXIS         (1<<2)

MCUFRIEND_kbv::MCUFRIEND_kbv() : Adafruit_GFX(240, 320)
{
  // we can not access GPIO pins until AHB has been enabled.
}

static uint8_t done_reset;

void MCUFRIEND_kbv::reset(void)
{
	done_reset = 1;
	setWriteDir();
	CTL_INIT();
	CS_IDLE;
	RD_IDLE;
	WR_IDLE;
	RESET_IDLE;
	wait_ms( 50 );
	RESET_ACTIVE;
	wait_ms( 100 );
	RESET_IDLE;
	wait_ms( 100 );
}

void MCUFRIEND_kbv::WriteCmdData(uint16_t cmd, uint16_t dat)
{
	CS_ACTIVE;
	WriteCmd(cmd);
	WriteData(dat);
	CS_IDLE;
}

static void WriteCmdData2(uint16_t cmd, uint16_t dat1, uint16_t dat2)
{
	CS_ACTIVE;
	WriteCmd(cmd);
	WriteData(dat1);
	WriteData(dat2);
	CS_IDLE;
}

uint16_t MCUFRIEND_kbv::readReg(uint16_t reg)
{
	uint16_t ret;
	if (!done_reset) reset();
	CS_ACTIVE;
	WriteCmd(reg);
	setReadDir();
	CD_DATA;
	READ_16(ret);
	RD_IDLE;
	CS_IDLE;
	setWriteDir();
	return ret;
}

uint32_t MCUFRIEND_kbv::readReg32(uint16_t reg)
{
	uint16_t h, l;
	CS_ACTIVE;
	WriteCmd(reg);
	setReadDir();
	CD_DATA;
	READ_16(h);
	READ_16(l);
	RD_IDLE;
	CS_IDLE;
	setWriteDir();
	return (h << 16L) | (l);
}

uint16_t MCUFRIEND_kbv::readID(void)
{
	uint16_t ret;
	ret = readReg(0);
	if (ret == 0) {
	    ret = readReg32(0xD3);     //0x9488 for ILI9488
	    if (ret == 0)
	        ret = readReg32(0xEF); //0x0493 for ILI9327
	}
	return ret;
}

// independent cursor and window registers.   S6D0154, ST7781 increments.  ILI92320/5 do not.  
int16_t MCUFRIEND_kbv::readGRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h)
{
	uint16_t ret, dummy, _MR = _MW, i = 0;
	int16_t n = w * h;
//    setAddrWindow(0, 0, width() - 1, height() - 1);

	while (n > 0) {
        if (_lcd_capable & TWO_PARM_WINDOW) {
		    setAddrWindow(x, y, x + w - 1, y + h - 1);
			_MR = 0x2E;
        } else {
	        WriteCmdData( _MC, x + i);
	        WriteCmdData( _MP, y );
		}
		CS_ACTIVE;
		WriteCmd(_MR);
		setReadDir();
		CD_DATA;
        switch (_lcd_ID) {
		case 0x9488: 
        	uint8_t r, g, b;
		    READ_8(r);
		    while (n-- > 0) {
	            READ_8(r);
		        READ_8(g);
		        READ_8(b);
		        *block++ = color565(r, g, b);
			}
			break;
		case 0x0154: 
		case 0x7783: 
		    READ_16(dummy);
		    while (n-- > 0) {
			    READ_16(ret);
			    *block++ = ret;
			}
			break;
		default:
		    READ_16(dummy);
			READ_16(ret);
			*block++ = ret;
			n--;
			if (++i >= w) {
			    i = 0;
			    y++;
			}
			break;
		}
		RD_IDLE;
		CS_IDLE;
		setWriteDir();
	}
	if (!(_lcd_capable & TWO_PARM_WINDOW)) setAddrWindow(0, 0, width() - 1, height() - 1);
	return 0;
}

void MCUFRIEND_kbv::setRotation(uint8_t r)
{
	uint16_t drivOut = 0;
	uint16_t entryMod = 0x1030;    // BGR, I/D1, ID/0
	uint16_t gateScan = 0x2700;
	Adafruit_GFX::setRotation(r & 3);
	switch (rotation) {
		case 0:                    //PORTRAIT:
		default:
		drivOut = 0x0100;          // SS   (MX)
		entryMod |= 0x0000;
		gateScan |= 0x8000;        // GS   (!MY)
		break;

		case 1:                    //LANDSCAPE:
		drivOut = 0x0100;          // SS   (MX)
		entryMod |= 0x0008;        // ORG  (MV)
		gateScan |= 0x0000; 
		break;

		case 2:                    //PORTRAIT_REV:
		drivOut = 0x0000;
		entryMod |= 0x0000;
		gateScan |= 0x0000;
		break;

		case 3:                    //LANDSCAPE_REV:
		drivOut = 0x0000;
		entryMod |= 0x0008;        // ORG (MV)
		gateScan |= 0x8000;        // GS  (!MY)
		break;
 	}
	switch (_lcd_ID) {
	    case 0x9320:
		    gateScan ^= 0x8000;   //.kbv ILI9320 has inverted logic on GS
		case 0x9325:
		case 0x7783:
		    _MC = 0x20, _MP = 0x21, _MW = 0x22, _SC = 0x50, _EC = 0x51, _SP = 0x52, _EP = 0x53;
		    WriteCmdData( 0x01, drivOut ); // set Driver Output Control
	        WriteCmdData( 0x60, gateScan ); // Gate Scan Line (0xA700)
	        WriteCmdData( 0x03, entryMod ); // set GRAM write direction and BGR=1.
		    break;
		case 0x0154:
		    _MC = 0x20, _MP = 0x21, _MW = 0x22, _SC = 0x37, _EC = 0x36, _SP = 0x39, _EP = 0x38;
		    gateScan = (gateScan & 0x8000) ? 0x0028 : 0x0228;
		    WriteCmdData( 0x01, drivOut | gateScan); // set Driver Output Control
	        WriteCmdData( 0x03, entryMod ); // set GRAM write direction and BGR=1.
		    break;
		case 0xB509:
		    _MC = 0x200, _MP = 0x201, _MW = 0x202, _SC = 0x210, _EC = 0x211, _SP = 0x212, _EP = 0x213;
		    WriteCmdData( 0x400, (gateScan & 0x8000) ? 0x6200 : 0xE200);
		    WriteCmdData( 0x01, drivOut); // set Driver Output Control
	        WriteCmdData( 0x03, entryMod ); // set GRAM write direction and BGR=1.
		    break;
		case 0x9488:
		    _MC = 0x2A, _MP = 0x2B, _MW = 0x2C, _SC = 0x2A, _EC = 0x2A, _SP = 0x2B, _EP = 0x2B;
		    WriteCmdData( 0x36, ((gateScan & 0x8000) ? 0 : 0x8000)     //MY
                    		  | ((drivOut & 0x0100) ? 0x4000 : 0)      //MX
                    		  | ((entryMod & 0x0008) ? 0x2000 : 0)     //MV
							  | (0x0800));                             //BGR
			break;
	}
	if ((rotation & 1) && ((_lcd_capable & MV_AXIS) == 0)) {
	    uint16_t x;
		x = _MC, _MC = _MP, _MP = x;
		x = _SC, _SC = _SP, _SP = x;
		x = _EC, _EC = _EP, _EP = x;
	}
}

void MCUFRIEND_kbv::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	// MCUFRIEND just plots at edge if you try to write outside of the box:
	if (x < 0 || y < 0 || x >= width() || y >= height()) return;
    if (_lcd_capable & TWO_PARM_WINDOW) {
	    WriteCmdData2( _MC, x, x );
	    WriteCmdData2( _MP, y, y );
	    WriteCmdData( _MW, color );
	} else {
	    WriteCmdData( _MC, x );
	    WriteCmdData( _MP, y );
	    WriteCmdData( _MW, color );
	}
}

void MCUFRIEND_kbv::setAddrWindow(int16_t x, int16_t y, int16_t x1, int16_t y1)
{
    if (_lcd_capable & TWO_PARM_WINDOW) {
	    WriteCmdData2( _MC, x, x1 );
	    WriteCmdData2( _MP, y, y1 );
	} else {
	    WriteCmdData( _MC, x );
	    WriteCmdData( _MP, y );
	    WriteCmdData( _SC, x );
	    WriteCmdData( _SP, y );
	    WriteCmdData( _EC, x1 );
	    WriteCmdData( _EP, y1 );
    }
}

void MCUFRIEND_kbv::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	int16_t end;
	if (w < 0) { w = -w; x -= w; }   //+ve w
	end = x + w;
	if (x < 0) x = 0;
	if (end > width()) end = width();
	w = end - x;
	if (h < 0) { h = -h; y -= h; }   //+ve h
	end = y + h;
	if (y < 0) y = 0;
	if (end > height()) end = height();
	h = end - y;
	setAddrWindow(x, y, x+w-1, y+h-1);
	CS_ACTIVE;
	WriteCmd( _MW);
	while (w-- > 0) {
		for (int16_t i = h; i-- > 0; )
		WriteData(color);
	}
	CS_IDLE;
	if (!(_lcd_capable & TWO_PARM_WINDOW)) setAddrWindow(0, 0, width() - 1, height() - 1);
}

void MCUFRIEND_kbv::write_data_block(uint16_t *block, int16_t n)
{
	uint16_t color;
	CS_ACTIVE;
	CD_DATA;
	while (n-- > 0) {
		color = *block++;
		write16(color);
	}
	CS_IDLE;
}

void MCUFRIEND_kbv::pushColors(uint16_t *block, int16_t n, uint8_t first) 
{
    if (first) {
	    CS_ACTIVE;
		WriteCmd(_MW);
	}
	write_data_block(block, n);
}

void MCUFRIEND_kbv::vertScroll(int16_t top, int16_t scrollines, int16_t offset)
{
	switch (_lcd_ID) {
    case 0x7783:
	    WriteCmdData( 0x61, 1);       //!NDL, !VLE, REV
	    WriteCmdData( 0x6A, offset);  //VL#
        break;		
	case 0x0154:
	    WriteCmdData( 0x31, scrollines-1);  //SEA
    	WriteCmdData( 0x32, top);       //SSA
    	WriteCmdData( 0x33, offset);       //SST
		break;
	case 0xB509:
	    WriteCmdData( 0x401, 3);       //VLE, REV 
        WriteCmdData( 0x404, offset);  //VL# 
        break;
	case 0x9488:
        WriteCmdData( 0x0033, top);        //TOP
        WriteData(scrollines);
        WriteData(320 - top - scrollines);
        WriteCmdData( 0x0037, offset);  //VL# 
        break;
	default:
	    WriteCmdData( 0x61, 3);       //!NDL, VLE, REV
	    WriteCmdData( 0x6A, offset);  //VL#
		break;
	}
}

void MCUFRIEND_kbv::begin(uint16_t ID)
{
	int16_t *p16;
	reset();
	switch (_lcd_ID = ID) {
		case 0x0154:
		_lcd_capable = AUTO_READINC;
		WriteCmdData(0x11,0x001A);
		WriteCmdData(0x12,0x3121);
		WriteCmdData(0x13,0x006C);
		WriteCmdData(0x14,0x4249);

		WriteCmdData(0x10,0x0800);
		wait_ms(10);
		WriteCmdData(0x11,0x011A);
		wait_ms(10);
		WriteCmdData(0x11,0x031A);
		wait_ms(10);
		WriteCmdData(0x11,0x071A);
		wait_ms(10);
		WriteCmdData(0x11,0x0F1A);
		wait_ms(10);
		WriteCmdData(0x11,0x0F3A);
		wait_ms(30);

		WriteCmdData(0x01,0x0128);
		WriteCmdData(0x02,0x0100);
		WriteCmdData(0x03,0x1030);
		WriteCmdData(0x07,0x1012);
		WriteCmdData(0x08,0x0303);
		WriteCmdData(0x0B,0x1100);
		WriteCmdData(0x0C,0x0000);
		WriteCmdData(0x0F,0x1801);
		WriteCmdData(0x15,0x0020);
		/*
		WriteCmdData(0x50,0x0101);
		WriteCmdData(0x51,0x0603);
		WriteCmdData(0x52,0x0408);
		WriteCmdData(0x53,0x0000);
		WriteCmdData(0x54,0x0605);
		WriteCmdData(0x55,0x0406);
		WriteCmdData(0x56,0x0303);
		WriteCmdData(0x57,0x0303);
		WriteCmdData(0x58,0x0010);
		WriteCmdData(0x59,0x1000);
		*/
		WriteCmdData(0x07,0x0012);
		wait_ms(40);
		WriteCmdData(0x07,0x0013);/* GRAM Address Set */
		WriteCmdData(0x07,0x0017);/* Display Control DISPLAY ON */
		
		WriteCmdData(0x36,0x00EF);
		WriteCmdData(0x37,0x0000);
		WriteCmdData(0x38,0x013F);
		WriteCmdData(0x39,0x0000);

		break;
    
		case 0x7783:
		_lcd_capable = AUTO_READINC;
		WriteCmdData(0x00FF, 0x0001);
		WriteCmdData(0x00F3, 0x0008);
		//  LCD_Write_COM(0x00F3);

		WriteCmdData(0x00, 0x0001);
		WriteCmdData(0x0001, 0x0100);     // Driver Output Control Register (R01h)
		WriteCmdData(0x0002, 0x0700);     // LCD Driving Waveform Control (R02h)
		WriteCmdData(0x0003, 0x1030);     // Entry Mode (R03h)
		WriteCmdData(0x0008, 0x0302);
		WriteCmdData(0x0009, 0x0000);
		WriteCmdData(0x0010, 0x0000);     // Power Control 1 (R10h)
		WriteCmdData(0x0011, 0x0007);     // Power Control 2 (R11h)
		WriteCmdData(0x0012, 0x0000);     // Power Control 3 (R12h)
		WriteCmdData(0x0013, 0x0000);     // Power Control 4 (R13h)
		wait_ms(50);
		WriteCmdData(0x0010, 0x14B0);     // Power Control 1 (R10h)
		wait_ms(10);
		WriteCmdData(0x0011, 0x0007);     // Power Control 2 (R11h)
		wait_ms(10);
		WriteCmdData(0x0012, 0x008E);     // Power Control 3 (R12h)
		WriteCmdData(0x0013, 0x0C00);     // Power Control 4 (R13h)
		WriteCmdData(0x0029, 0x0015);     // NVM read data 2 (R29h)
		wait_ms(10);
		WriteCmdData(0x0030, 0x0000);     // Gamma Control 1
		WriteCmdData(0x0031, 0x0107);     // Gamma Control 2
		WriteCmdData(0x0032, 0x0000);     // Gamma Control 3
		WriteCmdData(0x0035, 0x0203);     // Gamma Control 6
		WriteCmdData(0x0036, 0x0402);     // Gamma Control 7
		WriteCmdData(0x0037, 0x0000);     // Gamma Control 8
		WriteCmdData(0x0038, 0x0207);     // Gamma Control 9
		WriteCmdData(0x0039, 0x0000);     // Gamma Control 10
		WriteCmdData(0x003C, 0x0203);     // Gamma Control 13
		WriteCmdData(0x003D, 0x0403);     // Gamma Control 14
		WriteCmdData(0x0050, 0x0000);     // Window Horizontal RAM Address Start (R50h)
		WriteCmdData(0x0051, 240 - 1);    // Window Horizontal RAM Address End (R51h)
		WriteCmdData(0x0052, 0X0000);     // Window Vertical RAM Address Start (R52h)
		WriteCmdData(0x0053, 320 - 1);    // Window Vertical RAM Address End (R53h)
		WriteCmdData(0x0060, 0xA700);     // Driver Output Control (R60h) .kbv was 0xa700
		WriteCmdData(0x0061, 0x0001);     // Driver Output Control (R61h)
		WriteCmdData(0x0090, 0X0029);     // Panel Interface Control 1 (R90h)

		// Display On
		WriteCmdData(0x0007, 0x0133);     // Display Control (R07h)
		wait_ms(50);
		break;
		case 0x9320:
		_lcd_capable = 0;
		WriteCmdData(0xe5, 0x8000);
		WriteCmdData(0x00, 0x0001);
		WriteCmdData(0x01, 0x100);
		WriteCmdData(0x02, 0x0700);
		WriteCmdData(0x03, 0x1030);
		WriteCmdData(0x04, 0x0000);
		WriteCmdData(0x08, 0x0202);
		WriteCmdData(0x09, 0x0000);
		WriteCmdData(0x0A, 0x0000);
		WriteCmdData(0x0C, 0x0000);
		WriteCmdData(0x0D, 0x0000);
		WriteCmdData(0x0F, 0x0000);
		//-----Power On sequence-----------------------
		WriteCmdData(0x10, 0x0000);
		WriteCmdData(0x11, 0x0007);
		WriteCmdData(0x12, 0x0000);
		WriteCmdData(0x13, 0x0000);
		wait_ms(50);
		WriteCmdData(0x10, 0x17B0);
		WriteCmdData(0x11, 0x0007);
		wait_ms(10);
		WriteCmdData(0x12, 0x013A);
		wait_ms(10);
		WriteCmdData(0x13, 0x1A00);
		WriteCmdData(0x29, 0x000c);
		wait_ms(10);
		//-----Gamma control-----------------------
		WriteCmdData(0x30, 0x0000);
		WriteCmdData(0x31, 0x0505);
		WriteCmdData(0x32, 0x0004);
		WriteCmdData(0x35, 0x0006);
		WriteCmdData(0x36, 0x0707);
		WriteCmdData(0x37, 0x0105);
		WriteCmdData(0x38, 0x0002);
		WriteCmdData(0x39, 0x0707);
		WriteCmdData(0x3C, 0x0704);
		WriteCmdData(0x3D, 0x0807);
		//-----Set RAM area-----------------------
		WriteCmdData(0x50, 0x0000);
		WriteCmdData(0x51, 0x00EF);
		WriteCmdData(0x52, 0x0000);
		WriteCmdData(0x53, 0x013F);
		WriteCmdData(0x60, 0x2700);
		WriteCmdData(0x61, 0x0001);
		WriteCmdData(0x6A, 0x0000);
		WriteCmdData(0x21, 0x0000);
		WriteCmdData(0x20, 0x0000);
		//-----Partial Display Control------------
		WriteCmdData(0x80, 0x0000);
		WriteCmdData(0x81, 0x0000);
		WriteCmdData(0x82, 0x0000);
		WriteCmdData(0x83, 0x0000);
		WriteCmdData(0x84, 0x0000);
		WriteCmdData(0x85, 0x0000);
		//-----Panel Control----------------------
		WriteCmdData(0x90, 0x0010);
		WriteCmdData(0x92, 0x0000);
		WriteCmdData(0x93, 0x0003);
		WriteCmdData(0x95, 0x0110);
		WriteCmdData(0x97, 0x0000);
		WriteCmdData(0x98, 0x0000);
		//-----Display on-----------------------
		WriteCmdData(0x07, 0x0173);
		wait_ms(50);
		break;
		case 0x9325:
		_lcd_capable = 0;
		WriteCmdData( 0xE5, 0x78F0 ); // set SRAM internal timing
		WriteCmdData( 0x01, 0x0100 ); // set Driver Output Control
		WriteCmdData( 0x02, 0x0200 ); // set 1 line inversion
		WriteCmdData( 0x03, 0x1030 ); // set GRAM write direction and BGR=1.
		WriteCmdData( 0x04, 0x0000 ); // Resize register
		WriteCmdData( 0x05, 0x0000 ); // .kbv 16bits Data Format Selection
		WriteCmdData( 0x08, 0x0207 ); // set the back porch and front porch
		WriteCmdData( 0x09, 0x0000 ); // set non-display area refresh cycle ISC[3:0]
		WriteCmdData( 0x0A, 0x0000 ); // FMARK function
		WriteCmdData( 0x0C, 0x0000 ); // RGB interface setting
		WriteCmdData( 0x0D, 0x0000 ); // Frame marker Position
		WriteCmdData( 0x0F, 0x0000 ); // RGB interface polarity
		// ----------- Power On sequence ----------- //
		WriteCmdData( 0x10, 0x0000 ); // SAP, BT[3:0], AP, DSTB, SLP, STB
		WriteCmdData( 0x11, 0x0007 ); // DC1[2:0], DC0[2:0], VC[2:0]
		WriteCmdData( 0x12, 0x0000 ); // VREG1OUT voltage
		WriteCmdData( 0x13, 0x0000 ); // VDV[4:0] for VCOM amplitude
		WriteCmdData( 0x07, 0x0001 );
		wait_ms( 200 ); // Dis-charge capacitor power voltage
		WriteCmdData( 0x10, 0x1690 ); // SAP, BT[3:0], AP, DSTB, SLP, STB
		WriteCmdData( 0x11, 0x0227 ); // Set DC1[2:0], DC0[2:0], VC[2:0]
		wait_ms( 50 ); // wait_ms 50ms
		WriteCmdData( 0x12, 0x000D ); // 0012
		wait_ms( 50 ); // wait_ms 50ms
		WriteCmdData( 0x13, 0x1200 ); // VDV[4:0] for VCOM amplitude
		WriteCmdData( 0x29, 0x000A ); // 04  VCM[5:0] for VCOMH
		WriteCmdData( 0x2B, 0x000D ); // Set Frame Rate
		wait_ms( 50 ); // wait_ms 50ms
		WriteCmdData( 0x20, 0x0000 ); // GRAM horizontal Address
		WriteCmdData( 0x21, 0x0000 ); // GRAM Vertical Address
		// ----------- Adjust the Gamma Curve ----------//

		WriteCmdData( 0x30, 0x0000 );
		WriteCmdData( 0x31, 0x0404 );
		WriteCmdData( 0x32, 0x0003 );
		WriteCmdData( 0x35, 0x0405 );
		WriteCmdData( 0x36, 0x0808 );
		WriteCmdData( 0x37, 0x0407 );
		WriteCmdData( 0x38, 0x0303 );
		WriteCmdData( 0x39, 0x0707 );
		WriteCmdData( 0x3C, 0x0504 );
		WriteCmdData( 0x3D, 0x0808 );

		//------------------ Set GRAM area ---------------//
		WriteCmdData( 0x50, 0x0000 ); // Horizontal GRAM Start Address
		WriteCmdData( 0x51, 0x00EF ); // Horizontal GRAM End Address
		WriteCmdData( 0x52, 0x0000 ); // Vertical GRAM Start Address
		WriteCmdData( 0x53, 0x013F ); // Vertical GRAM Start Address
		WriteCmdData( 0x60, 0xA700 ); // Gate Scan Line (0xA700)
		WriteCmdData( 0x61, 0x0001 ); // NDL,VLE, REV .kbv
		WriteCmdData( 0x6A, 0x0000 ); // set scrolling line
		//-------------- Partial Display Control ---------//
		WriteCmdData( 0x80, 0x0000 );
		WriteCmdData( 0x81, 0x0000 );
		WriteCmdData( 0x82, 0x0000 );
		WriteCmdData( 0x83, 0x0000 );
		WriteCmdData( 0x84, 0x0000 );
		WriteCmdData( 0x85, 0x0000 );
		//-------------- Panel Control -------------------//
		WriteCmdData( 0x90, 0x0010 );
		WriteCmdData( 0x92, 0x0000 );
		WriteCmdData( 0x07, 0x0133 ); // 262K color and display ON
		break;
        case 0x9488:
		_lcd_capable = AUTO_READINC | TWO_PARM_WINDOW | MV_AXIS;
// from Adafruit_diger67
#define DUMMY 0x0000
    WriteCmdData(0x01, DUMMY);    //Soft Reset
    wait_ms(50);
    WriteCmdData(0x28, DUMMY);    //Display Off
/*
    WriteCmdData(0xC0, 0x2300);    //Power Control 1 [0x0E0E]
    WriteCmdData(0xC1, 0x1000);    //Power Control 2 [0x43XX]
 
    WriteCmdData(0xC5, 0x2B2B);    //VCOM  Control 1 [0x00400040]
    WriteCmdData(0xC6, 0xC000);    //VCOM  Control 2 [0xE001]
*/
    WriteCmdData(0x36, 0x4800);    //Memory Access
    WriteCmdData(0x3A, 0x5500);    //Interlace Pixel   
    
    WriteCmdData(0xB1, 0x001B);    //Frame Rate Control [0xB011]

    WriteCmdData(0xB7, 0x0700);    //Entry Mode      [0x06XX]

    WriteCmdData(0x11, DUMMY);    //Sleep Out
    wait_ms(120);
    WriteCmdData(0x29, DUMMY);    //Display On
    WriteCmdData(0x2a, 0x0000);
    WriteCmdData(0x2b, 0x0000);
		p16 = (int16_t *)&HEIGHT;
		*p16 = 480;
		p16 = (int16_t *)&WIDTH;
		*p16 = 320;
            break;
	case 0xB509:
		_lcd_capable = AUTO_READINC;
        WriteCmdData(0x0000, 0x0000);
        WriteCmdData(0x0000, 0x0000);
        WriteCmdData(0x0000, 0x0000);
        WriteCmdData(0x0000, 0x0000);
        wait_ms(15);
        WriteCmdData(0x0400, 0x6200);
        WriteCmdData(0x0008, 0x0808);
    //gamma
        WriteCmdData(0x0300, 0x0C00);
        WriteCmdData(0x0301, 0x5A0B);
        WriteCmdData(0x0302, 0x0906);
        WriteCmdData(0x0303, 0x1017);
        WriteCmdData(0x0304, 0x2300);
        WriteCmdData(0x0305, 0x1700);
        WriteCmdData(0x0306, 0x6309);
        WriteCmdData(0x0307, 0x0C09);
        WriteCmdData(0x0308, 0x100C);
        WriteCmdData(0x0309, 0x2232);

        WriteCmdData(0x0010, 0x0016);      //69.5Hz         0016
        WriteCmdData(0x0011, 0x0101);
        WriteCmdData(0x0012, 0x0000);
        WriteCmdData(0x0013, 0x0001);

        WriteCmdData(0x0100, 0x0330);      //BT,AP
        WriteCmdData(0x0101, 0x0237);      //DC0,DC1,VC
        WriteCmdData(0x0103, 0x0D00);      //VDV
        WriteCmdData(0x0280, 0x6100);      //VCM
        WriteCmdData(0x0102, 0xC1B0);      //VRH,VCMR,PSON,PON
        wait_ms(50);

        WriteCmdData(0x0001, 0x0100);
        WriteCmdData(0x0002, 0x0100);
        WriteCmdData(0x0003, 0x1030);       //1030
        WriteCmdData(0x0009, 0x0001);
        WriteCmdData(0x000C, 0x0000);
        WriteCmdData(0x0090, 0x8000);
        WriteCmdData(0x000F, 0x0000);

        WriteCmdData(0x0210, 0x0000);
        WriteCmdData(0x0211, 0x00EF);
        WriteCmdData(0x0212, 0x0000);
        WriteCmdData(0x0213, 0x018F);       //432=01AF,400=018F
        WriteCmdData(0x0500, 0x0000);
        WriteCmdData(0x0501, 0x0000);
        WriteCmdData(0x0502, 0x005F);
        WriteCmdData(0x0401, 0x0001);
        WriteCmdData(0x0404, 0x0000);
        wait_ms(50);

        WriteCmdData(0x0007, 0x0100);         //BASEE
        wait_ms(50);

        WriteCmdData(0x0200, 0x0000);
        WriteCmdData(0x0201, 0x0000);
		p16 = (int16_t *)&HEIGHT;
		*p16 = 400;
		break;	
	}
	setRotation(0);                //PORTRAIT
}

