/*
  DAC using TLC5615
  
 The circuit:
  * DIP 8  of TLC5615 connected to +5V
  * DIP 5 of TLC5615 connected to ground
  * DIP 3 CS of TLC5615 connected digital pin 10  (SS pin)
  * DIP 1 DIN of TLC5615 connected ISP 4 (MOSI pin)
  * DIP 2 SCLK of TLC5615 connectedPin ISP 2 (SCK pin)
 
 created 17 Jan 2015 
 by Olof Tångrot
 
*/
#include <SPI.h> // include the SPI library:
const int slaveSelectPin = 10; // set pin 10 as the slave select for the digital pot:

unsigned int i;

void setup() {
  pinMode (slaveSelectPin, OUTPUT);   // set the slaveSelectPin as an output:
  SPI.begin();   // initialize SPI:
  SPI.setBitOrder( MSBFIRST ) ;
}

void loop() { 
  tlc5615Write( i );
  i++;
  if ( i >= 1024 ) i = 0; // /|/|/|/|/|/|/|/|
}

void tlc5615Write( unsigned int d ) {
  d = d << 2;   /* Condition the data, two LSB 0's, four MSB dummy bits */
  digitalWrite(slaveSelectPin,LOW);   // take the SS pin low to select the chip:
  SPI.transfer( d >> 8 );
  SPI.transfer( d );
  digitalWrite(slaveSelectPin,HIGH);   // take the SS pin high to de-select the chip:
}
