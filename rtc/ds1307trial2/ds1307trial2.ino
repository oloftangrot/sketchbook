// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

bool f = false;
const unsigned char relay = 10;
void timer( void );

const unsigned char batteryPin = A0;
// Input voltage devider constants
const unsigned char Rgnd = 10; 
const unsigned char Rload = 47;
const unsigned char Vscale = 50; // 50 = 5 V * 10 
const unsigned int  adcMax = 1024;
int batteryVoltage;
int inputVoltage( int pinVoltage );

void setup () {
  pinMode( relay, OUTPUT );
  f = false;
  digitalWrite( relay, 1 );
  Serial.begin(57600);
#ifdef AVR
  Wire.begin();
#else
  Wire1.begin(); // Shield I2C pins connect to alt I2C bus on Arduino Due
#endif
  rtc.begin();

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
//    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    rtc.adjust(DateTime(2015, 4, 26, 7, 16, 0));
  }
//  rtc.adjust(DateTime(2015, 4, 26, 19, 16, 0));

}

void loop () {
    batteryVoltage = inputVoltage( analogRead( batteryPin) );  
    DateTime now = rtc.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    Serial.print(" since midnight 1/1/1970 = ");
    Serial.print(now.unixtime());
    Serial.print("s = ");
    Serial.print(now.unixtime() / 86400L);
    Serial.println("d");
    
    // calculate a date which is 7 days and 30 seconds into the future
    DateTime future (now.unixtime() + 7 * 86400L + 30);
    
    Serial.print(" now + 7d + 30s: ");
    Serial.print(future.year(), DEC);
    Serial.print('/');
    Serial.print(future.month(), DEC);
    Serial.print('/');
    Serial.print(future.day(), DEC);
    Serial.print(' ');
    Serial.print(future.hour(), DEC);
    Serial.print(':');
    Serial.print(future.minute(), DEC);
    Serial.print(':');
    Serial.print(future.second(), DEC);
    Serial.println();
    
    Serial.println();
    delay(3000);
    timer() ;
#if 0
    digitalWrite( relay, f );
    if ( f ) {
      f = false;
    }
    else {
      f = true;
    }
#endif
}

struct myTime {
  unsigned char h, m;
};

struct myTime onTime {
  21, 39
};

struct myTime offTime {
  21, 40
};

int myTime2int( struct myTime & mT ) {
  return 60 * mT.h + mT.m;
}
void timer ( void )
{
  DateTime now = rtc.now();
  int current = now.hour() * 60 + now.minute();
  const int on = myTime2int( onTime );
  const int off = myTime2int( offTime );
  
  if ( on < off ) {  // This happens when on time is like 01:00 and off time is like 02:00
    if ( ( current < on ) || ( current >= off ) ) { // Relay shall be off!
      Serial.println( "Should be off!" );           
      if ( f ) {
        digitalWrite( relay, 1 );
        f = false;
      }
    }
    if ( ( current >=  on ) && ( current < off ) ) {
      Serial.println( "Should be on!" );           
       if ( !f ) {
        digitalWrite( relay, 0 );
        f = true;
      }
    }
  }
  else { // on > off, this happens when on time is like 23:00 and off time is 01:00
    if ( ( current < on ) && ( current >= off ) ) { // Relay shall be off!
      Serial.println( "Should be off!" );           
      if ( f ) {
        digitalWrite( relay, 1 );
        f = false;
      }
    }
    if ( ( current >=  on ) || ( current < off ) ) {
      Serial.println( "Should be on!" );           
       if ( !f ) {
        digitalWrite( relay, 0 );
        f = true;
      }
    }
  }
}
// Model conversion from voltage divider net.
int inputVoltage( int pinVoltage )
{
   unsigned int tmp = Vscale * pinVoltage;
   tmp = tmp / Rgnd;
   return ( ( Rgnd + Rload ) * tmp )/ adcMax ; 
}
