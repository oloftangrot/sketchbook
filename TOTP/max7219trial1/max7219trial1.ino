#include "LedControl.h" 

// Pin 12 to Data In, 10 to Clk, 11 to LOAD, number of devices is 1 
LedControl lc=LedControl(12,10,11,1);

void setup() { 
  // Initialize the MAX7219 devices 
  for(int k=0; k < 2; k++){ 
    lc.shutdown(k,false); // Enable display 
    lc.setIntensity(k,2); // Set brightness level (0 is min, 15 is max)
    lc.clearDisplay(k); // Clear display register 
  } 
} 
unsigned int cnt = 0;
void loop()
{
  unsigned int tmp = cnt++;
  unsigned int b;
  for( int i = 0; i < 8; i++ ) {
    b = tmp % 10;
    tmp = tmp / 10;
   lc.setDigit(0,i, b,false);
  }  
  delay(10);
}
// - See more at: http://embedded-lab.com/blog/?p=6862#sthash.ePNXcHHe.dpuf
