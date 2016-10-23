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
#define DDS_REF   180000000

// DDS Offset in Hz
#define DDS_OSET  164  //DDS #2

// WSPR Output Frequency
#define WSPR_TX_A    10.140100e6  // this is the bottom of the band. The station moves about.
#define WSPR_TX_B     7.040000e6  // this is the bottom of the band. The station moves about.

#define WSPR_DUTY  3 // transmit every N slices.

// BUNKER: GB0SNB 20dbm (100mw)
// WSPR Tone Data - line breaks in no parciular place, just to look pretty. (0-161).
static byte WSPR_DATA_BUNKER[] = {1,1,2,2,0,2,0,0,1,0,0,0,3,1,3,0,0,2,3,0,2,1,0,3,1,1,3,2,0,0,2,2,2,
              2,1,2,0,1,2,1,2,0,2,0,0,2,1,2,1,3,2,2,1,3,2,1,0,0,0,3,1,2,1,0,2,2,0,1,1,0,3,2,3,2,1,0,
              1,0,0,3,2,2,3,0,3,1,0,0,0,1,3,0,3,0,1,0,0,0,1,0,2,0,2,2,3,2,2,3,0,2,3,3,1,0,3,1,0,0,3,
              3,2,1,2,0,0,3,3,1,0,2,0,0,2,1,0,1,0,0,1,1,2,2,0,0,0,2,2,3,3,0,1,0,1,3,0,2,2,3,1,0,2,2}; // 162 bits

//HOME: M1GEO 23dbm (200mw)
static byte WSPR_DATA_HOME[] = {3,3,0,0,2,0,2,0,1,0,2,0,1,3,1,2,2,2,3,0,2,1,2,3,1,1,1,2,0,0,0,2,0,
            2,3,0,0,1,2,1,2,2,0,0,2,2,3,0,1,1,2,2,3,3,2,1,2,2,0,1,3,0,3,2,2,0,2,3,3,0,3,0,3,0,3,0,
            1,0,2,3,2,2,3,2,3,1,0,2,2,1,1,2,1,0,3,0,2,2,1,2,0,0,0,0,1,2,2,1,0,2,3,3,1,0,3,3,0,2,1,
            1,0,3,0,0,2,3,3,3,2,0,0,0,0,3,0,3,2,2,3,1,0,2,0,0,2,2,2,3,3,2,3,2,3,3,2,0,0,3,1,0,2,2}; // 162 bits

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
#include <TinyGPS.h>

// GPS Stuff
TinyGPS gps;
SoftwareSerial nss(3, 4);  //GPS RX 3, TX 4

// Variables
unsigned long  WSPR_TXF = 0;
unsigned long startT = 0, stopT = 0;
int year;
byte month, day, hour, minute, second, hundredths, Nsatellites, ret, duty, band;
unsigned long fix_age, fail;
char sz[32];


void setup()
{
  // Set all pins to output states
  pinMode (DDS_DATA,  OUTPUT);
  pinMode (DDS_CLOCK, OUTPUT);
  pinMode (DDS_LOAD,  OUTPUT);
  pinMode (LED,       OUTPUT);
  pinMode (GPS_LED,   OUTPUT);
  
  // Setup RS232
  Serial.begin(4800);
  //nss.begin(4800);
  
  sprintf(sz, "\nM1GEO Compiled %s %s", __TIME__, __DATE__);
  Serial.println(sz);
  
  Serial.print("\n\nDDS Reset   ");
  delay(900);
  frequency(0);
  delay(100);
  Serial.println("OK");
  
  duty = 0;
  
}

void loop()
{
  fail++;
  ret = feedgps();
  
  if (fail == 60000) {
    digitalWrite (GPS_LED, LOW);
    Serial.println("GPS: No Data.");
  }
  
  if (ret>0) {
    Nsatellites = gps.satellites();
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &fix_age);
    if (fix_age == TinyGPS::GPS_INVALID_AGE) {
      Serial.println("GPS: No Fix.");
      digitalWrite (GPS_LED, LOW);
    } else {
      digitalWrite (GPS_LED, HIGH);
      sprintf(sz, "Date %02d/%02d/%02d, Time %02d:%02d:%02d (Sats: %02d, WSPR Duty: %d, GPS Age: %lu ms, GPS Feed: %lu).", day, month, year, hour, minute, second, Nsatellites, (duty % WSPR_DUTY), fix_age, fail);
      Serial.println(sz);
      
      if(band % 2 == 0) {
        WSPR_TXF = (WSPR_TX_A+DDS_OSET) + random(10, 190); // always choose a frequency, it mixes it all up a little with the pRNG.
      } else {
        WSPR_TXF = (WSPR_TX_B+DDS_OSET) + random(10, 190); // always choose a frequency, it mixes it all up a little with the pRNG.
      }
      
      if ( (minute % 2 == 0) && (second >= 1) && (second <= 4) ) {  // start transmission between 1 and 4 seconds, on every even minute
        if (duty % WSPR_DUTY == 0) {
          Serial.print("Beginning WSPR Transmission on ");
          Serial.print(WSPR_TXF-DDS_OSET);
          Serial.println(" Hz.");
          wsprTX();
          duty++;
          band++;
          Serial.println(" Transmission Finished.");    
        } else {
          duty++;
          digitalWrite (LED, LOW);
          digitalWrite (GPS_LED, LOW);
          delay(5000); // so we miss the window to start TX.
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
  if ((t >= 0) && (t <= 3) ) {
    frequency((WSPR_TXF + (t * 2))); // should really be 1.4648 Hz not 2.
  } else {
    Serial.print("Tone #");
    Serial.print(t);
    Serial.println(" is not valid.  (0 <= t <= 3).");
  }
}

void wsprTX() {
  int i = 0;

  digitalWrite(LED, HIGH);
  for (i=0;i<162;i++) {
    wsprTXtone( WSPR_DATA[i] );
    delay(682);
  }
  frequency(0);
  digitalWrite(LED, LOW);
}

static bool feedgps()
{
  while (Serial.available()) {
    if (gps.encode(Serial.read())) {
      return true;
    }
  }
  return false;
}

