#include <Servo.h>

#define PIN_O1 6
#define PIN_I1 2

int a;

void setup()
{
  pinMode( PIN_O1, OUTPUT);
  pinMode( PIN_I1, INPUT );
}


void loop()
{
  
  a = digitalRead ( PIN_I1 );
  
  digitalWrite( PIN_O1, a );
//  delay( 1000 );
//  digitalWrite( PIN_O1, HIGH );  
//  delay( 1000 );
}
