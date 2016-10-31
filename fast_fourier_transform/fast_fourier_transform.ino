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
#define FFT_N 32 // set to 256 point fft
#define LED 13 // Output pin to be used to indicate something.

#include <FFT.h> // include the library
#include <RunningAverage.h>


uint8_t test[ FFT_N ];
RunningAverage ma[FFT_N / 2];
bool f = false;
unsigned int ledDelay = 0;

void setup() {
  Serial.begin(115200); // use the serial port
  TIMSK0 = 0; // turn off timer0 for lower jitter - delay() and millis() killed
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
  for ( int i = 0; i < FFT_N / 2; i++ ) // Assign sizes to the moving average filters.
    (void) ma[i].setSize( 8 ); 
  for ( int i = 0; i < FFT_N/2; i++ ) {
    if ( i < FFT_N/4 ) {
      test[i] = i;
    }
    else {
      test[i] = FFT_N/2 - i;
    }
  }
  pinMode( LED, OUTPUT);     

}

void loop() {
  while(1) { // reduces jitter
    if ( (ledDelay % 64) == 0 ) {
      digitalWrite( LED, f );
      if (f) f = false;
      else f = true;
    }
    ledDelay++;
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FFT_N ; i += 2) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fft_input[i] = k; // put real data into even bins
      fft_input[i+1] = 0; // set odd bins to 0
//      test[ i/2] = ( (j<<8)|m) / 10;
    }
    // window data, then reorder, then run, then take output
    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_log(); // take the output of the fft
    sei(); // turn interrupts back on
    for ( int i = 0; i < FFT_N / 2; i++ )
      ma[i].addValue( fft_log_out[i] );
    Serial.write(255); // send a start byte
    Serial.write( fft_log_out, FFT_N/2 ); // send out the data
#if 1    
    for ( int i = 0; i < FFT_N / 2; i++ ) // Send moving average data
      Serial.write( ma[i].getAverage() );
#endif      
//    Serial.write( test, FFT_N ); // send out the data
//    delay( 1000 );
    }
}
