#include <SPI.h>
#include <ELECHOUSE_CC1101.h>

#define size 61

byte TX_buffer[size] = { 0 };
byte i;
int cnt = 0;

void setup()
{
  Serial.begin(9600);
  Serial.print( "Hello from CC1101 TX");
  ELECHOUSE_cc1101.Init();
  for( i = 0; i < size; i++ ) {
     TX_buffer[i] = i;
  }
}

void loop()
{
  ELECHOUSE_cc1101.SendData(TX_buffer,size);
  TX_buffer[0]++;
  cnt++;
  Serial.print( cnt );
  Serial.print( " " );
  delay(1000);

}
