// GetCode.ino
// 
// Basic example for the TOTP library
//
// This example uses the opensource SwRTC library as a software real-time clock
// you can download from https://github.com/leomil72/swRTC
// for real implementation it's suggested the use of an hardware RTC

#include "sha1.h"
#include "TOTP.h"
#include "RTClib.h"
#include "LedControl.h" 

// Pin 12 to Data In, 10 to Clk, 11 to LOAD, number of devices is 1 
LedControl lc=LedControl(12,10,11,1);


// The shared secret is MyLegoDoor
uint8_t hmacKey[] = {0x67, 0x64, 0x66, 0x64, 0x67, 0x64, 0x66, 0x67, 0x64, 0x66};

TOTP totp = TOTP(hmacKey, 10);
RTC_DS1307 rtc;
char code[7];

void setup() {
  
  Serial.begin( 115200 );
  if ( ! rtc.begin() ) {
    Serial.println( "Couldn't find RTC" );
    while ( 1 );
  }
  // Set up real time clock.
  if ( ! rtc.isrunning() ) {
    Serial.println( "RTC is NOT running!" );
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  // Initialize the MAX7219 device 
  for(int k=0; k < 2; k++){ 
    lc.shutdown(k,false); // Enable display 
    lc.setIntensity(k,2); // Set brightness level (0 is min, 15 is max)
    lc.clearDisplay(k); // Clear display register 
  } 
}

void loop() {
  DateTime now = rtc.now() ;

  long GMT = now.unixtime() - 3600 -25; // Adjust for GMT+1 TZ.
  char* newCode = totp.getCode( GMT );
  if(strcmp( code, newCode) != 0 ) {
    strcpy( code, newCode);
    for( int i = 0; i < 7; i++ ) {
      lc.setDigit( 0, 5 - i, code[i]-48, false );
    }  
    Serial.println(code);
    Serial.print(now.year(), DEC);
    Serial.print('-');
    Serial.print(now.month(), DEC);
    Serial.print('-');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    Serial.print(GMT);
    Serial.println();
  }  
//    Serial.print(GMT);
//    Serial.println();
}
