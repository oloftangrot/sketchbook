#include <Servo.h>

// Hitta nollpunkten för servon, FRO Ungdom, Oktober 2013

class Servo_Ext : public Servo {
public:
  void initiate( int s, int v ) { sign = s; current_value = v; };
  void update_pos( int i) {
    current_value = i * sign + current_value;
    this->write( current_value );
  };
  int get_current( void ) { return current_value; };
private:
  int sign;
public:
  int current_value;
};

class Axis {
public:
  Axis(Servo_Ext& left, Servo_Ext& right) : l(left), r(right) {};
  void lin_speed( int i ) {
    l.update_pos( i );
    r.update_pos( i );
  };
  void rot_speed( int i ) {
    l.update_pos( i );
    r.update_pos( -i );
  };

private:
  Servo_Ext& l;
  Servo_Ext& r;
  Axis(); // Hide default constructor
};

// Konstanter
#define STARBOARD_SERVO   9
#define PORTSIDE_SERVO    10
#define LED 13
#define STARBOARD_SW      3
#define PORTSIDE_SW       4

#define STARBOARD_ZERO   88  // utprovat
#define PORTSIDE_ZERO    89  // utprovat

// Servon
int starboard_value = STARBOARD_ZERO;
int portside_value = PORTSIDE_ZERO;
Servo_Ext StarboardServo;
Servo_Ext PortsideServo;
Axis      myAxis( StarboardServo, PortsideServo );
const int tLinScale = 100;
const int fwdSpeed = 10;
const int rewSpeed = -10;
const int tRotScale = 100;
const int leftSpeed = -10;
const int rightSpeed = 10;

void setup() {
  pinMode( STARBOARD_SW, INPUT );
  pinMode( PORTSIDE_SW, INPUT );
  pinMode( LED, OUTPUT );

  StarboardServo.attach( STARBOARD_SERVO );
  StarboardServo.write( starboard_value );
  StarboardServo.initiate( 1, STARBOARD_ZERO );
  
  PortsideServo.attach( PORTSIDE_SERVO );
  PortsideServo.initiate( -1, PORTSIDE_ZERO );
  PortsideServo.write( portside_value );
  Serial.begin( 9600 );
  printStatus();
}

void loop() {
  int incomingByte;
  int starboard_in, portside_in;
#if 0  
  if(Serial.available() > 0) {
    incomingByte = Serial.read();
    if(incomingByte=='q') {
        portside_value++;
    } else if(incomingByte=='z') {
        portside_value--;
    } else if(incomingByte=='w') {
        starboard_value--;
    } else if(incomingByte=='x') {
        starboard_value++;
    }    
    drive(starboard_value,portside_value);  
  }  
#else

/*  
  starboard_in = digitalRead( STARBOARD_SW );
  portside_in = digitalRead( PORTSIDE_SW );
  if ( starboard_in || portside_in ) {
    digitalWrite( LED, 1 );
  }
  else {
    digitalWrite( LED, 0 );
  }
*/

  delay( 500);
  fram( 40 );
  delay( 500 );
  fram( 25 );
  delay( 500 );
  hoger( 5);
   delay( 500 );
  delay( 500 );
  fram  ( 200);
  for (;;);  // Wait for ever.
#endif
  
}

void drive(int starboard, int portside) {
  StarboardServo.write(starboard);
  PortsideServo.write(portside);
  printStatus();   
}

void fram( int t )
{
  myAxis.lin_speed( fwdSpeed );
  delay( t*tLinScale );
  myAxis.lin_speed( -fwdSpeed );
}

void bak( int t )
{
  myAxis.lin_speed( rewSpeed );
  delay( t*tLinScale );
  myAxis.lin_speed( -rewSpeed );
}

void vanster( int t ) {
  myAxis.rot_speed( leftSpeed );
  delay( t*tRotScale );
  myAxis.rot_speed( -leftSpeed );
}

void hoger( int t ) {
  myAxis.rot_speed( rightSpeed );
  delay( t*tRotScale );
  myAxis.rot_speed( -rightSpeed );
}

void printStatus() {
  Serial.print("> Starboard servo=");
  Serial.print( StarboardServo.get_current() );
  Serial.print(". Portside : servo=");
  Serial.print( PortsideServo.get_current() );
  Serial.println(".");
}  
