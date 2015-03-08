#include "calibrate.h"

// calibration points for 240x400 screen
const POINT cal_pa[3] = { // See application note Atmel AVR341
  { 120, 360 },   // P1 : 50% max height, max width - 10 %
  { 216, 200 },   // P2 : Max height - 10 %, 50% max widht
  { 24, 40 }      // P3 : 10%, 10 %
};
POINT res_pa[3]; // Result from screen read back 
MATRIX M;

void doCalibration( void )
{
  setCalibrationMatrix( cal_pa, res_pa, &M );
  
}

void setup()
{
}

void loop() {
}

