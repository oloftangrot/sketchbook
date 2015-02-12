// This program shown how to control arduino from PC or Android Via Bluetooth
// Connect ...
// arduino>>bluetooth
// D11   >>>  Rx
// D10   >>>  Tx


#include <SoftwareSerial.h>// import the serial library

SoftwareSerial hc05( 10, 11 ); // RX, TX
int ledpin = 13; // led on D13 will show blink on / off
int BluetoothData; // the data given from Computer

void setup() {
  // put your setup code here, to run once:
  hc05.begin(9600);
  hc05.println("Bluetooth On please press 1 or 0 blink LED ..");
  pinMode(ledpin,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if ( hc05.available() ) {
    BluetoothData= hc05.read();
    if ( BluetoothData == '1') {   // if number 1 pressed ....
      digitalWrite( ledpin, 1 );
      hc05.println( "LED  On D13 ON ! " );
    }
    if (BluetoothData == '0') {// if number 0 pressed ....
      digitalWrite(ledpin,0);
      hc05.println("LED  On D13 Off ! ");
    }
  }
  delay(100);// prepare for next data ...
}


