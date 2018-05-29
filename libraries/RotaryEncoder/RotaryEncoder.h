/*
 * RotaryEncoder.h
 *
 *  Created on: 25.08.2015
 */

#ifndef LIBRARIES_ROTARYENCODER_H_
#define LIBRARIES_ROTARYENCODER_H_
#include "TimersClass.h"

class RotaryEncoder : public TimerClass {
 public:
  RotaryEncoder(unsigned long interval, int pinA, int pinB,
                int clearing = 1.3 * 1024 / 5);
  virtual ~RotaryEncoder();

  void update();
  int getCurrentPosition();
  void setCurrentPosition(int pos);
  void debug(HardwareSerial& serial);
  void debugHeader(HardwareSerial& serial);

 private:
  int m_pinA;
  int m_pinB;

  int m_voltageA;
  int m_voltageB;
  int m_clearing;  // if measured voltage changes (to the right direction) by
                   // more than this the state is swapped
  bool m_aHigh;
  bool m_bHigh;
  int m_currentPosition;
  friend class UI;
};

#endif /* LIBRARIES_ROTARYENCODER_H_ */
