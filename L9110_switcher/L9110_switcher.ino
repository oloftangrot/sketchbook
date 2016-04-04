/*
  HV supply switcher using the L9110 dual H-bridge.
  Use the modified sinus method.
 */
 
const unsigned char a = 13;
const unsigned char b = 12;
unsigned char phase = 0;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode( a, OUTPUT);     
  pinMode( b, OUTPUT);     
}

// the loop routine runs over and over again forever:
void loop() {
  unsigned char r = phase % 4;
  switch( r )  {
    case 0:
      digitalWrite( a, HIGH ); 
      break;
    case 1:
      digitalWrite( b, HIGH ); 
      break;
    case 2:
      digitalWrite( a, LOW ); 
      break;
    case 3:
      digitalWrite( b, LOW ); 
      break;
    default:
      break;
  }  
  phase++;
  delay( 5 ); // wait for a 100 ms

}
