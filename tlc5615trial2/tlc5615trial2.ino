/*
  DAC using TLC5615, atempt to speed up output by pre calculating the SPI data
  
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
typedef struct _preCalc {
  unsigned char hi;
  unsigned char lo;
} preCalc;

/* Forward declarations of local function prototypes */
void tlc5615Write( preCalc pC ); 
void tlc5615Write( unsigned int d );

/* Typed constants */
const int slaveSelectPin = 10; // set pin 10 as the slave select for the digital pot:
const int sig1_table_size = 128; 
const int sig2_table_size = 100; 
const int sig3_table_size = 128; 
const int sig4_table_size = 100; 

/* Program variables */
unsigned int i;
unsigned int time;
unsigned int sig1[sig1_table_size];
unsigned int sig2[sig2_table_size];
unsigned int siz = sig1_table_size;
unsigned int* tab = sig1;

preCalc sig3[sig3_table_size];
preCalc sig4[sig4_table_size];
preCalc *pCtab = sig3;
unsigned int pCsiz = sig3_table_size;

boolean sig = false;

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
  for ( int j = 0; j < sig3_table_size; j++ ) {    
    unsigned int t = (unsigned int) ( 512 + 511 * sin ( j * 2 * 3.1415f/sig3_table_size ) );
    t = t << 2; // Shift in the two dummy bits.
    sig3[j].lo = t;
    sig3[j].hi = t>>8;
  }
  for ( int j = 0; j < sig4_table_size; j++ ) {
     unsigned int t = (unsigned int) ( 512 + 511 * sin ( j * 2 * 3.1415f/sig4_table_size ) );
    t = t << 2; // Shift in the two dummy bits.
    sig4[j].lo = t;
    sig4[j].hi = t>>8;
  }
}

void loop() { 
#if 0  
  tlc5615Write( i );
  i++;
  if ( i >= 1024 ) i = 0; // ~~~
#endif
#if 0
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
  dfsdf
#endif
#if 1
  tlc5615Write( pCtab[i++] );
  if ( i >= pCsiz )  i = 0; // 
  if ( time++ > 30000 ) {
    if ( sig ) {
      pCtab = sig4;
      pCsiz = sig4_table_size;
    }
    else {
      pCtab = sig3;
      pCsiz = sig3_table_size;
    }
    i = 0; // Just reset the table index
    sig = !sig;
    time = 0;
  }
#endif
}
/*
** Write routine for 10 bit data to be sent in a 16 bit package for a TCL5615.
*/
void tlc5615Write( unsigned int d ) {
  d = d << 2;   /* Condition the data, two LSB 0's, four MSB dummy bits */
  digitalWrite(slaveSelectPin,LOW);   // take the SS pin low to select the chip:
  SPI.transfer( d >> 8 );
  SPI.transfer( d );
  digitalWrite(slaveSelectPin,HIGH);   // take the SS pin high to de-select the chip:
}
/*
** Write routine for data that has already been prepared with dummy bits and separated
** into a high and low octet.
*/
void tlc5615Write( preCalc pC ) {
  PORTB &= ~_BV( PB2 );   // Set the SS pin low to select the chip:
//  digitalWrite(slaveSelectPin,LOW);   // take the SS pin low to select the chip:
  SPI.transfer( pC.hi );
  SPI.transfer( pC.lo );
  PORTB |= _BV( PB2 );   // Set the SS pin high to unselect the chip:
//  digitalWrite(slaveSelectPin,HIGH);   // take the SS pin high to de-select the chip:
}

