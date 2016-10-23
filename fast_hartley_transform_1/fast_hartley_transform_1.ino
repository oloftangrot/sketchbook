/*
fft_adc.pde
guest openmusiclabs.com 8.18.12
example sketch for testing the fft library.
it takes in data on ADC0 (Analog0) and processes them
with the fft. the data is sent out over the serial
port at 115.2kb.  there is a pure data patch for
visualizing the data.
*/

// do #defines BEFORE #includes
#define LOG_OUT 1 // use the log output function
#define FHT_N 128 // set to 256 point fft

#include <FHT.h> // include the library

uint8_t test[ FHT_N ];

void setup() {
  Serial.begin(115200); // use the serial port
  TIMSK0 = 0; // turn off timer0 for lower jitter - delay() and millis() killed
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  for ( int i = 0; i < FHT_N/2; i++ ) {
    if ( i < FHT_N/4 ) {
      test[i] = i;
    }
    else {
      test[i] = FHT_N/2 - i;
    }
  }
}

void loop() {
  while(1) { // reduces jitter
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for ( int i = 0 ; i < FHT_N ; i++ ) { // save FHT_N samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into even bins
//      test[ i/2] = ( (j<<8)|m) / 10;
    }
    // window data, then reorder, then run, then take output
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fft
    fht_run(); // process the data in the fft
    fht_mag_log(); // take the output of the fft
    sei(); // turn interrupts back on
    Serial.write(255); // send a start byte
    Serial.write( fht_log_out, FHT_N/2 ); // send out the data
//    Serial.write( test, FT_N ); // send out the data
//    delay( 1000 );
    }
}
