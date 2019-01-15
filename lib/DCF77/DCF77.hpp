#include <Ticker.h>
#pragma once

/*
Usage: Insert this into the main setup() function.
       sensorPin is the Pin that the DCF77 Module
       is connected to.

void setup() {
  pinMode(sensorPin, INPUT_PULLUP);
  Time::setup(sensorPin);
  DCF77Interpreter::setup();
}
 */

using uint = unsigned int;

namespace Time {
  Ticker timer;
  
  struct Time{
    uint hours;
    uint minutes;
    uint seconds;
  
    Time()
    : hours{0}, minutes{0}, seconds{0} {}
  };
  Time currTime;
  
  void increaseTimeByASecond() ;
  
  void setup() ;
};

namespace DCF77Interpreter {
  Ticker interpreter;
  int sensorPin = 4;    // select the input pin for the potentiometer
  
  uint evaluateData(uint const* data) ;
  
  void interpret() ;
  
  void setup(int pin) ;
};
