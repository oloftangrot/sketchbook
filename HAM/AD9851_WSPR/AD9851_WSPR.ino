// WSPR Transmitter from AD9851
// George Smart, M1GEO.
//  here: http://www,george-smart.co.uk/wiki/Arduino_WSPR
//
// AD9851 Code
// Originally by Peter Marks
//  here: http://blog.marxy.org/2008/05/controlling-ad9851-dds-with-arduino.html
//
// GPS provided by TinyGPS
//  here: http://arduiniana.org/libraries/tinygps/
//

// DDS Reference Oscilliator Frequency, in Hz. (Remember, the PLL).
#define DDS_REF   125000000

// DDS Offset in Hz
#define DDS_OSET  164  //DDS #2

// WSPR Output Frequency
#define WSPR_TX_A    10.140100e6  // this is the bottom of the band. The station moves about.
#define WSPR_TX_B     7.040000e6  // this is the bottom of the band. The station moves about.

#define WSPR_DUTY  3 // transmit every N slices.
/*
** ./wspr0 -t -X -c SA2KAA -g KP03dv -dBm 20 | awk '{print $2}' | sed ':a;N;$!ba;s/\n/,/g'
*/ 
//HOME: SA2KAA -20dbm (???mw)
static byte WSPR_DATA_HOME[] = {
  3,3,2,2,2,2,2,2,3,2,0,0,3,1,1,0,2,0,1,2,2,3,2,3,3,1,1,2,0,2,0,2,
  2,0,3,0,0,1,0,1,0,0,2,0,0,2,3,2,3,1,0,2,1,3,0,1,0,0,0,1,1,2,3,0,
  0,2,0,1,3,2,3,2,1,0,1,0,1,0,0,3,0,0,3,0,1,1,2,0,2,1,1,2,1,2,3,0,
  0,2,1,0,0,0,0,2,3,0,2,1,2,0,3,3,3,2,3,3,2,0,3,3,0,3,0,2,0,1,3,1,
  2,2,2,0,0,3,2,3,2,0,1,3,2,2,2,0,0,0,2,3,1,2,3,0,3,3,0,0,2,1,1,0,
  2,0
}; // 162 bits

#define WSPR_DATA WSPR_DATA_HOME

// DDS/Arduino Connections
#define DDS_LOAD  8
#define DDS_CLOCK 9
#define DDS_DATA  10
#define LED       13
#define GPS_LED   12

// Libraries
#include <stdint.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// GPS Stuff
TinyGPSPlus gps;
SoftwareSerial nss(3, 4);  //GPS RX 3, TX 4

// Variables
unsigned long  WSPR_TXF = 0;
unsigned long startT = 0, stopT = 0;
int year;
byte month, day, hour, minute, second, hundredths, Nsatellites, ret, duty, band;
unsigned long fix_age, fail;
char sz[32];

// Prototype declarations
void ad9850_test( void );
void sendFrequency( float frequency );

// Arduino initializer function.
void setup()
{
  // Set all pins to output states
  pinMode (DDS_DATA,  OUTPUT);
  pinMode (DDS_CLOCK, OUTPUT);
  pinMode (DDS_LOAD,  OUTPUT);
  pinMode (LED,       OUTPUT);
  pinMode (GPS_LED,   OUTPUT);
  
  // Setup RS232
  Serial.begin( 115200 );
  nss.begin( 9600 );
  
//  sprintf(sz, "\nSA2KAA Compiled %s %s", __TIME__, __DATE__);
  Serial.print( F("\nSA2KAA Compiled ") );
  Serial.print( F(__TIME__) );
  Serial.println( F(__DATE__) );
  
  Serial.print( F("\n\nDDS Reset " ) );
  delay(900);
  sendFrequency( 0.f );
  delay(100);
  Serial.println( F("OK") );
  duty = 0;
}

