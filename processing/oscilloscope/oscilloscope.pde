  

// Example by Tom Igoe

import processing.serial.*;

Serial myPort;  // The serial port

void setup() {
  size( 256, 256 );
  background(200);

  // List all the available serial ports:
  printArray(Serial.list());
  // Open the port you are using at the rate you want:
  myPort = new Serial(this, Serial.list()[32], 115200);
//  myPort.write(65);
}

void draw() {
  int state = 0;
  int count = 0;
    
  while (myPort.available() > 0) {
    // Expand array size to the number of bytes you expect:
    char inBuffer;
    inBuffer =  myPort.readChar();
    if (inBuffer == 0xff ) { // Start char 
      state = 1; count = 0 ; 
      //println("START"); 
      background(200);
    }
    else if ( ( inBuffer != 0xff ) && (state == 1 ) ) {
      set( count, inBuffer, color(0) );
 //     print(  (int)inBuffer );
 //     print( ":" );
      count++;
      if ( count == 256 ) {
        state = 2;
 //       println("END");
      }
    }
  }
}