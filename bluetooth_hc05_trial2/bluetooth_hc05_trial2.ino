// This program shown how to control arduino from PC or Android Via Bluetooth
//
// The target hardware is a Arduino Leonardo with a separate
// USB-controller that is not connected to the UART.
//
// Connect ...
// arduino>>bluetooth
// D1 (Tx)   >>>  Rx
// D0 (Rx)  >>>  Tx

//#include <SoftwareSerial.h>// import the serial library
//#include <Servo.h>
#include <SoftwareServo.h>

SoftwareServo servo;

const int servoPin = 12;
//SoftwareSerial hc05( 10, 11 ); // RX, TX
#define hc05 Serial1

const char ledpin = 13; // led on D13 will show blink on / off
int BluetoothData; // the data given from Computer
//Servo servo;
int pwm = 120;
unsigned int repeatTimer = 0;
unsigned int pwmLoopTimer = 0;
const unsigned int pwmLoopTimerStart = 1;
const unsigned int repeatStart = 200;
const unsigned char stepSize = 4;
boolean pos;

void setup() {
  servo.attach( servoPin ); 
  servo.setMaximumPulse( 2400 );
  servo.setMinimumPulse( 900 ); 
  servo.write( pwm );
  
  hc05.begin( 9600 );
  hc05.println( "Bluetooth On please press 1 or 0 blink LED .." );
  pinMode( ledpin, OUTPUT );
}

void loop() {
  // put your main code here, to run repeatedly:
  if ( hc05.available() ) { // Handle new bluetooth commands
    BluetoothData= hc05.read();
    if ( BluetoothData == '1' ) {   // if number 1 pressed ....
      digitalWrite( ledpin, 1 );
//      hc05.println( "LED  On D13 ON ! " );
//      hc05.println( "+" );
      pos = true;
      repeatTimer = repeatStart; // Set the loop count for the next command.
      if ( 0 == pwmLoopTimer ) { // Update only if not already running
        pwmLoopTimer = 1; // Initialise with 1 to get a fast response on the first push.
      }
    }
    if (BluetoothData == '0') {// if number 0 pressed ....
      digitalWrite(ledpin,0);
//      hc05.println("LED  On D13 Off ! ");
//      hc05.println( "-" );
      pos = false;
      repeatTimer = repeatStart;
      if ( 0 == pwmLoopTimer ) { // Update only if not already running
        pwmLoopTimer = 1; // Initialise with 1 to get a fast response on the first push.
      }
    }
    if ( pwmLoopTimer ) { // When the loop timer is active an the movment commands are acitve
      pwmLoopTimer--;
      if ( 0 == pwmLoopTimer ) { // Each time the loop timer counts out the PWM shall be updated.
        if ( pos ) {
          pwm += stepSize;
        }
        else {
          pwm -= stepSize;
        }
        servo.write( pwm );
        pwmLoopTimer = pwmLoopTimerStart; 
      }
    }
    if ( repeatTimer ) { // The blue tooth command is active
      repeatTimer--;
      if ( 0 == repeatTimer ) { // Repeat has not been renewed.
        pwmLoopTimer = 0; // Stop all PWM increments and decrements
      }
    }      
  }
  servo.refresh();
//  delay(1);// prepare for next data ...
}


