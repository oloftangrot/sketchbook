#include "LedControl.h" 

// Pin 12 to Data In, 10 to Clk, 11 to LOAD, number of devices is 2 
LedControl lc=LedControl(12,10,11,2);

void setup() { 
  // Initialize the MAX7219 devices 
  for(int k=0; k < 2; k++){ 
    lc.shutdown(k,false); // Enable display 
    lc.setIntensity(k,2); // Set brightness level (0 is min, 15 is max)
    lc.clearDisplay(k); // Clear display register 
  } 
} 

void loop() { 
  int count = 0; 

  for(int j=0; j < 2; j++) { 
    for(int i=0; i<8; i++) {
      lc.setDigit(j,7-i,count, true); // Decimal point enabled 
      count ++; 
      if(count == 16) count=0; 
    } delay(1000); 
  } 
} // - See more at: http://embedded-lab.com/blog/?p=6862#sthash.ePNXcHHe.dpuf
