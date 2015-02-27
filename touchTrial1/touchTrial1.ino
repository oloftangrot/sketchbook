// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'
// This demo code returns raw readings, public domain

#include <stdint.h>
#include "TouchScreen.h"

// These are the pins for the shield!
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

#define HC245 2 // The enable pin for the resitive/ili9327 transceiver
#define MINPRESSURE 10
#define MAXPRESSURE 10000

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

void setup(void) {
  Serial.begin(9600);
  pinMode( HC245, OUTPUT );
  digitalWrite( HC245, LOW );
}
int x, y;
void loop(void) {
#if 0
  // a point object holds x y and z coordinates
  TSPoint p = ts.getPoint();
  
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
     Serial.print("X = "); Serial.print(p.x);
     Serial.print("\tY = "); Serial.print(p.y);
     Serial.print("\tPressure = "); Serial.println(p.z);
  }
#endif
  x = 0, y = 0;
  for ( int i = 0; i < 10; i++ ) {
    x += ts.readTouchX();
    y += ts.readTouchY();
  }
  x = x / 10 - 750;
  y = y / 10 - 750;
  if ( x < 260 && y < 260 ) {
    Serial.print("X = "); Serial.print( x ); 
    Serial.print("\tY = "); Serial.println( y );
  }
}
