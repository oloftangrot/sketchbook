/*
  morseTrainer
 */

#define MAXEVENT (200)
const unsigned char KEY = 2; // digital pin 2 has a pushbutton attached to it. Give it a name:
const unsigned char KEY_OUT = 5;
const unsigned char HOST_OUT = 6;
const unsigned char ON_DUTYCYCLE = 100;
const unsigned char XON_SIZE = 20;
const unsigned char XOFF_SIZE = MAXEVENT - XON_SIZE;
const char XOFF = 19;
const char XON = 17;
const char LF = 10; // Ctrl-J
int count = 0;
int xoff_cnt = 0;
int rise_cnt = 0;
int owerflow_cnt = 0;
int q_max = 0;
unsigned char q_in=0;
unsigned char q_out=0;
unsigned char q_size = 0;
unsigned char flankDetector;

void test(void);
void pullQueue(unsigned char flankDet);

struct {
  boolean b;
  int time ;
} queue[MAXEVENT];

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);
  pinMode(KEY, INPUT);
  pinMode(KEY_OUT, OUTPUT);
  pinMode(HOST_OUT, OUTPUT);
  Serial.write( ">");
  flankDetector = 0;
}

int getTime(void) {
  int t = 0;
  short c = Serial.read();
  do {
    t = 10*t+c-'0';
    Serial.read();
  } while ('\n' != c);
  return t;
}

unsigned long t_change, t_next = 0, t0 = 0;
unsigned long t=0;
boolean out_state = false,on = false, off = false;
boolean rx = false;

// the loop routine runs over and over again forever:
void loop() {
  flankDetector = 3 & ( flankDetector << 1) ;
//  test();
  // read the input pin:
  if ( digitalRead( KEY ) ) {
    analogWrite( KEY_OUT, ON_DUTYCYCLE );
    flankDetector |=1;
  }
  else {
    analogWrite( KEY_OUT, 0 );
  }
    
  // print out the state of the button:
  if ( millis() >= t_next ) {
    pullQueue( flankDetector );
    if ( XON_SIZE == q_size ) Serial.write( XON );
  }
}

void pullQueue( unsigned char flankDet )
{
  if ( q_size ) {
    out_state = queue[q_out].b;
    if ( out_state )
      analogWrite( HOST_OUT, ON_DUTYCYCLE );
    else
      analogWrite( HOST_OUT, 0 );
    t_change = millis() - t0; // Store set time.
    Serial.write('#');
    Serial.println( t_change );
    Serial.print( out_state );
    Serial.println( "%" );
    if ( 1 == flankDet ) Serial.println( "$1" );
    if ( 2 == flankDet ) Serial.println( "$0" );
    t_next = t_change + queue[q_out].time;
    
    q_out++;
    q_size--;
    if ( q_out >= ( MAXEVENT - 1 ) ) q_out = 0; // Wrap pointer. 
  }
  else { // Queue is empty.
    t_next = millis();
    if ( 1 == flankDet ) {
      Serial.write('#');
      Serial.println( t_next );
      Serial.println( "$1" );
    }
    if ( 2 == flankDet ) {
      Serial.write('#');
      Serial.println( t_next );
      Serial.println( "$0" );
    }
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  char c = (char) Serial.read(); 
  
  count++;
  if ('/' == c ) {
    rise_cnt++;
    queue[q_in].b = true;
    t = 0; // Clear time for next frame.
    rx = true;
//    Serial.write('H');
 
  } else if ('\\' == c ) {
    queue[q_in].b = false;
    t = 0; // Clear time for next frame.
    rx = true;
//    Serial.write('L');
  }
  else if ( rx && ( c>='0' ) && ( c <= '9' ) ) {
    t = t*10+c-'0';
//    Serial.write('.');
  }
  else if ( rx && ( c == '\n' )) {
    rx = false;
    queue[q_in++].time = t;
    q_size++;
    if ( q_size > q_max ) q_max = q_size;
    if ( q_in >= ( MAXEVENT - 1) ) q_in = 0;
    if ( XOFF_SIZE == q_size ) {
      Serial.write( XOFF );
      Serial.write( XOFF );
      xoff_cnt++;
    }
//  Serial.write('E');
  }
  else if ( '?' == c ) {
    Serial.println( count );
    Serial.println( xoff_cnt );
    Serial.println( rise_cnt );
    Serial.println( q_max );
  }
//  else Serial.write(c);
}

void test(void) {
  for(;;) {
    digitalWrite( KEY_OUT, true );  
    delay(1);
    digitalWrite( KEY_OUT, false );
    delay(1);  
  }
}

