#include "LedControl.h" 

// Pin 7 to Data In, 6 to Clk, 5 to LOAD, number of devices is 3 
LedControl lc=LedControl(12,11,10,3);

void setup() { 
  // Initialize the 3 MAX7219 devices 
  for(int k=0; k<3; k++){ 
    lc.shutdown(k,false); // Enable display 
    lc.setIntensity(k,15); // Set brightness level (0 is min, 15 is max) l
    c.clearDisplay(k); // Clear display register 
  } 
} 

void loop() { 
  int count = 0; 
  for(int j=0; j<3; j++) { 
    for(int i=0; i<8; i++) {
      lc.setDigit(j,7-i,count, true); // Decimal point enabled 
      count ++; 
      if(count == 16) count=0; 
    } delay(1000); 
  } 
} // - See more at: http://embedded-lab.com/blog/?p=6862#sthash.ePNXcHHe.dpuf
