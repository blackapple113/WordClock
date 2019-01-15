#include <Ticker.h>
#include "DCF77.hpp"

using uint = unsigned int;

void Time::increaseTimeByASecond() {
  if (currTime.seconds == 59) {
    currTime.seconds = 0;
    if (currTime.minutes == 59) {
      currTime.minutes = 0;
      if (currTime.hours == 23)
        currTime.hours = 0;
      else
        ++currTime.hours;
    } else
    ++currTime.minutes;
  } else
  ++currTime.seconds;

  Serial.print("Time: ");
  Serial.print(currTime.hours);
  Serial.print(":");
  Serial.print(currTime.minutes);
  Serial.print(":");
  Serial.println(currTime.seconds);
}

void Time::setup() {
  // increase the time by a second every second
  timer.attach(1, increaseTimeByASecond);
}

uint DCF77Interpreter::evaluateData(uint const* data) {
  uint ones{0};
  for (std::size_t i{0}; i < 100; ++i) {
    if (data[i] == 1)
      ++ones;
  }
  if (5 < ones && ones < 15)
    return 0;
  else if (15 < ones && ones < 25)
    return 1;
  else if (ones < 5)
    return 2;
  else
    return 3;
}

void DCF77Interpreter::interpret() {
  static uint rawData[100]; // data that is read every 10ms
  static uint rCounter{0}; // counter for above array
  
  static bool data[36]; // the values for seconds
  static uint counter{0}; // counter for above array
  static bool readMinute{false}; // true when the beginning of a minute is found
  
  rawData[rCounter++] = digitalRead(sensorPin);

  if (rCounter == 100) { // this condition is true every second
    rCounter = 0;
    ++counter;
    
    uint eval{evaluateData(rawData)};
    switch(eval) {
      case 0:
        data[counter++] = 0;
        break;
      case 1:
        data[counter++] = 1;
        break;
      case 2:
        readMinute = true;
        break;
      case 3:
        readMinute = false;
        counter = 0;
        break;
    }
    Serial.print("Eval: ");
    Serial.println(eval);
  }
  if (counter == 36) { // this condition is true once a full minute of data was read
    counter = 0;

    // calculate the minute parity
    uint mParity{data[27] + data[26] + data[25] + data[24] + data[23] + data[22] + data[21]};
    if (mParity % 2 == 1)
      mParity = 1;
    else
      mParity = 0;

    // calculate the hour parity
    uint hParity{data[34] + data[33] + data[32] + data[31] + data[30] + data[29]};
    if (hParity % 2 == 1)
      hParity = 1;
    else
      hParity = 0;

    // if parities are correct set the new time
    if (mParity == data[28] && hParity == data[35]) {
      Time::currTime.minutes = 40 * data[27] + 20 * data[26] + 10 * data[25] + 8 * data[24] + 4 * data[23] + 2 * data[22] + 1 * data[21];
      Time::currTime.hours = 20 * data[34] + 10 * data[33] + 8 * data[32] + 4 * data[31] + 2 * data[30] + 1 * data[29];
    } else {
      Serial.println("Parity failed!");
    }
  }
}

void DCF77Interpreter::setup(int pin) {
  // read data from the dcf77 every 10 milliseconds and update the time accordingly
  sensorPin = pin;
  interpreter.attach_ms(10, interpret);
}
