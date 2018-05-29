/*
 * GPS.h
 *
 *  Created on: 25.08.2015
 */

#ifndef LIBRARIES_GPS_GPS_H_
#define LIBRARIES_GPS_GPS_H_

#include "TimersClass.h"

class GPS : public TimerClass {
 public:
  GPS(unsigned long interval, HardwareSerial *serial);
  virtual ~GPS();
  void update();
  void debug(HardwareSerial& serial);
  void debugHeader(HardwareSerial& serial);

 private:
  String m_position;
  HardwareSerial *m_serial;
  static const int m_maxLength = 100;
};

#endif /* LIBRARIES_GPS_GPS_H_ */
