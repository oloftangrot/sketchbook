#define LCD_CS A3    
#define LCD_CD A2    
#define LCD_WR A1   
#define LCD_RD A0    
#define LCD_RESET A4
#define SD_CS 10 // Card select for shield use

//#include "TFTLCD.h"
#include <SD.h>

#define BUFFPIXEL 20

File root;
File bmpImage;

// TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

inline uint16_t Color565(uint8_t r, uint8_t g, uint8_t b) {
  uint16_t c;

  c = r >> 3;
  c <<= 6;
  c |= g >> 2;
  c <<= 5;
  c |= b >> 3;
  return c;
}

void setup(void) {

  Serial.begin(115200);
  Serial.println("APC magazine LCD BMP image viewer");
  
//  tft.reset();
 
//  tft.initDisplay();
//  tft.setRotation(1);
//  tft.fillScreen(0x0000); // fill screen with BLACK

  Serial.print( F( "Initializing SD card...") );
  pinMode( 10, OUTPUT );
  
  if ( !SD.begin( SD_CS ) ) {
    Serial.println( F( "failed!" ) );
    return;
  }
  Serial.println( F( "OK!" ) );

  root = SD.open("/");

}
 
void loop(void) {

//    tft.fillScreen(0x0000); // fill screen with BLACK
    bmpDrawFromRoot();
    delay( 5000 );
}

void bmpDrawFromRoot() {
  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col, tW, tH;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  boolean  first = true;
  
//  TODO tW = tft.width(); tH = tft.height();
  
  bmpFile = root.openNextFile();
  if (! bmpFile ) { root.rewindDirectory(); return; }  

  Serial.println();
  Serial.print("Loading image '");
  Serial.print(bmpFile.name());
  Serial.println('\'');

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print(F("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;
        Serial.print(F("Row Size: ")); Serial.println(rowSize);

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
       if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        Serial.print(F("bmp height: ")); Serial.println(h);
        Serial.print(F("TFT height: ")); Serial.println(tW);
        int start = bmpHeight+((tH-bmpHeight)/2);
        for (row=0; row<bmpHeight; row++) { // For each scanline...
            pos = bmpImageoffset + (row * rowSize);
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
// TODO            tft.goTo(0, start - row);

          for (col=0; col<bmpWidth; col++) { // For each column...
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            Color565( r, g, b );
// TODO            tft.writeData( Color565( r, g, b ) );
          } // end pixel
        } // end scanline
        Serial.print(F("Loaded in ")); Serial.print(millis() - startTime); Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println("BMP format not recognized.");
}

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
