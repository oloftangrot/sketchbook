/*
  morseTrainer
 */

// digital pin 2 has a pushbutton attached to it. Give it a name:
const unsigned char key = 2;
const unsigned char key_out = 5;
const unsigned char host_out = 6;
const unsigned char ON_DUTYCYCLE = 100;
int count = 0;
int xoff_cnt = 0;
int rise_cnt = 0;

void test(void);

#define MAXEVENT (100)
struct {
  boolean b;
  int time ;
} queue[MAXEVENT];

unsigned char q_in=0;
unsigned char q_out=0;
unsigned char q_size = 0;
const unsigned char XON_SIZE = 20;
const unsigned char XOFF_SIZE = MAXEVENT - XON_SIZE;
const char XOFF = 19;
const char XON = 17;
const char LF = 10; // Ctrl-J

void pullQueue(void);

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);
  pinMode(key, INPUT);
  pinMode(key_out, OUTPUT);
  pinMode(host_out, OUTPUT);
  analogWrite( host_out, 0);
  analogWrite( key_out, 0);
  Serial.write( ">");
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
//  test();
  // read the input pin:
  if ( digitalRead( key ) ) {
    analogWrite( key_out, ON_DUTYCYCLE );
  }
  else {
    analogWrite( key_out, 0 );
  }
  
  // print out the state of the button:
  if ( millis() >= t_next ) {
    pullQueue();
    if ( XON_SIZE == q_size ) Serial.write( XON );
  }
}

void pullQueue( void )
{
  if ( q_size ) {
    out_state = queue[q_out].b;
    if ( out_state )
      analogWrite( host_out, ON_DUTYCYCLE );
    else
      analogWrite( host_out, 0 );
    t_change = millis() - t0; // Store set time.
    Serial.write('#');
    Serial.println( t_change );
    Serial.print( out_state );
    Serial.println( "%" );
    t_next = t_change + queue[q_out].time;
    
    q_out++;
    q_size--;
    if ( q_out >= MAXEVENT ) q_out = 0; // Wrap pointer. 
  }
  else { // Queue is empty.
    t_next = millis();
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
  else if ( rx && ( c == '*' )) {
    rx = false;
    queue[q_in++].time = t;
    q_size++;
    if ( q_in >= MAXEVENT ) q_in = 0;
    if ( XOFF_SIZE == q_size ) {
      Serial.write( XOFF );
      xoff_cnt++;
    }
//  Serial.write('E');
  }
  else if ('?' == c ) {
    Serial.println( count );
    Serial.println( xoff_cnt );
    Serial.println( rise_cnt );
  }
//  else Serial.write(c);
}

void test(void) {
  for(;;) {
    digitalWrite(key_out,true);  
    delay(1);
    digitalWrite(key_out,false);
    delay(1);  
  }
}

