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

#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

SoftwareSerial mySerial( 5, 6 ); // RX, TX


uint8_t test[ FFT_N ];
//RunningAverage ma[FFT_N / 2];
bool f = false;
bool playing = false;
unsigned int ledDelay = 0;
unsigned int playDelay;

void setup() {
//  Serial.begin( 9600 ); // use the serial port for the MiniMP3 player.
//  Serial.begin(115200); // use the serial port for remote spectrum display.
  TIMSK0 = 0; // turn off timer0 for lower jitter - delay() and millis() killed
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
//  for ( int i = 0; i < FFT_N / 2; i++ ) // Assign sizes to the moving average filters.
//    (void) ma[i].setSize( 8 ); 
  for ( int i = 0; i < FFT_N/2; i++ ) {
    if ( i < FFT_N/4 ) {
      test[i] = i;
    }
    else {
      test[i] = FFT_N/2 - i;
    }
  }
  pinMode( LED, OUTPUT);     
  
  mySerial.begin( 9600 ); // Set speed for MiniMP3 player
  mp3_set_serial( mySerial );	//set softwareSerial for DFPlayer-mini mp3 module 
  delay( 1 );  //wait 1ms for mp3 module to set volume
  mp3_set_volume ( 30 );
}

void loop() {
  while(1) { // reduces jitter
  #if 0
    if ( (ledDelay % 64) == 0 ) { // Create a blinking action on LED
      digitalWrite( LED, f );
      if (f) f = false;
      else f = true;
    }
  #endif
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
    if ( fft_log_out[1] > 150  )
    {
      digitalWrite( LED, HIGH );
      if ( false == playing ) { // Start playing an prevent a new start.
        playing = true;
        playDelay = 0; // Reset playDelay counter
        mp3_play();
      }
    }
    else 
    {
      digitalWrite( LED, LOW );
    }
#if 0    
    for ( int i = 0; i < FFT_N / 2; i++ )
      ma[i].addValue( fft_log_out[i] );
    Serial.write(255); // send a start byte
    Serial.write( fft_log_out, FFT_N/2 ); // send out the data
#endif    
#if 0    
    for ( int i = 0; i < FFT_N / 2; i++ ) // Send moving average data
      Serial.write( ma[i].getAverage() );
#endif      
//    Serial.write( test, FFT_N ); // send out the data
//    delay( 1000 );
    if ( playing ) { // handle delay while playing
      playDelay++;
      if ( playDelay > 6000 ) {
        playing = false;
      }
    }
  }
}
