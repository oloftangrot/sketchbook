#ifndef RunningAverage_h
#define RunningAverage_h
//
//    FILE: RunningAverage.h
//  AUTHOR: Rob dot Tillaart at gmail dot com
// PURPOSE: RunningAverage library for Arduino
//     URL: http://arduino.cc/playground/Main/RunningAverage
// HISTORY: See RunningAverage.cpp
//
// Released to the public domain
//

// backwards compatibility
// clr() clear()
// add(x) addValue(x)
// avg() getAverage()

#define RUNNINGAVERAGE_LIB_VERSION "0.2.05"

typedef unsigned short buff_t	;
typedef unsigned char input_t; 

#include "Arduino.h"

class RunningAverage
{
public:
    RunningAverage( void );
    RunningAverage( uint8_t );
    ~RunningAverage();

    void clear();
    void addValue( input_t );
    void fillValue( input_t );

    input_t getAverage();

    input_t getElement( uint8_t idx );
    uint8_t getSize() { return _size; }

protected:
    uint8_t _size;
    uint8_t _idx;
    buff_t   _sum;
    input_t * _ar;
};

#endif
// END OF FILE
