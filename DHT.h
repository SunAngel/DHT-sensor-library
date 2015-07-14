#ifndef DHT_H
#define DHT_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/* DHT library

MIT license
written by Adafruit Industries
*/

// how many timing transitions we need to keep track of. 2 * number bits + extra
//#define MAXTIMINGS 85
// how many timing transitions we need to keep track of. number bits + extra
#define MAXTIMINGS 43

#define DHT11 11
#define DHT22 22
#define DHT21 21
#define AM2301 21

class DHT {
 private:
  uint8_t data[6];
  uint8_t _pin, _type;
  unsigned long _lastreadtime;
  boolean firstreading;

 public:
  DHT(uint8_t pin, uint8_t type);
  void begin(void);
  int16_t readTemperature(void);
  int16_t readHumidity(void);
  boolean read(void);
  uint8_t getPin() { return _pin; }

  static const int16_t badTemperature = -1000;
  static const int16_t badHumidity = -1000;

};
#endif