void loop()
{
//  ad9850_test(); // Test infinately
  fail++;
  ret = feedgps();
  
  if ( fail == 60000 ) {
    digitalWrite (GPS_LED, LOW);
    Serial.println( F("GPS: No Data.") );
    fail = 0;
  }
  
  if ( ret > 0 ) {
//    Nsatellites = gps.satellites();
   Nsatellites = 1;
//    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &fix_age);
//    if (fix_age == TinyGPS::GPS_INVALID_AGE) {
    if ( !gps.time.isValid() ) {
      Serial.println( F("GPS: No Fix.") );
      digitalWrite (GPS_LED, LOW);
    } 
    else {
      digitalWrite (GPS_LED, HIGH);
      hour =  gps.time.hour();
      minute = gps.time.minute();
      second = gps.time.second();
//      sprintf(sz, "Date %02d/%02d/%02d, Time %02d:%02d:%02d (Sats: %02d, WSPR Duty: %d, GPS Age: %lu ms, GPS Feed: %lu).", day, month, year, hour, minute, second, Nsatellites, (duty % WSPR_DUTY), fix_age, fail);
//      sprintf(sz, "Time %02d:%02d:%02d ", hour, minute, second );
//      Serial.println(sz);
      
      //(Sats: %02d, WSPR Duty: %d, GPS Age: %lu ms, GPS Feed: %lu).", Nsatellites, (duty % WSPR_DUTY), fix_age, fail);
   
  //    Serial.println(sz);
      Serial.print( F( "Time") ); Serial.print( hour ); Serial.print( " " ); Serial.print( minute ); Serial.print( " " ); Serial.println( second );
      if(band % 2 == 0) {
        WSPR_TXF = (WSPR_TX_A+DDS_OSET) + random(10, 190); // always choose a frequency, it mixes it all up a little with the pRNG.
      } else {
        WSPR_TXF = (WSPR_TX_B+DDS_OSET) + random(10, 190); // always choose a frequency, it mixes it all up a little with the pRNG.
      }
      
      if ( ( minute % 2 == 0) && (second >= 1) && (second <= 4) ) {  // start transmission between 1 and 4 seconds, on every even minute
        if ( duty % WSPR_DUTY == 0 ) {
          Serial.print(F("Beginning WSPR Transmission on ") );
          Serial.print(WSPR_TXF-DDS_OSET);
          Serial.println( F(" Hz.") );
          wsprTX();
          duty++;
          band++;
          Serial.println( F(" Transmission Finished.") );    
        } else {
          duty++;
          digitalWrite (LED, LOW);
          digitalWrite (GPS_LED, LOW);
          delay( 5000 ); // so we miss the window to start TX.
          flash_led(Nsatellites, GPS_LED); // flash the number of GPSes we have.
          flash_led(WSPR_DUTY, LED); // flash the WSPR duty.
        }
      }
      digitalWrite (LED, HIGH);
      delay(250);
      digitalWrite (LED, LOW);
      delay(250);
    }
    fail = 0;
  }
}
#if 0
void frequency(unsigned long frequency) {
  unsigned long tuning_word = (frequency * pow(2, 32)) / DDS_REF;
  digitalWrite (DDS_LOAD, LOW); // take load pin low

  for(int i = 0; i < 32; i++) {
    if ((tuning_word & 1) == 1)
      outOne();
    else
      outZero();
    tuning_word = tuning_word >> 1;
  }
  byte_out(0x09);
  digitalWrite (DDS_LOAD, HIGH); // Take load pin high again
}

void byte_out(unsigned char byte) {
  int i;

  for (i = 0; i < 8; i++) {
    if ((byte & 1) == 1)
      outOne();
    else
      outZero();
    byte = byte >> 1;
  }
}

void outOne() {
  digitalWrite(DDS_CLOCK, LOW);
  digitalWrite(DDS_DATA, HIGH);
  digitalWrite(DDS_CLOCK, HIGH);
  digitalWrite(DDS_DATA, LOW);
}

void outZero() {
  digitalWrite(DDS_CLOCK, LOW);
  digitalWrite(DDS_DATA, LOW);
  digitalWrite(DDS_CLOCK, HIGH);
}
#endif

void flash_led(unsigned int t, int l) {
  unsigned int i = 0;
  if (t > 25) {
    digitalWrite(l, HIGH);
    delay(2000);
    digitalWrite(l, LOW);
  } else {
    for (i=0;i<t;i++) {
      digitalWrite(l, HIGH);
      delay(250);
      digitalWrite(l, LOW);
      delay(250);
    }
  }
}

void wsprTXtone(int t) {
  if ( ( t >= 0 ) && ( t <= 3 ) ) {
//    sendFrequency((WSPR_TXF + (t * 2))); // should really be 1.4648 Hz not 2.
    sendFrequency( ( WSPR_TXF + ( t * 1.4648f ) ) ); // should really be 1.4648 Hz not 2.
  } else {
    Serial.print( F("Tone #") );
    Serial.print(t);
    Serial.println( F(" is not valid.  (0 <= t <= 3).") );
  }
}

void wsprTX() {
  int i = 0;

  digitalWrite(LED, HIGH);
  for ( i = 0; i < 162; i++ ) {
    wsprTXtone( WSPR_DATA[i] );
    delay( 682 );
  }
  sendFrequency( 0.f );
  digitalWrite( LED, LOW );
}

static bool feedgps()
{
  while ( nss.available() ) {
    if ( gps.encode( nss.read() ) ) {
//      Serial.println( F("3>") );
      return true;
    }
  }
  return false;
}
#define pulseHigh(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }

// transfer a byte a bit at a time LSB first to DATA
void tfr_byte(byte data)
{
  for ( int i = 0; i < 8; i++, data >>= 1) {
    digitalWrite( DDS_DATA, data & 0x01 );
    pulseHigh( DDS_CLOCK);
  }
}

// frequency of signwave (datasheet page 12) will be <sys clock> * <frequency tuning word> / 2^32
void sendFrequency( float frequency) {
  int32_t freq = frequency * 4294967296.0f / 125.0e6f;
  for ( int b = 0; b < 4; b++, freq >>=8 ) {
    tfr_byte( freq & 0xFF );
  }
//  tfr_byte(0x001);
  tfr_byte( 0x0 );  // Phase and control register
  pulseHigh( DDS_LOAD );
}


void ad9850_test( void ) {
  for( ;; ) {
    sendFrequency( 1200 );
    delay( 200 );
    sendFrequency( 2400 );
    delay( 200 );
  }
}
