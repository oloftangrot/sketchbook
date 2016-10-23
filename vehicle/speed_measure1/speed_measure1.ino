
char in_clock_rsync;
boolean rise_edge;
unsigned long count;

void setup() {
  Serial.begin(9600);
  pinMode( 9, INPUT );
}

void loop() {
  unsigned long t = meas();
  Serial.println( t );
}

unsigned long meas( void )
{
  unsigned long res;
  in_clock_rsync = digitalRead( 9 );
//  Serial.println( "Rsync " );
//  Serial.println( (int) in_clock_rsync );
  do {
    in_clock_rsync <<= 1; 
    in_clock_rsync |= digitalRead( 9 );
    in_clock_rsync &= 3; // Clear the higher bits
  } while ( in_clock_rsync != 1 );  // A rising edge is detected
  unsigned long tmp = micros();
  do {
    in_clock_rsync <<= 1; 
    in_clock_rsync |= digitalRead( 9 );
    in_clock_rsync &= 3; // Clear the higher bits
  } while ( in_clock_rsync != 1 ); // A rising edge is detected
  unsigned long tmp2 = micros();
    if ( tmp2 > tmp ) 
      res = tmp2 - tmp;
    else
      res = tmp - tmp2;
//  Serial.println( "tmp " );
//  Serial.println( tmp );
//  Serial.println( "tmp2 " );
//  Serial.println( tmp2 );
  
  
  return res;
}
