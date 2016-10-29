#include <RunningAverage.h>

RunningAverage ch[2];

RunningAverage chX(4);
void setup() {
  Serial.begin( 9600 );
  // put your setup code here, to run once:
  for ( int i = 0; i < 2; i++ ) {
    if ( !ch[i].setSize( 4 ) ) {
      Serial.println( "Fail i setup!" );
    }
  }
}

void loop() {
  #if 0
  Serial.println( ch[0].getAverage() );
  Serial.println( ch[1].getAverage() );
  // put your main code here, to run repeatedly: 
  for( int i = 0; i < 8;i++ ) ch[0].addValue( i +1 );
  Serial.println( ch[0].getAverage() );
  Serial.println( ch[1].getAverage() );
  #else
  Serial.println( chX.getAverage() );
  // put your main code here, to run repeatedly: 
  for( int i = 0; i < 8;i++ ) chX.addValue( i +1 );
  Serial.println( chX.getAverage() );
  #endif
  delay( 10000 );  
}
