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
const int sig1_table_size = 128; 
const int sig2_table_size = 100; 

unsigned int i;
unsigned int time;
unsigned int sig1[sig1_table_size];
unsigned int sig2[sig2_table_size];
boolean sig = false;
unsigned int siz = sig1_table_size;
unsigned int* tab = sig1;

void setup() {
  pinMode (slaveSelectPin, OUTPUT);   // set the slaveSelectPin as an output:
  SPI.begin();   // initialize SPI:
  SPI.setBitOrder( MSBFIRST ) ;
  SPI.setClockDivider( SPI_CLOCK_DIV2 ); // Set clock to 8 MHz.
  for ( int j = 0; j < sig1_table_size; j++ ) {
    sig1[j] = (unsigned int) ( 512 + 511 * sin ( j * 2 * 3.1415f/sig1_table_size ) );
  }
  for ( int j = 0; j < sig2_table_size; j++ ) {
    sig2[j] = (unsigned int) ( 512 + 511 * sin ( j * 2 * 3.1415f/sig2_table_size ) );
  }
}

void loop() { 
#if 0  
  tlc5615Write( i );
  i++;
  if ( i >= 1024 ) i = 0; // ~~~
#endif
#if 1
  tlc5615Write( tab[i++] );
  if ( i >= siz )  i = 0; //  
  if ( time++ > 30000 ) {
    if ( sig ) {
      tab = sig2;
      siz = sig2_table_size;
    }
    else {
      tab = sig1;
      siz = sig1_table_size;
    }
    i = 0; // Just reset the table index
    sig = !sig;
    time = 0;
  }
#endif 
}

void tlc5615Write( unsigned int d ) {
  d = d << 2;   /* Condition the data, two LSB 0's, four MSB dummy bits */
  digitalWrite(slaveSelectPin,LOW);   // take the SS pin low to select the chip:
  SPI.transfer( d >> 8 );
  SPI.transfer( d );
  digitalWrite(slaveSelectPin,HIGH);   // take the SS pin high to de-select the chip:
}
