/*
  morseTrainer
 */

#define MAXEVENT (100)
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

void test(void);
void pullQueue(void);

struct {
  boolean b;
  int time ;
} queue[MAXEVENT];

unsigned char m;

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);
  pinMode(KEY, INPUT_PULLUP);
  Serial.write( ">");
  m = 0;
}


// the loop routine runs over and over again forever:
void loop() {
  m = m<<1;
  // read the input pin:
  m |= digitalRead( KEY );
  m &= 3;
  if (1 == m) { // Rising flank
    Serial.write( XOFF );
//      Serial.write( '%' );
  }
  if (2 == m ) { // Faling flank;
    Serial.write( XON );
//    Serial.write( '@' );
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
  
  Serial.write(c);
}


