#include <Skipjack.h>

Skipjack sj;

void setup() {
  Serial.begin(9600);
  Serial.println("Hello!");
}

void loop() {
  skipJackTest();
  delay(2000);
}

void skipJackTest( void ) { // Note data and key are little endian
  byte data[] = { 0xaa, 0xbb, 0xcc, 0xdd, 0x00, 0x11, 0x22, 0x33 }; // NIST test vector
  byte sj_key[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x00 };// 80-bit skipjack key NIST

  Serial.println();
  Serial.println( "========= Skipjack test ==========" );
  //encrypt
  Serial.print("Encrypt...");
  long time = millis();
  sj.encrypt( data, sj_key);
  time = millis() - time;
  Serial.print("done. (");
  Serial.print(time);
  Serial.println(" ms)");
  printArray( data ); // Expected out is 2587cae27a12d30, note that Highest byte come last in the output
  
  //decrypt
  Serial.print("Decrypt...");
  time = millis();
  sj.decrypt( data, sj_key);
  time = millis() - time;
  Serial.print("done. (");
  Serial.print(time);
  Serial.println(" ms)");
  printArray( data );
}

void printArray(byte output[])
{
  for (int i = 0; i < 8; i++)
  {
    if (output[i] < 0x10)
    {
      Serial.print("0");
    }
    Serial.print(output[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}
