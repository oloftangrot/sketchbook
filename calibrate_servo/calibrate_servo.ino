#include <Servo.h>

// Hitta nollpunkten för servon, FRO Ungdom, Oktober 2013

// Konstanter
#define STARBOARD_SERVO   9
#define PORTSIDE_SERVO    10


#define STARBOARD_ZERO   88  // utprovat
#define PORTSIDE_ZERO    89  // utprovat

// Servon
int starboard_value = STARBOARD_ZERO;
int portside_value = PORTSIDE_ZERO;
Servo StarboardServo;
Servo PortsideServo;


void setup() {
  StarboardServo.attach(STARBOARD_SERVO);
  StarboardServo.write(starboard_value);
  PortsideServo.attach(PORTSIDE_SERVO);
  PortsideServo.write(portside_value);
  Serial.begin(9600);
  printStatus();
}

void loop() {
  int incomingByte;
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
}

void drive(int starboard, int portside) {
  StarboardServo.write(starboard);
  PortsideServo.write(portside);
  printStatus();   
}

void printStatus() {
  Serial.print("> Starboard servo=");
  Serial.print(starboard_value);
  Serial.print(". Portside : servo=");
  Serial.print(portside_value);
  Serial.println(".");
}  
