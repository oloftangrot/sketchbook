/**********************************
* Author: Nathan House            *
* Website: www.foxytronics.com    *
* Modified: 3/21/13               *
**********************************/

const int serialPeriod = 250;       // only print to the serial console every 1/4 second
unsigned long timeSerialDelay = 0;

const int loopPeriod = 20;          // a period of 20ms = a frequency of 50Hz
unsigned long timeLoopDelay   = 0;

// specify the trig & echo pins used for the ultrasonic sensors
const int ultrasonic2TrigPin = 8;
const int ultrasonic2EchoPin = 9;

int ultrasonic2Distance;
int ultrasonic2Duration;


void setup()
{
    Serial.begin(9600);
  
    // ultrasonic sensor pin configurations
    pinMode(ultrasonic2TrigPin, OUTPUT);
    pinMode(ultrasonic2EchoPin, INPUT);
}


void loop()
{
    debugOutput(); // prints debugging messages to the serial console
    
    if(millis() - timeLoopDelay >= loopPeriod)
    {
        readUltrasonicSensors(); // read and store the measured distances
        
        timeLoopDelay = millis();
    }
}


void readUltrasonicSensors()
{
    // ultrasonic 2
    digitalWrite(ultrasonic2TrigPin, HIGH);
    delayMicroseconds(10);                  // must keep the trig pin high for at least 10us
    digitalWrite(ultrasonic2TrigPin, LOW);
    
    ultrasonic2Duration = pulseIn(ultrasonic2EchoPin, HIGH);
    ultrasonic2Distance = (ultrasonic2Duration/2)/29;
}


void debugOutput()
{
    if((millis() - timeSerialDelay) > serialPeriod)
    {
        Serial.print("ultrasonic2Distance: ");
        Serial.print(ultrasonic2Distance);
        Serial.print("cm: ");
        Serial.println();
        
        timeSerialDelay = millis();
    }
}
