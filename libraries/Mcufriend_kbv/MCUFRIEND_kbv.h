#ifndef MCUFRIEND_KBV_H_
#define MCUFRIEND_KBV_H_

//#include "ADA_GFX_kbv.h"
#include "Adafruit_GFX.h"

class MCUFRIEND_kbv : public Adafruit_GFX {

 public:
  MCUFRIEND_kbv();
  void     reset(void);                                       // you only need the constructor
  void     begin(uint16_t ID);                                       // you only need the constructor
  virtual void     drawPixel(int16_t x, int16_t y, uint16_t color);  // and these three
  void     WriteCmdData(uint16_t cmd, uint16_t dat);                 // public methods !!!
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b) { return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3); }
  uint16_t readID(void);
//  void     print(const char *s)   { while (*s) write(*s++); }
//  void     println(const char *s) { print(s); print("\n"); }
  virtual void     fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  virtual void     drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) { fillRect(x, y, 1, h, color); }
  virtual void     drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) { fillRect(x, y, w, 1, color); }
  virtual void     fillScreen(uint16_t color)                                     { fillRect(0, 0, _width, _height, color); }
  virtual void     setRotation(uint8_t r);

  uint16_t readReg(uint16_t reg);
  uint32_t readReg32(uint16_t reg);
  int16_t  readGRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h);
  uint16_t readPixel(int16_t x, int16_t y) { uint16_t color; readGRAM(x, y, &color, 1, 1); return color; }
  void    setAddrWindow(int16_t x, int16_t y, int16_t x1, int16_t y1);
  void write_data_block(uint16_t *block, int16_t n);
  void pushColors(uint16_t *block, int16_t n, uint8_t first);
  void     vertScroll(int16_t top, int16_t scrollines, int16_t offset);
  
 private:
    uint16_t        _lcd_ID = 0x9325, _lcd_capable = 0, _MC, _MP, _MW, _SC, _EC, _SP, _EP;
};

#endif
