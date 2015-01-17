#include <SoftwareServo.h>

const int ledPin = 13;
const int button1 = 7;   
const int button2 = 6; 
const int button3 = 5; 
const int servoPin = 12;
const int delayPressed = 30;
const int delayUnPressed = 20;

int val = 90;     // variable to store the read value
boolean b1, b2, b3;
unsigned int cnt = 20; // delay counter

SoftwareServo servo;

void setup()
{
  pinMode( ledPin, OUTPUT );      // sets the digital pin 13 as output
  pinMode( button1, INPUT_PULLUP );      // sets the digital pin as input
  pinMode( button2, INPUT_PULLUP );      // sets the digital pin as input
  pinMode( button3, INPUT_PULLUP );      // sets the digital pin as input
  servo.attach( servoPin ); //analog pin 0
  servo.setMaximumPulse(2400);
  servo.setMinimumPulse(900); 
  
//  Serial.begin(9600);           // set up Serial library at 9600 bps
//  Serial.println("Hello world!");  // prints hello with ending line break 
}

void loop()
{
  
  b1 = !digitalRead( button1 );   // read the input pin
  b2 = !digitalRead( button2 );   // read the input pin
  b3 = !digitalRead( button3 );   // read the input pin
  
  if ( b1 && !b3 ) {
    if ( ( 0 == cnt ) && ( val < 180 ) ) { 
      val++; //
      cnt = delayPressed;
    }
    else if ( cnt > 0 ) cnt--;
  }
  
  if ( !b1 && b3 ) {
    if ( ( 0 == cnt ) && ( val > 0 ) ) { 
      val--; //
      cnt = delayPressed;
    }  
    else if ( cnt > 0 ) cnt--;
  }
  if ( b1 || b2 || b3 ) {
    digitalWrite( ledPin, 1 );    // sets the LED to the button's value
//    Serial.println( val );
  }
  else {
    cnt = delayUnPressed;
    digitalWrite( ledPin, 0 );
  }
  servo.write( val );
 
  servo.refresh();
}
