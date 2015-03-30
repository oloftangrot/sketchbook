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
#include <Servo.h>

const int servoPin = 12;
//SoftwareSerial hc05( 10, 11 ); // RX, TX
#define hc05 Serial1

int ledpin = 13; // led on D13 will show blink on / off
int BluetoothData; // the data given from Computer
Servo servo;
int pwm = 90;

void setup() {
  servo.attach( servoPin ); 
//  servo.setMaximumPulse( 2400 );
//  servo.setMinimumPulse( 900 ); 
  servo.write( pwm );
  
  hc05.begin( 9600 );
  hc05.println( "Bluetooth On please press 1 or 0 blink LED .." );
  pinMode( ledpin, OUTPUT );
}

void loop() {
  // put your main code here, to run repeatedly:
  if ( hc05.available() ) {
    BluetoothData= hc05.read();
    if ( BluetoothData == '1' ) {   // if number 1 pressed ....
      digitalWrite( ledpin, 1 );
//      hc05.println( "LED  On D13 ON ! " );
//      hc05.println( "+" );
      pwm++;
      servo.write( pwm );
    }
    if (BluetoothData == '0') {// if number 0 pressed ....
      digitalWrite(ledpin,0);
//      hc05.println("LED  On D13 Off ! ");
//      hc05.println( "-" );
      pwm--;
      servo.write( pwm );
    }
  }
//  servo.refresh();
  delay(100);// prepare for next data ...
}


